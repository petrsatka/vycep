/*
By Petr Satka
Licnese CC-BY
*/

//Ovládání ventilu
#include "Valve.h"

Valve::Valve(SemaphoreHandle_t xSemaphore, const char* nvsParttionName) {
  this->xSemaphore = xSemaphore;
  valveStateStorage = new TSafePreferences(this->xSemaphore, NAMESPACE_VALVE_STATE, nvsParttionName);
}

Valve::~Valve() {
  delete (valveStateStorage);
}

//Inicializace čítače pulsů
void Valve::initPulseCounter(int16_t pulsesPerServing, int pinNumber) {
  sprintln("!initPulseCounter");
  pcnt_config_t pcntFreqConfig = {};
  pcntFreqConfig.pulse_gpio_num = pinNumber;        // pin assignment for pulse counter
  pcntFreqConfig.pos_mode = PCNT_COUNT_INC;         //režim přičítání na náběžné hraně
  pcntFreqConfig.counter_h_lim = pulsesPerServing;  //horní limit počítání maximálně 32767
  pcntFreqConfig.unit = PCNT_MAIN_UNIT;
  pcntFreqConfig.channel = PCNT_CHANNEL_0;
  pcnt_unit_config(&pcntFreqConfig);

  pcnt_counter_pause(PCNT_MAIN_UNIT);
  pcnt_counter_clear(PCNT_MAIN_UNIT);

  pcnt_event_enable(PCNT_MAIN_UNIT, PCNT_EVT_H_LIM);         // nastavení události při dosažení horního limitu
  pcnt_isr_register(onServingReachedStatic, this, 0, NULL);  // registrace handleru
  pcnt_intr_enable(PCNT_MAIN_UNIT);                          // zapnutí přerušení

  //Glitch filter - doladit s reálným průtokoměrem
  pcnt_set_filter_value(PCNT_MAIN_UNIT, 1023);
  pcnt_filter_enable(PCNT_MAIN_UNIT);
}

void Valve::initCalibrationPulseCounter(int pinNumber) {
  sprintln("!initCalibrationPulseCounter");
  pcnt_config_t pcntFreqConfig = {};
  pcntFreqConfig.pulse_gpio_num = pinNumber;  // pin assignment for pulse counter
  pcntFreqConfig.pos_mode = PCNT_COUNT_INC;   //režim přičítání na náběžné hraně
  pcntFreqConfig.unit = PCNT_CALIB_UNIT;
  pcntFreqConfig.channel = PCNT_CHANNEL_0;
  pcnt_unit_config(&pcntFreqConfig);

  pcnt_counter_pause(PCNT_CALIB_UNIT);
  pcnt_counter_clear(PCNT_CALIB_UNIT);

  //Glitch filter - doladit s reálným průtokoměrem
  pcnt_set_filter_value(PCNT_CALIB_UNIT, 1023);
  pcnt_filter_enable(PCNT_CALIB_UNIT);
}

void Valve::startCalibration() {
  sprintln("!startCalibration");
  if (actualMode != Settings::DeviceMode::CALIBRATION) {
    permanentlyCloseValve();
    initCalibrationPulseCounter(this->flowMeterPinNumber);
    pcnt_counter_resume(PCNT_CALIB_UNIT);
    permanentlyOpenValve();
    actualMode = Settings::DeviceMode::CALIBRATION;
  }
}

int16_t Valve::stopCalibration(Settings::DeviceMode mode) {
  sprintln("!stopCalibration");
  if (actualMode == Settings::DeviceMode::CALIBRATION) {
    int16_t pulseCount = 0;
    pcnt_counter_pause(PCNT_CALIB_UNIT);
    permanentlyCloseValve();
    pcnt_get_counter_value(PCNT_CALIB_UNIT, &pulseCount);
    setMode(mode);
    return pulseCount;
  }

  return -1;
}

void Valve::setMode(Settings::DeviceMode mode) {
  sprintln("!setMode");
  switch (mode) {
    case Settings::DeviceMode::OPEN:
      permanentlyOpenValve();
      return;
    case Settings::DeviceMode::CLOSED:
      permanentlyCloseValve();
      return;
    case Settings::DeviceMode::TEST:
      setTestMode();
      return;
  }

  setAutoMode();
}

Settings::DeviceMode Valve::getMode() {
  sprintln("!getMode");
  return actualMode;
}

void Valve::configure(uint16_t pulsesPerServing, int flowMeterPinNumber, int valvePinNumber) {
  sprintln("!configure");
  pinMode(valvePinNumber, OUTPUT);
  this->valvePinNumber = valvePinNumber;
  this->flowMeterPinNumber = flowMeterPinNumber;
  initPulseCounter(pulsesPerServing, flowMeterPinNumber);
  shouldClose = false;
  shouldOpen = false;
  if (actualMode == Settings::DeviceMode::AUTO) {
    closeValve();
    //Jsou nějaké objednávky ve frontě?
    //Pokud ano musíme původní roztočenou zrušit, aby nedocházelo k šizení vypnutím těsně před dočepování objednávky
    uint16_t orderCount = valveStateStorage->getUShort(KEY_ORDER_COUNT);
    if (orderCount > 0) {
      valveStateStorage->addUShort(KEY_ORDER_COUNT, -1, 0, orderCount);
    }

    if (orderCount > 0) {
      //Něco zbylo i po odečtení roztočené objednávky
      shouldOpen = true;
    }
  }
}

void Valve::setTestMode() {
  sprintln("!setTestMode");
  if (actualMode != Settings::DeviceMode::TEST) {
    actualMode = Settings::DeviceMode::TEST;  //Od teď nepřijde žádná nová objednávka
    shouldOpen = false;
    closeValve();  //Od teď nepřijde žádné přerušení z čítače
    shouldClose = false;
    valveStateStorage->putUShort(KEY_ORDER_COUNT, 0);  //Reset objednávek
  }
}

bool Valve::makeOrder() {
  sprintln("!makeOrder");
  if (actualMode == Settings::DeviceMode::AUTO || actualMode == Settings::DeviceMode::TEST) {
    uint16_t orderCount = 0;
    valveStateStorage->addUShort(KEY_ORDER_COUNT, 1, 0, orderCount);
    if (orderCount > 0) {
      shouldOpen = true;
      return true;
    }
  }

  return false;
}

void Valve::refresh() {
  if (shouldOpen) {
    shouldOpen = false;
    openValve();
  }

  if (shouldClose) {
    uint16_t orderCount = 0;
    valveStateStorage->addUShort(KEY_ORDER_COUNT, -1, 0, orderCount);
    if (orderCount == 0) {
      closeValve();
    }

    shouldClose = false;
  }
}

void Valve::permanentlyCloseValve() {
  sprintln("!permanentlyCloseValve");
  if (actualMode != Settings::DeviceMode::CLOSED) {
    actualMode = Settings::DeviceMode::CLOSED;  //Od teď nepřijde žádná nová objednávka
    shouldOpen = false;
    closeValve();  //Od teď nepřijde žádné přerušení z čítače
    shouldClose = false;
    valveStateStorage->putUShort(KEY_ORDER_COUNT, 0);  //Reset objednávek
  }
}

void Valve::permanentlyOpenValve() {
  sprintln("!permanentlyOpenValve");
  if (actualMode != Settings::DeviceMode::OPEN) {
    actualMode = Settings::DeviceMode::OPEN;  //Od teď nepřijde žádná nová objednávka
    shouldOpen = false;
    closeValve();  //Od teď nepřijde žádné přerušení z čítače
    shouldClose = false;
    valveStateStorage->putUShort(KEY_ORDER_COUNT, 0);  //Reset objednávek
    digitalWrite(this->valvePinNumber, HIGH);
  }
}

void Valve::setAutoMode() {
  sprintln("!setAutoMode");
  if (actualMode != Settings::DeviceMode::AUTO) {
    closeValve();
    valveStateStorage->putUShort(KEY_ORDER_COUNT, 0);  //Reset objednávek
    actualMode = Settings::DeviceMode::AUTO;
  }
}

void Valve::openValve() {
  sprintln("!openValve");
  if (digitalRead(this->valvePinNumber) == LOW) {
    pcnt_counter_clear(PCNT_MAIN_UNIT);
    pcnt_counter_resume(PCNT_MAIN_UNIT);
    digitalWrite(this->valvePinNumber, HIGH);
  }
}

void Valve::closeValve() {
  sprintln("!closeValve");
  if (digitalRead(this->valvePinNumber) == HIGH) {
    digitalWrite(this->valvePinNumber, LOW);
    pcnt_counter_pause(PCNT_MAIN_UNIT);
    pcnt_counter_clear(PCNT_MAIN_UNIT);
  }
}

//Dosažení porce
void IRAM_ATTR Valve::onServingReachedStatic(void* valve) {
  Valve* that = static_cast<Valve*>(valve);
  that->onServingReached();
}

void IRAM_ATTR Valve::onServingReached() {
  PCNT.int_clr.val = BIT(PCNT_MAIN_UNIT);  // Vymazat příznak přerušení
  pcnt_counter_clear(PCNT_MAIN_UNIT);      //Vynulování čítače
  shouldClose = true;
}

uint16_t Valve::getQueueCount() {
  sprintln("!getQueueCount");
  return valveStateStorage->getUShort(KEY_ORDER_COUNT);
}
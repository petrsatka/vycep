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
  pcnt_config_t pcntFreqConfig = {};
  pcntFreqConfig.pulse_gpio_num = pinNumber;        // pin assignment for pulse counter
  pcntFreqConfig.pos_mode = PCNT_COUNT_INC;         //režim přičítání na náběžné hraně
  pcntFreqConfig.counter_h_lim = pulsesPerServing;  //horní limit počítání maximálně 32767
  pcntFreqConfig.unit = PCNT_UNIT;
  pcntFreqConfig.channel = PCNT_CHANNEL_0;
  pcnt_unit_config(&pcntFreqConfig);

  pcnt_counter_pause(PCNT_UNIT);
  pcnt_counter_clear(PCNT_UNIT);

  pcnt_event_enable(PCNT_UNIT, PCNT_EVT_H_LIM);   // nastavení události při dosažení horního limitu
  pcnt_isr_register(onServingReachedStatic, this, 0, NULL);  // registrace handleru
  pcnt_intr_enable(PCNT_UNIT);                    // zapnutí přerušení

  //Glitch filter - doladit s reálným průtokoměrem
  pcnt_set_filter_value(PCNT_UNIT, 1023);
  pcnt_filter_enable(PCNT_UNIT);
}

void Valve::configure(uint16_t pulsesPerServing, int flowMeterPinNumber, int valvePinNumber) {
  pinMode(valvePinNumber, OUTPUT);
  this->valvePinNumber = valvePinNumber;
  initPulseCounter(pulsesPerServing, flowMeterPinNumber);
  shouldClose = false;
  shouldOpen = false;
  closeValve();
  //Jsou nějaké objednávky ve frontě?
  //Pokud ano musíme původní roztočenou zrušit, aby nedocházelo k šizení vypnutím těsně před dočepování objednávky
  uint16_t orderCount = valveStateStorage->getUShort(KEY_ORDER_COUNT);
  if (orderCount > 0) {
    valveStateStorage->addUShort(KEY_ORDER_COUNT, -1, 0, orderCount);
  }

  pcnt_counter_resume(PCNT_UNIT);  //Zapnutí čítače
  if (orderCount > 0) {
    //Něco zbylo i po odečtení roztočené objednávky
    shouldOpen = true;
  }
}

void Valve::makeOrder() {
  uint16_t orderCount = 0;
  valveStateStorage->addUShort(KEY_ORDER_COUNT, 1, 0, orderCount);
  if (orderCount > 0) {
    shouldOpen = true;
  }
}

void Valve::refresh() {
  if (shouldOpen) {
    shouldOpen = false;
    openValve();
  }

  if (shouldClose) {
    shouldClose = false;
    uint16_t orderCount = 0;
    valveStateStorage->addUShort(KEY_ORDER_COUNT, -1, 0, orderCount);
    if (orderCount == 0) {
      closeValve();
    }
  }
}

void Valve::openValve() {
  digitalWrite(this->valvePinNumber, HIGH);
}

void Valve::closeValve() {
  digitalWrite(this->valvePinNumber, LOW);
  pcnt_counter_clear(PCNT_UNIT);
}

//Dosažení porce
void IRAM_ATTR Valve::onServingReachedStatic(void* valve)
{
    Valve* that = static_cast<Valve*>(valve);
    that->onServingReached();
}

void IRAM_ATTR Valve::onServingReached() {
  PCNT.int_clr.val = BIT(PCNT_UNIT);  // Vymazat příznak přerušení
  pcnt_counter_clear(PCNT_UNIT);      //Vynulování čítače
  shouldClose = true;
}

uint16_t Valve::getOrderCount() {
  return valveStateStorage->getUShort(KEY_ORDER_COUNT);
}
/*
By Petr Satka
Licnese CC-BY
*/

#ifndef Valve_h
#define Valve_h
#include <Arduino.h>
#include "TSafePreferences.h"
#include "driver/pcnt.h"
#include "soc/pcnt_struct.h"

class Valve {
public:
  Valve(SemaphoreHandle_t xSemaphore, const char* nvsParttionName);
  ~Valve();

  void configure(uint16_t pulsesPerServing, int flowMeterPinNumber, int valvePinNumber);
  void makeOrder();
  void refresh();
  uint16_t getOrderCount();

private:
  static constexpr const char* NAMESPACE_VALVE_STATE = "valve-state";
  static constexpr const char* KEY_ORDER_COUNT = "order-cnt";
  static const pcnt_unit_t PCNT_UNIT = PCNT_UNIT_0;

  SemaphoreHandle_t xSemaphore = NULL;
  TSafePreferences* valveStateStorage = NULL;
  int valvePinNumber = 0;
  bool volatile shouldOpen = false;
  bool volatile shouldClose = false;

  static void onServingReachedStatic(void* valve);

  void initPulseCounter(int16_t pulsesPerServing, int pinNumber);
  void openValve();
  void closeValve();
  void onServingReached();
};

#endif
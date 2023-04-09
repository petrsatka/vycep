/*
By Petr Satka
Licnese CC-BY
*/

#ifndef Valve_h
#define Valve_h
#include <Arduino.h>
#include "TSafePreferences.h"

class Valve {
public:
  Valve(SemaphoreHandle_t xSemaphore, const char* nvsParttionName);
  ~Valve();

  void configure(uint16_t pulsesPerServing);
  void makeOrder();
  void refresh();
  uint16_t getOrderCount();

    private : static constexpr const char* NAMESPACE_VALVE_STATE = "valve-state";
  static constexpr const char* KEY_ORDER_COUNT = "order-cnt";

  SemaphoreHandle_t xSemaphore = NULL;
  TSafePreferences* valveStateStorage = NULL;
  uint16_t pulsesPerServing = 0;
  bool shouldOpen = false;
  bool shouldClose = true;
};

#endif
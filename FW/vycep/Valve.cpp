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

void Valve::configure(uint16_t pulsesPerServing) {
  this->pulsesPerServing = pulsesPerServing;
}

void Valve::makeOrder() {
  uint16_t orderCount = 0;
  valveStateStorage->addUShort(KEY_ORDER_COUNT, 1, 0, orderCount);
  if (orderCount > 0) {
    shouldOpen = true;
  }
}

void Valve::refresh() {
}

uint16_t Valve::getOrderCount() {
  return valveStateStorage->getUShort(KEY_ORDER_COUNT);
}
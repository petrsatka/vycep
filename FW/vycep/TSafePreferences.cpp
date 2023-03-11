#include "TSafePreferences.h"
TSafePreferences::TSafePreferences(SemaphoreHandle_t xMutex, const char* nmspce, const char* partitionName) {
  this->xMutex = xMutex;
  preferences.begin(nmspce, false, partitionName);
}

bool TSafePreferences::clear() {
  dprintln("clear");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.clear();
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::remove(const char* key) {
  sprintln("!remove");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.remove(key);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putChar(const char* key, int8_t value) {
  sprintln("!putChar");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putChar(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putShort(const char* key, int16_t value) {
  sprintln("!putShort");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putShort(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putUShort(const char* key, uint16_t value) {
  sprintln("!putUShort");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putUShort(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putInt(const char* key, int32_t value) {
  sprintln("!putInt");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putInt(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putUInt(const char* key, uint32_t value) {
  dprintln("putUInt");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putUInt(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putLong(const char* key, int32_t value) {
  sprintln("!putLong");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putLong(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putULong(const char* key, uint32_t value) {
  sprintln("!putULong");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putULong(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putLong64(const char* key, int64_t value) {
  sprintln("!putLong64");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putLong64(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putULong64(const char* key, uint64_t value) {
  sprintln("!putULong64");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putULong64(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putFloat(const char* key, float_t value) {
  sprintln("!putFloat");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putFloat(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putDouble(const char* key, double_t value) {
  sprintln("!putDouble");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putDouble(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putBool(const char* key, bool value) {
  dprintln("putBool");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putBool(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putString(const char* key, const char* value) {
  sprintln("!putString");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putString(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putString(const char* key, String value) {
  sprintln("!putString");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putString(key, value);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::putBytes(const char* key, const void* value, size_t len) {
  dprintln("putBytes");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.putBytes(key, value, len);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::isKey(const char* key) {
  sprintln("!isKey");
  bool res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.isKey(key);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

PreferenceType TSafePreferences::getType(const char* key) {
  sprintln("!getType");
  PreferenceType res = PT_INVALID;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getType(key);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

int8_t TSafePreferences::getChar(const char* key, int8_t defaultValue) {
  sprintln("!getChar");
  int8_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getChar(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

uint8_t TSafePreferences::getUChar(const char* key, uint8_t defaultValue) {
  sprintln("!getUChar");
  uint8_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getUChar(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

int16_t TSafePreferences::getShort(const char* key, int16_t defaultValue) {
  sprintln("!getShort");
  int16_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getShort(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

uint16_t TSafePreferences::getUShort(const char* key, uint16_t defaultValue) {
  sprintln("!getUShort");
  uint16_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getUShort(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

int32_t TSafePreferences::getInt(const char* key, int32_t defaultValue) {
  sprintln("!getInt");
  int32_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getInt(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

uint32_t TSafePreferences::getUInt(const char* key, uint32_t defaultValue) {
  dprintln("getUInt");
  uint32_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getUInt(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

int32_t TSafePreferences::getLong(const char* key, int32_t defaultValue) {
  sprintln("!getLong");
  int32_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getLong(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

uint32_t TSafePreferences::getULong(const char* key, uint32_t defaultValue) {
  sprintln("!getULong");
  uint32_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getULong(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

int64_t TSafePreferences::getLong64(const char* key, int64_t defaultValue) {
  sprintln("!getLong64");
  int64_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getLong64(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

uint64_t TSafePreferences::getULong64(const char* key, uint64_t defaultValue) {
  sprintln("!getULong64");
  uint64_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getULong64(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

float_t TSafePreferences::getFloat(const char* key, float_t defaultValue) {
  sprintln("!getFloat");
  float_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getFloat(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

double_t TSafePreferences::getDouble(const char* key, double_t defaultValue) {
  sprintln("!getDouble");
  double_t res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getDouble(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::getBool(const char* key, bool defaultValue) {
  dprintln("getBool");
  bool res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getBool(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::getString(const char* key, char* value, size_t maxLen) {
  sprintln("!getString");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getString(key, value, maxLen);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

String TSafePreferences::getString(const char* key, String defaultValue) {
  sprintln("!getString");
  String res = defaultValue;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getString(key, defaultValue);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::getBytesLength(const char* key) {
  sprintln("!getBytesLength");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getBytesLength(key);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::getBytes(const char* key, void* buf, size_t maxLen) {
  dprintln("getBytes");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.getBytes(key, buf, maxLen);
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addChar(const char* key, int8_t add, int8_t defaultInitValue, int8_t& result) {
  sprintln("!addChar");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    int8_t nm = preferences.getChar(key, defaultInitValue);
    nm += add;
    res = preferences.putChar(key, nm) > 0;
    if (res) {
      result = nm;
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addUChar(const char* key, uint8_t add, uint8_t defaultInitValue, uint8_t& result) {
  sprintln("!addUChar");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    uint8_t nm = preferences.getUChar(key, defaultInitValue);
    nm += add;
    res = preferences.putUChar(key, nm) > 0;
    if (res) {
      result = nm;
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addShort(const char* key, int16_t add, int16_t defaultInitValue, int16_t& result) {
  sprintln("!addShort");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    int16_t nm = preferences.getShort(key, defaultInitValue);
    nm += add;
    res = preferences.putShort(key, nm) > 0;
    if (res) {
      result = nm;
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addUShort(const char* key, uint16_t add, uint16_t defaultInitValue, uint16_t& result) {
  sprintln("!addUShort");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    uint16_t nm = preferences.getUShort(key, defaultInitValue);
    nm += add;
    res = preferences.putUShort(key, nm) > 0;
    if (res) {
      result = nm;
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addInt(const char* key, int32_t add, int32_t defaultInitValue, int32_t& result) {
  sprintln("!addInt");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    int32_t nm = preferences.getInt(key, defaultInitValue);
    nm += add;
    res = preferences.putInt(key, nm) > 0;
    if (res) {
      result = nm;
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addUInt(const char* key, uint32_t add, uint32_t defaultInitValue, uint32_t& result) {
  sprintln("!addUInt");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    uint32_t nm = preferences.getUInt(key, defaultInitValue);
    nm += add;
    res = preferences.putUInt(key, nm) > 0;
    if (res) {
      result = nm;
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addLong(const char* key, int32_t add, int32_t defaultInitValue, int32_t& result) {
  sprintln("!addLong");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    int32_t nm = preferences.getLong(key, defaultInitValue);
    nm += add;
    res = preferences.putLong(key, nm) > 0;
    if (res) {
      result = nm;
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addULong(const char* key, uint32_t add, uint32_t defaultInitValue, uint32_t& result) {
  sprintln("!addULong");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    uint32_t nm = preferences.getULong(key, defaultInitValue);
    nm += add;
    res = preferences.putULong(key, nm) > 0;
    if (res) {
      result = nm;
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addLong64(const char* key, int64_t add, int64_t defaultInitValue, int64_t& result) {
  sprintln("!addLong64");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    int64_t nm = preferences.getLong64(key, defaultInitValue);
    nm += add;
    res = preferences.putLong64(key, nm) > 0;
    if (res) {
      result = nm;
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addULong64(const char* key, uint64_t add, uint64_t defaultInitValue, uint64_t& result) {
  sprintln("!addULong64");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    uint64_t nm = preferences.getULong64(key, defaultInitValue);
    nm += add;
    res = preferences.putULong64(key, nm) > 0;
    if (res) {
      result = nm;
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addFloat(const char* key, float_t add, float_t defaultInitValue, float_t& result) {
  sprintln("!addFloat");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    float_t nm = preferences.getFloat(key, defaultInitValue);
    if (!isnan(nm)) {
      nm += add;
      res = preferences.putFloat(key, nm) > 0;
      if (res) {
        result = nm;
      }
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

bool TSafePreferences::addDouble(const char* key, double_t add, double_t defaultInitValue, double_t& result) {
  sprintln("!addDouble");
  bool res = false;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    double_t nm = preferences.getDouble(key, defaultInitValue);
    if (!isnan(nm)) {
      nm += add;
      res = preferences.putDouble(key, nm) > 0;
      if (res) {
        result = nm;
      }
    }

    xSemaphoreGive(this->xMutex);
  }

  return res;
}

size_t TSafePreferences::freeEntries() {
  sprintln("!freeEntries");
  size_t res = 0;
  if (this->xMutex != NULL && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
    res = preferences.freeEntries();
    xSemaphoreGive(this->xMutex);
  }

  return res;
}

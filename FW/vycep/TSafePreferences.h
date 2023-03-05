//Thread safe ukládání do NVS

#ifndef TSafePreferences_h
#define TSafePreferences_h
#include <Arduino.h>
#include <Preferences.h>
#include "Debug.h"

class TSafePreferences {
public:
  TSafePreferences(SemaphoreHandle_t xMutex, const char* nmspce, const char* partitionName);
  bool clear(); //Vymaže celý namespace
  bool remove(const char* key); //Odstraní klíč
  size_t putChar(const char* key, int8_t value);
  size_t putUChar(const char* key, uint8_t value);
  size_t putShort(const char* key, int16_t value);
  size_t putUShort(const char* key, uint16_t value);
  size_t putInt(const char* key, int32_t value);
  size_t putUInt(const char* key, uint32_t value);
  size_t putLong(const char* key, int32_t value);
  size_t putULong(const char* key, uint32_t value);
  size_t putLong64(const char* key, int64_t value);
  size_t putULong64(const char* key, uint64_t value);
  size_t putFloat(const char* key, float_t value);
  size_t putDouble(const char* key, double_t value);
  size_t putBool(const char* key, bool value);
  size_t putString(const char* key, const char* value);
  size_t putString(const char* key, String value);
  size_t putBytes(const char* key, const void* value, size_t len);

  bool isKey(const char* key); //Je uložena hodnota pod tímto klíčem?
  PreferenceType getType(const char* key);
  int8_t getChar(const char* key, int8_t defaultValue = 0);
  uint8_t getUChar(const char* key, uint8_t defaultValue = 0);
  int16_t getShort(const char* key, int16_t defaultValue = 0);
  uint16_t getUShort(const char* key, uint16_t defaultValue = 0);
  int32_t getInt(const char* key, int32_t defaultValue = 0);
  uint32_t getUInt(const char* key, uint32_t defaultValue = 0);
  int32_t getLong(const char* key, int32_t defaultValue = 0);
  uint32_t getULong(const char* key, uint32_t defaultValue = 0);
  int64_t getLong64(const char* key, int64_t defaultValue = 0);
  uint64_t getULong64(const char* key, uint64_t defaultValue = 0);
  float_t getFloat(const char* key, float_t defaultValue = NAN);
  double_t getDouble(const char* key, double_t defaultValue = NAN);
  bool getBool(const char* key, bool defaultValue = false);
  size_t getString(const char* key, char* value, size_t maxLen);
  String getString(const char* key, String defaultValue = String());
  size_t getBytesLength(const char* key);
  size_t getBytes(const char* key, void* buf, size_t maxLen);

  //Metody pro thread safe navýšení uložené hodnoty
  bool addChar(const char* key, int8_t add, int8_t defaultInitValue, int8_t& result);
  bool addUChar(const char* key, uint8_t add, uint8_t defaultInitValue, uint8_t& result);
  bool addShort(const char* key, int16_t add, int16_t defaultInitValue, int16_t& result);
  bool addUShort(const char* key, uint16_t add, uint16_t defaultInitValue, uint16_t& result);
  bool addInt(const char* key, int32_t add, int32_t defaultInitValue, int32_t& result);
  bool addUInt(const char* key, uint32_t add, uint32_t defaultInitValue, uint32_t& result);
  bool addLong(const char* key, int32_t add, int32_t defaultInitValue, int32_t& result);
  bool addULong(const char* key, uint32_t add, uint32_t defaultInitValue, uint32_t& result);
  bool addLong64(const char* key, int64_t add, int64_t defaultInitValue, int64_t& result);
  bool addULong64(const char* key, uint64_t add, uint64_t defaultInitValue, uint64_t& result);
  bool addFloat(const char* key, float_t add, float_t defaultInitValue, float_t& result);
  bool addDouble(const char* key, double_t add, double_t defaultInitValue, double_t& result);

  size_t freeEntries();

private:
  SemaphoreHandle_t xMutex = NULL;
  Preferences preferences;
};
#endif
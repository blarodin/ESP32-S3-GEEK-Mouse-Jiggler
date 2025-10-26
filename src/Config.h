#ifndef CONFIG_H
#define CONFIG_H

#include <Preferences.h>

struct JigglerConfig {
  unsigned long jiggleInterval;  // Milliseconds between jiggles
  int moveDistance;              // Pixel movement distance
  bool randomMoves;              // Use random movements instead of square pattern
  int randomMinDistance;         // Minimum random movement distance
  int randomMaxDistance;         // Maximum random movement distance
  char deviceName[32];           // BLE device name
  char wifiSSID[32];             // WiFi AP SSID
  char wifiPassword[64];         // WiFi AP password
};

class ConfigManager {
private:
  Preferences preferences;
  JigglerConfig config;
  
public:
  ConfigManager() {
    // Default values
    config.jiggleInterval = 30000;
    config.moveDistance = 2;
    config.randomMoves = false;
    config.randomMinDistance = 1;
    config.randomMaxDistance = 5;
    strcpy(config.deviceName, "Mouse Jiggler");
    strcpy(config.wifiSSID, "MouseJiggler-Config");
    strcpy(config.wifiPassword, "jiggler123");
  }
  
  void begin() {
    preferences.begin("jiggler", false);
    loadConfig();
  }
  
  void loadConfig() {
    config.jiggleInterval = preferences.getULong("interval", 30000);
    config.moveDistance = preferences.getInt("distance", 2);
    config.randomMoves = preferences.getBool("random", false);
    config.randomMinDistance = preferences.getInt("randMin", 1);
    config.randomMaxDistance = preferences.getInt("randMax", 5);
    preferences.getString("deviceName", config.deviceName, sizeof(config.deviceName));
    preferences.getString("wifiSSID", config.wifiSSID, sizeof(config.wifiSSID));
    preferences.getString("wifiPass", config.wifiPassword, sizeof(config.wifiPassword));
    
    // Set defaults if empty
    if (strlen(config.deviceName) == 0) strcpy(config.deviceName, "Mouse Jiggler");
    if (strlen(config.wifiSSID) == 0) strcpy(config.wifiSSID, "MouseJiggler-Config");
    if (strlen(config.wifiPassword) == 0) strcpy(config.wifiPassword, "jiggler123");
  }
  
  void saveConfig() {
    preferences.putULong("interval", config.jiggleInterval);
    preferences.putInt("distance", config.moveDistance);
    preferences.putBool("random", config.randomMoves);
    preferences.putInt("randMin", config.randomMinDistance);
    preferences.putInt("randMax", config.randomMaxDistance);
    preferences.putString("deviceName", config.deviceName);
    preferences.putString("wifiSSID", config.wifiSSID);
    preferences.putString("wifiPass", config.wifiPassword);
  }
  
  JigglerConfig& getConfig() {
    return config;
  }
  
  void setConfig(const JigglerConfig& newConfig) {
    config = newConfig;
    saveConfig();
  }
  
  void resetToDefaults() {
    preferences.clear();
    config.jiggleInterval = 30000;
    config.moveDistance = 2;
    config.randomMoves = false;
    config.randomMinDistance = 1;
    config.randomMaxDistance = 5;
    strcpy(config.deviceName, "Mouse Jiggler");
    strcpy(config.wifiSSID, "MouseJiggler-Config");
    strcpy(config.wifiPassword, "jiggler123");
    saveConfig();
  }
};

#endif

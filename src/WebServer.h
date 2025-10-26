#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <WiFi.h>
#include "Config.h"

class JigglerWebServer {
private:
  WiFiServer* server;
  ConfigManager* configManager;
  bool apActive;
  
  const char* getIndexHTML() {
    return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Mouse Jiggler Config</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Arial, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      padding: 20px;
      display: flex;
      justify-content: center;
      align-items: center;
    }
    .container {
      background: white;
      border-radius: 16px;
      box-shadow: 0 20px 60px rgba(0,0,0,0.3);
      padding: 40px;
      max-width: 600px;
      width: 100%;
    }
    h1 {
      color: #333;
      margin-bottom: 10px;
      font-size: 28px;
    }
    .subtitle {
      color: #666;
      margin-bottom: 30px;
      font-size: 14px;
    }
    .section {
      margin-bottom: 25px;
      padding: 20px;
      background: #f8f9fa;
      border-radius: 8px;
    }
    .section-title {
      font-size: 16px;
      font-weight: 600;
      color: #667eea;
      margin-bottom: 15px;
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }
    label {
      display: block;
      margin-bottom: 5px;
      color: #555;
      font-weight: 500;
      font-size: 14px;
    }
    input[type="text"], input[type="number"], input[type="password"] {
      width: 100%;
      padding: 12px;
      margin-bottom: 15px;
      border: 2px solid #e0e0e0;
      border-radius: 8px;
      font-size: 14px;
      transition: border-color 0.3s;
    }
    input:focus {
      outline: none;
      border-color: #667eea;
    }
    .checkbox-group {
      display: flex;
      align-items: center;
      margin-bottom: 15px;
    }
    input[type="checkbox"] {
      width: 20px;
      height: 20px;
      margin-right: 10px;
      cursor: pointer;
    }
    .btn-group {
      display: flex;
      gap: 10px;
      margin-top: 30px;
    }
    button {
      flex: 1;
      padding: 14px;
      border: none;
      border-radius: 8px;
      font-size: 16px;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.3s;
    }
    .btn-primary {
      background: #667eea;
      color: white;
    }
    .btn-primary:hover {
      background: #5568d3;
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
    }
    .btn-secondary {
      background: #6c757d;
      color: white;
    }
    .btn-secondary:hover {
      background: #5a6268;
    }
    .info-box {
      background: #e3f2fd;
      border-left: 4px solid #2196F3;
      padding: 15px;
      margin-bottom: 20px;
      border-radius: 4px;
      font-size: 13px;
      color: #555;
    }
    .success-message {
      background: #d4edda;
      color: #155724;
      padding: 15px;
      border-radius: 8px;
      margin-bottom: 20px;
      display: none;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🖱️ Mouse Jiggler Config</h1>
    <p class="subtitle">ESP32-S3-GEEK Configuration Panel</p>
    
    <div class="info-box">
      ℹ️ Changes will be saved to device memory and persist after reboot.
    </div>
    
    <div id="successMessage" class="success-message">
      ✓ Settings saved successfully!
    </div>
    
    <form id="configForm">
      <div class="section">
        <div class="section-title">Jiggle Settings</div>
        
        <label for="interval">Jiggle Interval (seconds)</label>
        <input type="number" id="interval" name="interval" min="5" max="300" value="30" required>
        
        <label for="distance">Move Distance (pixels)</label>
        <input type="number" id="distance" name="distance" min="1" max="20" value="2" required>
        
        <div class="checkbox-group">
          <input type="checkbox" id="random" name="random">
          <label for="random" style="margin-bottom: 0;">Use Random Movements</label>
        </div>
        
        <div id="randomSettings" style="display: none;">
          <label for="randMin">Random Min Distance</label>
          <input type="number" id="randMin" name="randMin" min="1" max="10" value="1">
          
          <label for="randMax">Random Max Distance</label>
          <input type="number" id="randMax" name="randMax" min="1" max="20" value="5">
        </div>
      </div>
      
      <div class="section">
        <div class="section-title">Device Settings</div>
        
        <label for="deviceName">BLE Device Name</label>
        <input type="text" id="deviceName" name="deviceName" maxlength="31" value="Mouse Jiggler" required>
      </div>
      
      <div class="section">
        <div class="section-title">WiFi AP Settings</div>
        
        <label for="wifiSSID">WiFi SSID</label>
        <input type="text" id="wifiSSID" name="wifiSSID" maxlength="31" value="MouseJiggler-Config" required>
        
        <label for="wifiPassword">WiFi Password (min 8 chars)</label>
        <input type="password" id="wifiPassword" name="wifiPassword" minlength="8" maxlength="63" value="jiggler123" required>
      </div>
      
      <div class="btn-group">
        <button type="submit" class="btn-primary">Save Settings</button>
        <button type="button" class="btn-secondary" onclick="resetDefaults()">Reset to Defaults</button>
      </div>
    </form>
  </div>
  
  <script>
    // Load current settings
    fetch('/api/config')
      .then(r => r.json())
      .then(data => {
        document.getElementById('interval').value = data.interval / 1000;
        document.getElementById('distance').value = data.distance;
        document.getElementById('random').checked = data.random;
        document.getElementById('randMin').value = data.randMin;
        document.getElementById('randMax').value = data.randMax;
        document.getElementById('deviceName').value = data.deviceName;
        document.getElementById('wifiSSID').value = data.wifiSSID;
        document.getElementById('wifiPassword').value = data.wifiPassword;
        toggleRandomSettings();
      });
    
    // Toggle random settings visibility
    document.getElementById('random').addEventListener('change', toggleRandomSettings);
    
    function toggleRandomSettings() {
      const randomSettings = document.getElementById('randomSettings');
      randomSettings.style.display = document.getElementById('random').checked ? 'block' : 'none';
    }
    
    // Handle form submission
    document.getElementById('configForm').addEventListener('submit', function(e) {
      e.preventDefault();
      
      const formData = {
        interval: parseInt(document.getElementById('interval').value) * 1000,
        distance: parseInt(document.getElementById('distance').value),
        random: document.getElementById('random').checked,
        randMin: parseInt(document.getElementById('randMin').value),
        randMax: parseInt(document.getElementById('randMax').value),
        deviceName: document.getElementById('deviceName').value,
        wifiSSID: document.getElementById('wifiSSID').value,
        wifiPassword: document.getElementById('wifiPassword').value
      };
      
      fetch('/api/config', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify(formData)
      })
      .then(r => r.json())
      .then(data => {
        const msg = document.getElementById('successMessage');
        msg.style.display = 'block';
        setTimeout(() => msg.style.display = 'none', 3000);
      });
    });
    
    function resetDefaults() {
      if (confirm('Reset all settings to defaults?')) {
        fetch('/api/reset', {method: 'POST'})
          .then(() => location.reload());
      }
    }
  </script>
</body>
</html>
)rawliteral";
  }
  
public:
  JigglerWebServer() : server(nullptr), configManager(nullptr), apActive(false) {}
  ~JigglerWebServer() {
    if (server) {
      delete server;
    }
  }
  
  void begin(ConfigManager* cm) {
    configManager = cm;
    
    // Start WiFi AP
    JigglerConfig& cfg = configManager->getConfig();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(cfg.wifiSSID, cfg.wifiPassword);
    
    Serial.println("WiFi AP Started");
    Serial.print("SSID: ");
    Serial.println(cfg.wifiSSID);
    Serial.print("Password: ");
    Serial.println(cfg.wifiPassword);
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
    
    apActive = true;
    
    // Create and start server
    server = new WiFiServer(80);
    server->begin();
    Serial.println("Web server started on http://" + WiFi.softAPIP().toString());
  }
  
  void handleClient() {
    if (!apActive || !server) return;
    
    WiFiClient client = server->available();
    if (!client) return;
    
    Serial.println("New client connected");
    String requestPath = "";
    String currentLine = "";
    String requestBody = "";
    bool isPost = false;
    int contentLength = 0;
    bool headersEnded = false;
    bool firstLine = true;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (!headersEnded) {
          if (c == '\n') {
            if (currentLine.length() == 0) {
              headersEnded = true;
              
              // Read POST body if present
              if (isPost && contentLength > 0) {
                delay(10);  // Give time for body to arrive
                while (requestBody.length() < contentLength && client.available()) {
                  requestBody += (char)client.read();
                }
              }
              
              // Handle request
              Serial.print("Request path: ");
              Serial.println(requestPath);
              handleRequest(client, requestPath, requestBody, isPost);
              break;
            } else {
              // First line contains the request path
              if (firstLine) {
                requestPath = currentLine;
                firstLine = false;
                if (currentLine.startsWith("POST")) {
                  isPost = true;
                }
              }
              // Check other headers
              if (currentLine.startsWith("Content-Length: ")) {
                contentLength = currentLine.substring(16).toInt();
              }
              currentLine = "";
            }
          } else if (c != '\r') {
            currentLine += c;
          }
        }
      }
    }
    
    delay(10);
    client.stop();
    Serial.println("Client disconnected");
  }
  
private:
  void handleRequest(WiFiClient& client, String& requestLine, String& body, bool isPost) {
    // Send headers
    client.println("HTTP/1.1 200 OK");
    
    // Route handling
    if (requestLine.indexOf("GET / ") >= 0) {
      client.println("Content-type:text/html");
      client.println();
      client.print(getIndexHTML());
    }
    else if (requestLine.indexOf("GET /api/config") >= 0) {
      client.println("Content-type:application/json");
      client.println();
      
      JigglerConfig& cfg = configManager->getConfig();
      String json = "{";
      json += "\"interval\":" + String(cfg.jiggleInterval) + ",";
      json += "\"distance\":" + String(cfg.moveDistance) + ",";
      json += "\"random\":" + String(cfg.randomMoves ? "true" : "false") + ",";
      json += "\"randMin\":" + String(cfg.randomMinDistance) + ",";
      json += "\"randMax\":" + String(cfg.randomMaxDistance) + ",";
      json += "\"deviceName\":\"" + String(cfg.deviceName) + "\",";
      json += "\"wifiSSID\":\"" + String(cfg.wifiSSID) + "\",";
      json += "\"wifiPassword\":\"" + String(cfg.wifiPassword) + "\"";
      json += "}";
      client.print(json);
    }
    else if (requestLine.indexOf("POST /api/config") >= 0 && isPost) {
      client.println("Content-type:application/json");
      client.println();
      
      // Parse JSON manually
      JigglerConfig newConfig = configManager->getConfig();
      
      int pos;
      if ((pos = body.indexOf("\"interval\":")) >= 0) {
        newConfig.jiggleInterval = body.substring(pos + 11).toInt();
      }
      if ((pos = body.indexOf("\"distance\":")) >= 0) {
        newConfig.moveDistance = body.substring(pos + 11).toInt();
      }
      if ((pos = body.indexOf("\"random\":true")) >= 0) {
        newConfig.randomMoves = true;
      } else if ((pos = body.indexOf("\"random\":false")) >= 0) {
        newConfig.randomMoves = false;
      }
      if ((pos = body.indexOf("\"randMin\":")) >= 0) {
        newConfig.randomMinDistance = body.substring(pos + 10).toInt();
      }
      if ((pos = body.indexOf("\"randMax\":")) >= 0) {
        newConfig.randomMaxDistance = body.substring(pos + 10).toInt();
      }
      if ((pos = body.indexOf("\"deviceName\":\"")) >= 0) {
        int endPos = body.indexOf("\"", pos + 14);
        body.substring(pos + 14, endPos).toCharArray(newConfig.deviceName, sizeof(newConfig.deviceName));
      }
      if ((pos = body.indexOf("\"wifiSSID\":\"")) >= 0) {
        int endPos = body.indexOf("\"", pos + 12);
        body.substring(pos + 12, endPos).toCharArray(newConfig.wifiSSID, sizeof(newConfig.wifiSSID));
      }
      if ((pos = body.indexOf("\"wifiPassword\":\"")) >= 0) {
        int endPos = body.indexOf("\"", pos + 16);
        body.substring(pos + 16, endPos).toCharArray(newConfig.wifiPassword, sizeof(newConfig.wifiPassword));
      }
      
      configManager->setConfig(newConfig);
      
      client.print("{\"success\":true}");
      Serial.println("Configuration updated via web interface");
    }
    else if (requestLine.indexOf("POST /api/reset") >= 0 && isPost) {
      client.println("Content-type:application/json");
      client.println();
      
      configManager->resetToDefaults();
      client.print("{\"success\":true}");
      Serial.println("Configuration reset to defaults");
    }
    else {
      client.println("Content-type:text/plain");
      client.println();
      client.print("404 Not Found");
    }
    
    client.println();
  }

public:
  
  bool isActive() {
    return apActive;
  }
  
  String getIPAddress() {
    return WiFi.softAPIP().toString();
  }
};

#endif

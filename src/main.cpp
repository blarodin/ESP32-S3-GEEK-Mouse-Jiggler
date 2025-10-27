/**
 * BLE Mouse Jiggler for ESP32-S3-GEEK
 * 
 * This program turns the ESP32-S3-GEEK into a Bluetooth LE mouse jiggler
 * that prevents the computer from going to sleep by moving the mouse cursor
 * in small, subtle movements at regular intervals.
 * 
 * The device includes LCD display support to show the jiggler status.
 * WiFi AP mode allows configuration via web interface.
 */

#include <Arduino.h>
#include <SPI.h>
#include <BleMouse.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "Config.h"
#include "WebServer.h"

// Configuration manager
ConfigManager configManager;
JigglerWebServer webServer;

// BLE Mouse instance - will be reinitialized with config
BleMouse* bleMouse = nullptr;

// Runtime variables
unsigned long lastJiggleTime = 0;
bool isJiggling = false;

// LCD available flag
bool lcdAvailable = true;

// WiFi status
bool wifiDisplayed = false;

// Status states
enum DisplayState {
  STATE_INITIALIZING,
  STATE_WAITING,
  STATE_CONNECTED,
  STATE_JIGGLING,
  STATE_WIFI_INFO
};

DisplayState currentState = STATE_INITIALIZING;
DisplayState lastDrawnState = STATE_INITIALIZING;
unsigned long jiggleCount = 0;
unsigned long lastDrawnJiggleCount = 0;
unsigned long nextJiggleIn = 0;
unsigned long lastDrawnNextJiggleIn = 0;
int lastDrawnProgress = -1;  // Track last drawn progress percentage

// Function declarations
void performJiggle();
void performRandomJiggle();
void updateDisplay(bool forceFullRedraw = false);
void drawHeader();
void drawConnectionStatus(bool connected);
void drawJiggleInfo();
void drawProgressBar(int percentage);
void drawStatusIcon(bool connected);
void drawWiFiIcon();
void updateCountdownOnly();
void showWiFiInfo();

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting BLE Mouse Jiggler...");
  
  // Load configuration
  configManager.begin();
  JigglerConfig& config = configManager.getConfig();
  Serial.println("Configuration loaded");
  
  // Initialize LCD
  Config_Init();
  Serial.println("Config initialized");
  
  LCD_Init();
  Serial.println("LCD initialized");
  
  LCD_SetBacklight(100);
  Serial.println("Backlight set");
  
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, WHITE);
  Serial.println("Paint image created");
  
  Paint_SetRotate(90);
  Serial.println("Rotation set");
  
  // Beautiful startup screen
  LCD_Clear(0x001F);  // Deep blue background
  Serial.println("LCD cleared");
  
  // Draw startup screen - left aligned
  Serial.println("Drawing startup screen...");
  Paint_DrawString_EN(5, 15, "MOUSE JIGGLER", &Font16, 0x001F, 0xFFFF);
  Paint_DrawString_EN(5, 40, "ESP32-S3-GEEK", &Font16, 0x001F, 0x07FF);
  
  // Author info
  Paint_DrawString_EN(5, 70, "YEVHENII RODIN", &Font16, 0x001F, 0xFFE0);
  Paint_DrawString_EN(5, 95, "BLARODIN@GMAIL.COM", &Font16, 0x001F, 0xFFE0);
  
  delay(2000);
  
  // Start WiFi AP and web server
  Serial.println("Starting WiFi AP...");
  webServer.begin(&configManager);
  
  // Show WiFi info on display
  showWiFiInfo();
  delay(3000);
  
  // Initialize BLE Mouse with configured name
  Serial.println("Starting BLE...");
  bleMouse = new BleMouse(config.deviceName, "ESP32-S3-GEEK", 100);
  bleMouse->begin();
  Serial.println("BLE Mouse Jiggler started!");
  Serial.println("Waiting for connection...");
  
  currentState = STATE_WAITING;
  updateDisplay(true);
  Serial.println("Setup complete!");
}

void loop() {
  static unsigned long lastDisplayUpdate = 0;
  unsigned long currentTime = millis();
  
  // Handle web server requests
  webServer.handleClient();
  
  // Get current configuration
  JigglerConfig& config = configManager.getConfig();
  
  // Check if BLE mouse is connected
  if (bleMouse && bleMouse->isConnected()) {
    if (!isJiggling) {
      isJiggling = true;
      lastJiggleTime = currentTime;  // Reset timer on connection
      Serial.println("Mouse connected! Jiggler active.");
      currentState = STATE_CONNECTED;
      updateDisplay(true);  // Full redraw on state change
    }
    
    // Check if it's time to jiggle
    if (currentTime - lastJiggleTime >= config.jiggleInterval) {
      if (config.randomMoves) {
        performRandomJiggle();
      } else {
        performJiggle();
      }
      lastJiggleTime = currentTime;
      jiggleCount++;
    }
    
    // Update display every second when connected (only countdown and progress)
    if (currentTime - lastDisplayUpdate >= 1000) {
      nextJiggleIn = (config.jiggleInterval - (currentTime - lastJiggleTime)) / 1000;
      if (currentState != STATE_JIGGLING) {
        updateCountdownOnly();  // Only update dynamic parts
      }
      lastDisplayUpdate = currentTime;
    }
  } else {
    if (isJiggling) {
      isJiggling = false;
      Serial.println("Mouse disconnected. Waiting for connection...");
      currentState = STATE_WAITING;
      updateDisplay(true);  // Full redraw on state change
    }
  }
  
  // Small delay to prevent overwhelming the CPU
  delay(100);
}

void performJiggle() {
  JigglerConfig& config = configManager.getConfig();
  
  // Move mouse in a small square pattern to create subtle movement
  DisplayState prevState = currentState;
  currentState = STATE_JIGGLING;
  
  Serial.println("Jiggling mouse (square pattern)...");
  updateDisplay(true);  // Full redraw for jiggling state
  
  // Move right
  bleMouse->move(config.moveDistance, 0);
  delay(50);
  
  // Move down
  bleMouse->move(0, config.moveDistance);
  delay(50);
  
  // Move left (return to approximately original position)
  bleMouse->move(-config.moveDistance, 0);
  delay(50);
  
  // Move up (return to approximately original position)
  bleMouse->move(0, -config.moveDistance);
  delay(50);
  
  Serial.println("Jiggle complete!");
  currentState = STATE_CONNECTED;
  updateDisplay(true);  // Full redraw back to connected state
}

void performRandomJiggle() {
  JigglerConfig& config = configManager.getConfig();
  
  currentState = STATE_JIGGLING;
  Serial.println("Jiggling mouse (random pattern)...");
  updateDisplay(true);
  
  // Generate random movements
  int dx1 = random(config.randomMinDistance, config.randomMaxDistance + 1);
  int dy1 = random(config.randomMinDistance, config.randomMaxDistance + 1);
  
  // Random direction
  if (random(0, 2)) dx1 = -dx1;
  if (random(0, 2)) dy1 = -dy1;
  
  // First move
  bleMouse->move(dx1, dy1);
  delay(100);
  
  // Return to origin (approximately)
  bleMouse->move(-dx1, -dy1);
  delay(50);
  
  Serial.println("Random jiggle complete!");
  currentState = STATE_CONNECTED;
  updateDisplay(true);
}

// Beautiful display update with status, progress, and info
void updateDisplay(bool forceFullRedraw) {
  // Only do full redraw if state changed or forced
  if (forceFullRedraw || currentState != lastDrawnState) {
    // Clear with gradient-like effect using different shades
    LCD_Clear(0x0010);  // Dark blue-black background
    
    // Draw decorative header bar - fill completely (240 pixels wide when rotated 90)
    for (int y = 0; y < 25; y++) {
      for (int x = 0; x < LCD_HEIGHT; x++) {
        Paint_SetPixel(x, y, 0x001F);
      }
    }
    
    // Title in header
    Paint_DrawString_EN(20, 5, "MOUSE JIGGLER", &Font16, 0x001F, 0xFFFF);
    
    // Draw WiFi icon
    drawWiFiIcon();
    
    // Draw status icon/indicator
    drawStatusIcon(bleMouse ? bleMouse->isConnected() : false);
    
    // Main content area
    if (currentState == STATE_WAITING) {
      Paint_DrawString_EN(15, 35, "Status:", &Font16, 0x0010, 0x07FF);
      Paint_DrawString_EN(80, 35, " WAITING", &Font16, 0x0010, 0xFFE0);  // Yellow
      
      Paint_DrawString_EN(15, 60, "Waiting for", &Font16, 0x0010, 0xFFFF);
      Paint_DrawString_EN(15, 80, "BLE connection", &Font16, 0x0010, 0xFFFF);
    }
    else if (currentState == STATE_CONNECTED) {
      // Connection status
      Paint_DrawString_EN(15, 35, "Status:", &Font16, 0x0010, 0x07FF);
      Paint_DrawString_EN(80, 35, " ACTIVE", &Font16, 0x0010, 0x07E0);  // Green with space
      
      // Jiggle count
      char countStr[32];
      sprintf(countStr, "Jiggles: %lu", jiggleCount);
      Paint_DrawString_EN(15, 60, countStr, &Font16, 0x0010, 0xFFFF);
      
      // Next jiggle countdown
      char timeStr[32];
      sprintf(timeStr, "Next in: %lus", nextJiggleIn);
      Paint_DrawString_EN(15, 85, timeStr, &Font16, 0x0010, 0xFFE0);
      
      // Progress bar
      JigglerConfig& config = configManager.getConfig();
      int progress = 100 - ((nextJiggleIn * 100) / (config.jiggleInterval / 1000));
      drawProgressBar(progress);
    }
    else if (currentState == STATE_JIGGLING) {
      Paint_DrawString_EN(15, 40, "Status:", &Font16, 0x0010, 0x07FF);
      Paint_DrawString_EN(15, 65, "JIGGLING!", &Font20, 0x0010, 0xF800);  // Red
      Paint_DrawString_EN(25, 95, "Moving...", &Font16, 0x0010, 0xFFE0);
    }
    
    lastDrawnState = currentState;
    lastDrawnJiggleCount = jiggleCount;
    lastDrawnNextJiggleIn = nextJiggleIn;
    lastDrawnProgress = -1;  // Reset progress tracker on full redraw
  }
}

// Update only the countdown timer and progress bar (no screen clear)
void updateCountdownOnly() {
  if (currentState != STATE_CONNECTED) return;
  
  // Only update if values changed
  if (nextJiggleIn != lastDrawnNextJiggleIn || jiggleCount != lastDrawnJiggleCount) {
    // Clear and update only the numeric values, not the labels
    
    if (jiggleCount != lastDrawnJiggleCount) {
      // Clear only the number area ("Jiggles: " takes ~95 pixels)
      Paint_DrawRectangle(110, 60, LCD_HEIGHT - 15, 75, 0x0010, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      
      // Draw only the number
      char countStr[16];
      sprintf(countStr, "%lu", jiggleCount);
      Paint_DrawString_EN(110, 60, countStr, &Font16, 0x0010, 0xFFFF);
    }
    
    if (nextJiggleIn != lastDrawnNextJiggleIn) {
      // Clear the number area wider to handle 2-digit to 1-digit transitions
      Paint_DrawRectangle(110, 85, LCD_HEIGHT - 15, 100, 0x0010, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      
      // Draw only the number with 's' - aligned with jiggle counter
      char timeStr[16];
      sprintf(timeStr, "%lus", nextJiggleIn);
      Paint_DrawString_EN(110, 85, timeStr, &Font16, 0x0010, 0xFFE0);
      
      // Update progress bar
      JigglerConfig& config = configManager.getConfig();
      int progress = 100 - ((nextJiggleIn * 100) / (config.jiggleInterval / 1000));
      drawProgressBar(progress);
    }
    
    lastDrawnJiggleCount = jiggleCount;
    lastDrawnNextJiggleIn = nextJiggleIn;
  }
}

void drawStatusIcon(bool connected) {
  // Draw a status indicator circle in top right (use LCD_HEIGHT for rotated width)
  int cx = LCD_HEIGHT - 15;
  int cy = 12;
  int radius = 6;
  
  uint16_t color = connected ? 0x07E0 : 0xF800;  // Green if connected, red if not
  
  // Draw filled circle
  for (int y = -radius; y <= radius; y++) {
    for (int x = -radius; x <= radius; x++) {
      if (x*x + y*y <= radius*radius) {
        Paint_DrawPoint(cx + x, cy + y, color, DOT_PIXEL_1X1, DOT_FILL_AROUND);
      }
    }
  }
}

void drawWiFiIcon() {
  // Draw WiFi indicator in top left corner of header
  int x = 5;
  int y = 8;
  uint16_t color = 0xFFFF;  // White
  
  // Simple WiFi icon (3 arcs)
  Paint_DrawLine(x, y + 8, x + 2, y + 8, color, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  Paint_DrawLine(x - 2, y + 5, x + 4, y + 5, color, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  Paint_DrawLine(x - 4, y + 2, x + 6, y + 2, color, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
}

void showWiFiInfo() {
  JigglerConfig& config = configManager.getConfig();
  String ip = webServer.getIPAddress();
  
  // Show 2 screens alternating
  for (int screen = 0; screen < 2; screen++) {
    LCD_Clear(0x0010);
    
    // Draw header bar
    for (int y = 0; y < 25; y++) {
      for (int x = 0; x < LCD_HEIGHT; x++) {
        Paint_SetPixel(x, y, 0x001F);
      }
    }
    
    if (screen == 0) {
      // Screen 1: WiFi credentials
      Paint_DrawString_EN(20, 5, "WiFi Network", &Font16, 0x001F, 0xFFFF);
      
      Paint_DrawString_EN(15, 35, "SSID:", &Font16, 0x0010, 0x07FF);
      Paint_DrawString_EN(15, 55, config.wifiSSID, &Font16, 0x0010, 0xFFFF);
      
      Paint_DrawString_EN(15, 80, "Password:", &Font16, 0x0010, 0x07FF);
      Paint_DrawString_EN(15, 100, config.wifiPassword, &Font16, 0x0010, 0xFFFF);
    } else {
      // Screen 2: IP address
      Paint_DrawString_EN(25, 5, "Open Browser", &Font16, 0x001F, 0xFFFF);
      
      Paint_DrawString_EN(15, 40, "Connect to:", &Font16, 0x0010, 0x07FF);
      Paint_DrawString_EN(15, 65, ip.c_str(), &Font16, 0x0010, 0x07E0);
      
      Paint_DrawString_EN(15, 95, "http://", &Font16, 0x0010, 0xFFE0);
      Paint_DrawString_EN(80, 95, ip.c_str(), &Font16, 0x0010, 0xFFE0);
    }
    
    delay(1500);  // Show each screen for 1.5 seconds
  }
}

void drawProgressBar(int percentage) {
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;
  
  // Skip redraw if percentage hasn't changed
  if (percentage == lastDrawnProgress) return;
  
  int barX = 15;
  int barY = 110;
  int barWidth = LCD_HEIGHT - 30;  // Use LCD_HEIGHT for rotated width
  int barHeight = 12;
  
  int oldFillWidth = (barWidth * lastDrawnProgress) / 100;
  int newFillWidth = (barWidth * percentage) / 100;
  
  // Only redraw on first call (full redraw) or when bar is shrinking
  if (lastDrawnProgress < 0 || newFillWidth < oldFillWidth) {
    // Clear only the portion that needs clearing (from new position to end)
    if (newFillWidth < barWidth) {
      Paint_DrawRectangle(barX + newFillWidth, barY, barX + barWidth, barY + barHeight, 0x0010, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }
  }
  
  // Draw border (only on first draw)
  if (lastDrawnProgress < 0) {
    Paint_DrawRectangle(barX - 1, barY - 1, barX + barWidth + 1, barY + barHeight + 1, 0x07FF, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  }
  
  // Draw filled portion
  if (newFillWidth > 0) {
    // Gradient effect: change color based on progress
    uint16_t barColor;
    if (percentage < 33) {
      barColor = 0x07E0;  // Green
    } else if (percentage < 66) {
      barColor = 0xFFE0;  // Yellow
    } else {
      barColor = 0xF800;  // Red
    }
    
    for (int y = 0; y < barHeight; y++) {
      Paint_DrawLine(barX, barY + y, barX + newFillWidth, barY + y, barColor, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    }
  }
  
  lastDrawnProgress = percentage;
}

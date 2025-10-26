/**
 * BLE Mouse Jiggler for ESP32-S3-GEEK
 * 
 * This program turns the ESP32-S3-GEEK into a Bluetooth LE mouse jiggler
 * that prevents the computer from going to sleep by moving the mouse cursor
 * in small, subtle movements at regular intervals.
 * 
 * The device includes LCD display support to show the jiggler status.
 */

#include <Arduino.h>
#include <SPI.h>
#include <BleMouse.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"

// BLE Mouse instance
BleMouse bleMouse("Mouse Jiggler", "ESP32-S3-GEEK", 100);

// Jiggler configuration
const unsigned long JIGGLE_INTERVAL = 60000;  // Move every 60 seconds (1 minute)
const int MOVE_DISTANCE = 2;                   // Small movement distance in pixels
unsigned long lastJiggleTime = 0;
bool isJiggling = false;

// LCD available flag
bool lcdAvailable = true;

// Status states
enum DisplayState {
  STATE_INITIALIZING,
  STATE_WAITING,
  STATE_CONNECTED,
  STATE_JIGGLING
};

DisplayState currentState = STATE_INITIALIZING;
DisplayState lastDrawnState = STATE_INITIALIZING;
unsigned long jiggleCount = 0;
unsigned long lastDrawnJiggleCount = 0;
unsigned long nextJiggleIn = 0;
unsigned long lastDrawnNextJiggleIn = 0;

// Function declarations
void performJiggle();
void updateDisplay(bool forceFullRedraw = false);
void drawHeader();
void drawConnectionStatus(bool connected);
void drawJiggleInfo();
void drawProgressBar(int percentage);
void drawStatusIcon(bool connected);
void updateCountdownOnly();

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting BLE Mouse Jiggler...");
  
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
  
  // Draw startup screen with centered title
  Serial.println("Drawing startup screen...");
  Paint_DrawString_EN(15, 20, "MOUSE JIGGLER", &Font16, 0x001F, 0xFFFF);
  Paint_DrawString_EN(30, 50, "ESP32-S3", &Font16, 0x001F, 0x07FF);
  Paint_DrawString_EN(20, 80, "Initializing", &Font16, 0x001F, 0xFFE0);
  Paint_DrawString_EN(40, 110, "BLE...", &Font16, 0x001F, 0xFFE0);
  
  delay(1500);
  
  // Start BLE Mouse
  Serial.println("Starting BLE...");
  bleMouse.begin();
  Serial.println("BLE Mouse Jiggler started!");
  Serial.println("Waiting for connection...");
  
  currentState = STATE_WAITING;
  updateDisplay(true);
  Serial.println("Setup complete!");
}

void loop() {
  static unsigned long lastDisplayUpdate = 0;
  unsigned long currentTime = millis();
  
  // Check if BLE mouse is connected
  if (bleMouse.isConnected()) {
    if (!isJiggling) {
      isJiggling = true;
      Serial.println("Mouse connected! Jiggler active.");
      currentState = STATE_CONNECTED;
      updateDisplay(true);  // Full redraw on state change
    }
    
    // Check if it's time to jiggle
    if (currentTime - lastJiggleTime >= JIGGLE_INTERVAL) {
      performJiggle();
      lastJiggleTime = currentTime;
      jiggleCount++;
    }
    
    // Update display every second when connected (only countdown and progress)
    if (currentTime - lastDisplayUpdate >= 1000) {
      nextJiggleIn = (JIGGLE_INTERVAL - (currentTime - lastJiggleTime)) / 1000;
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
  // Move mouse in a small square pattern to create subtle movement
  DisplayState prevState = currentState;
  currentState = STATE_JIGGLING;
  
  Serial.println("Jiggling mouse...");
  updateDisplay(true);  // Full redraw for jiggling state
  
  // Move right
  bleMouse.move(MOVE_DISTANCE, 0);
  delay(50);
  
  // Move down
  bleMouse.move(0, MOVE_DISTANCE);
  delay(50);
  
  // Move left (return to approximately original position)
  bleMouse.move(-MOVE_DISTANCE, 0);
  delay(50);
  
  // Move up (return to approximately original position)
  bleMouse.move(0, -MOVE_DISTANCE);
  delay(50);
  
  Serial.println("Jiggle complete!");
  currentState = STATE_CONNECTED;
  updateDisplay(true);  // Full redraw back to connected state
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
    
    // Draw status icon/indicator
    drawStatusIcon(bleMouse.isConnected());
    
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
      int progress = 100 - ((nextJiggleIn * 100) / (JIGGLE_INTERVAL / 1000));
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
  }
}

// Update only the countdown timer and progress bar (no screen clear)
void updateCountdownOnly() {
  if (currentState != STATE_CONNECTED) return;
  
  // Only update if values changed
  if (nextJiggleIn != lastDrawnNextJiggleIn || jiggleCount != lastDrawnJiggleCount) {
    // Clear and update only the numeric values, not the labels
    
    if (jiggleCount != lastDrawnJiggleCount) {
      // Clear only the number area for jiggle count (after "Jiggles: ")
      Paint_DrawRectangle(95, 60, LCD_WIDTH - 15, 75, 0x0010, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      
      // Draw only the number
      char countStr[16];
      sprintf(countStr, "%lu", jiggleCount);
      Paint_DrawString_EN(95, 60, countStr, &Font16, 0x0010, 0xFFFF);
    }
    
    if (nextJiggleIn != lastDrawnNextJiggleIn) {
      // Clear only the number area for countdown (after "Next in: ")
      Paint_DrawRectangle(95, 85, LCD_WIDTH - 15, 100, 0x0010, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      
      // Draw only the number with 's'
      char timeStr[16];
      sprintf(timeStr, "%lus", nextJiggleIn);
      Paint_DrawString_EN(95, 85, timeStr, &Font16, 0x0010, 0xFFE0);
      
      // Update progress bar
      int progress = 100 - ((nextJiggleIn * 100) / (JIGGLE_INTERVAL / 1000));
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

void drawProgressBar(int percentage) {
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;
  
  int barX = 15;
  int barY = 110;
  int barWidth = LCD_WIDTH - 30;
  int barHeight = 12;
  
  // Draw border
  Paint_DrawRectangle(barX - 1, barY - 1, barX + barWidth + 1, barY + barHeight + 1, 0x07FF, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  
  // Draw filled portion
  int fillWidth = (barWidth * percentage) / 100;
  if (fillWidth > 0) {
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
      Paint_DrawLine(barX, barY + y, barX + fillWidth, barY + y, barColor, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    }
  }
}

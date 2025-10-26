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

// Function declarations
void performJiggle();
void updateLCDStatus(const char* line1, const char* line2 = "", const char* line3 = "");

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
  
  LCD_Clear(0x000f);
  Serial.println("LCD cleared");
  
  // Display initial status first
  Serial.println("Drawing text...");
  Paint_DrawString_EN(10, 30, "BLE Mouse", &Font20, 0x000f, 0xfff0);
  Serial.println("Line 1 drawn");
  Paint_DrawString_EN(10, 60, "Jiggler", &Font20, 0x000f, 0xfff0);
  Serial.println("Line 2 drawn");
  Paint_DrawString_EN(10, 90, "Starting...", &Font20, 0x000f, 0xfff0);
  Serial.println("Line 3 drawn");
  
  delay(1000);
  
  // Start BLE Mouse
  Serial.println("Starting BLE...");
  bleMouse.begin();
  Serial.println("BLE Mouse Jiggler started!");
  Serial.println("Waiting for connection...");
  
  updateLCDStatus("BLE Mouse", "Jiggler", "Waiting...");
  Serial.println("Setup complete!");
}

void loop() {
  // Check if BLE mouse is connected
  if (bleMouse.isConnected()) {
    if (!isJiggling) {
      isJiggling = true;
      Serial.println("Mouse connected! Jiggler active.");
      updateLCDStatus("Connected!", "Jiggling", "Active");
    }
    
    // Check if it's time to jiggle
    unsigned long currentTime = millis();
    if (currentTime - lastJiggleTime >= JIGGLE_INTERVAL) {
      performJiggle();
      lastJiggleTime = currentTime;
    }
  } else {
    if (isJiggling) {
      isJiggling = false;
      Serial.println("Mouse disconnected. Waiting for connection...");
      updateLCDStatus("BLE Mouse", "Jiggler", "Waiting...");
    }
  }
  
  // Small delay to prevent overwhelming the CPU
  delay(100);
}

void performJiggle() {
  // Move mouse in a small square pattern to create subtle movement
  // This pattern is less noticeable than continuous movement in one direction
  
  Serial.println("Jiggling mouse...");
  updateLCDStatus("Jiggling!", "Moving", "cursor...");
  
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
  updateLCDStatus("Connected!", "Jiggling", "Active");
}

void updateLCDStatus(const char* line1, const char* line2, const char* line3) {
  // Clear LCD with dark blue background
  LCD_Clear(0x000f);
  
  // Draw text on LCD - yellow/bright text on dark blue background
  Paint_DrawString_EN(10, 30, line1, &Font20, 0x000f, 0xfff0);
  Paint_DrawString_EN(10, 60, line2, &Font20, 0x000f, 0xfff0);
  Paint_DrawString_EN(10, 90, line3, &Font20, 0x000f, 0xfff0);
}

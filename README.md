# BLE Mouse Jiggler for ESP32-S3-GEEK

A Bluetooth Low Energy (BLE) mouse jiggler for the ESP32-S3-GEEK device that prevents your computer from going to sleep by simulating subtle mouse movements.

## Features

- **BLE Mouse Emulation**: Acts as a Bluetooth mouse that your computer can connect to
- **Automatic Jiggling**: Moves the cursor in a small square pattern every 60 seconds
- **Subtle Movement**: Uses 2-pixel movements that are barely noticeable
- **Status Monitoring**: Serial output shows connection and jiggling status
- **Low Power**: Efficient BLE operation suitable for battery-powered use
- **LCD Ready**: Prepared for optional LCD display integration

## Hardware Requirements

- ESP32-S3-GEEK development board (16 MB flash)
- USB-C cable for programming and power

## Software Requirements

- [PlatformIO](https://platformio.org/) (recommended) or Arduino IDE
- ESP32 BLE Mouse library (automatically installed via PlatformIO)

## Installation

### Using PlatformIO (Recommended)

1. Clone or download this repository
2. Open the project folder in VS Code with PlatformIO extension
3. Connect your ESP32-S3-GEEK via USB
4. Build and upload:
   ```bash
   pio run --target upload
   ```
5. Monitor serial output:
   ```bash
   pio device monitor
   ```

### Using Arduino IDE

1. Install the ESP32 board support in Arduino IDE
2. Install the "ESP32 BLE Mouse" library from Library Manager
3. Open `BLE_Mouse_Jiggler.ino`
4. Select board: **ESP32S3 Dev Module**
5. Configure settings:
   - Flash Size: 16MB
   - Partition Scheme: Default 16MB
   - USB CDC On Boot: Enabled
6. Upload the sketch

## Usage

1. Upload the code to your ESP32-S3-GEEK
2. On your computer, open Bluetooth settings
3. Look for a device named **"Mouse Jiggler"**
4. Pair and connect to the device
5. The mouse will automatically start jiggling every 60 seconds
6. Monitor the Serial output (115200 baud) to see activity logs

## Configuration

You can customize the behavior by modifying these constants in `BLE_Mouse_Jiggler.ino`:

```cpp
const unsigned long JIGGLE_INTERVAL = 60000;  // Time between jiggles (ms)
const int MOVE_DISTANCE = 2;                   // Movement distance (pixels)
```

## Optional LCD Display Support

The code includes commented-out sections for LCD display integration. To enable:

1. Copy the LCD driver files from `examples/Arduino/WIFI/WIFI_AP_LCD/` directory:
   - `LCD_Driver.h` and `LCD_Driver.cpp`
   - `GUI_Paint.h` and `GUI_Paint.cpp`
   - `DEV_Config.h` and `DEV_Config.cpp`
   - Required font files

2. Uncomment the LCD-related sections in `BLE_Mouse_Jiggler.ino`

3. Rebuild and upload

## How It Works

The jiggler creates a BLE HID (Human Interface Device) that appears as a standard Bluetooth mouse to your computer. Every 60 seconds, it sends movement commands in a small square pattern:
- Move right 2 pixels
- Move down 2 pixels  
- Move left 2 pixels
- Move up 2 pixels

This returns the cursor to approximately its original position while keeping the system active.

## Troubleshooting

**Device won't connect:**
- Ensure Bluetooth is enabled on your computer
- Try removing/forgetting the device and pairing again
- Check serial monitor for connection status

**Computer still goes to sleep:**
- Reduce `JIGGLE_INTERVAL` to a shorter time
- Increase `MOVE_DISTANCE` for more noticeable movement

**Upload fails:**
- Hold the BOOT button while uploading
- Ensure correct COM port is selected
- Try a different USB cable

## License

This project is open source and available under the MIT License.

## Credits

Based on the ESP32 BLE Mouse library by T-vK and inspired by the ESP32-S3-GEEK example code from Waveshare.

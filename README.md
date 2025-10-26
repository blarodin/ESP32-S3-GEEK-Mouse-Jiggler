# BLE Mouse Jiggler for ESP32-S3-GEEK

A Bluetooth Low Energy (BLE) mouse jiggler for the ESP32-S3-GEEK device that prevents your computer from going to sleep by simulating subtle mouse movements.

## Features

- **BLE Mouse Emulation**: Acts as a Bluetooth mouse that your computer can connect to
- **Automatic Jiggling**: Moves the cursor in a small square pattern every 30 seconds
- **Subtle Movement**: Uses 2-pixel movements that are barely noticeable
- **Beautiful LCD Display**: Full-featured color display with real-time status
  - State-based UI (Initializing, Waiting, Connected, Jiggling)
  - Full-width blue header bar with device name
  - Live countdown timer showing seconds until next jiggle
  - Jiggle counter tracking total activations
  - Color-changing progress bar (green → yellow → red)
  - Connection status indicator dot (red/green)
  - Optimized partial updates for flicker-free rendering
- **Status Monitoring**: Serial output shows connection and jiggling status
- **Low Power**: Efficient BLE operation suitable for battery-powered use

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
5. The mouse will automatically start jiggling every 30 seconds
6. Monitor the LCD display for real-time status and countdown
7. Optionally monitor the Serial output (115200 baud) to see activity logs

## Configuration

You can customize the behavior by modifying these constants in `src/main.cpp`:

```cpp
const unsigned long JIGGLE_INTERVAL = 30000;  // Time between jiggles (30 seconds)
const int MOVE_DISTANCE = 2;                   // Movement distance (pixels)
```

### Display Configuration

The LCD display is fully integrated and shows:
- Connection status with visual indicator
- Real-time countdown timer
- Progress bar showing time until next jiggle
- Total jiggle count

The display uses optimized rendering to minimize flickering and power consumption.

## LCD Display

The ESP32-S3-GEEK's built-in 1.14" LCD (135x240 pixels) displays a beautiful, informative interface:

**Display Features:**
- Full-width blue header with "MOUSE JIGGLER" title
- Connection status indicator (red dot when waiting, green when connected)
- State display: WAITING (yellow) or ACTIVE (green)
- Real-time countdown showing seconds until next jiggle
- Jiggle counter showing total activations
- Animated progress bar with color gradient:
  - Green (0-33%): Just activated
  - Yellow (33-66%): Halfway to next jiggle
  - Red (66-100%): About to jiggle

**Display Optimization:**
- Smart partial updates: only redraws changing elements
- Flicker-free progress bar rendering
- Labels remain static while values update
- Minimal CPU usage for display updates

## How It Works

The jiggler creates a BLE HID (Human Interface Device) that appears as a standard Bluetooth mouse to your computer. Every 30 seconds, it sends movement commands in a small square pattern:
- Move right 2 pixels
- Move down 2 pixels  
- Move left 2 pixels
- Move up 2 pixels

This returns the cursor to approximately its original position while keeping the system active. The LCD display provides real-time feedback showing the countdown until the next jiggle, connection status, and total activity count.

## Troubleshooting

**Device won't connect:**
- Ensure Bluetooth is enabled on your computer
- Try removing/forgetting the device and pairing again
- Check serial monitor for connection status

**Computer still goes to sleep:**
- The default interval is 30 seconds, which should be sufficient for most systems
- If needed, reduce `JIGGLE_INTERVAL` to a shorter time (e.g., 15000 for 15 seconds)
- Increase `MOVE_DISTANCE` for more noticeable movement (e.g., 5-10 pixels)

**Upload fails:**
- Hold the BOOT button while uploading
- Ensure correct COM port is selected
- Try a different USB cable

## License

This project is open source and available under the MIT License.

## Credits

Based on the ESP32 BLE Mouse library by T-vK and inspired by the ESP32-S3-GEEK example code from Waveshare.

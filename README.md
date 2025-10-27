# BLE Mouse Jiggler for ESP32-S3-GEEK

A Bluetooth Low Energy (BLE) mouse jiggler for the ESP32-S3-GEEK device that prevents your computer from going to sleep by simulating subtle mouse movements.

## Features

- **BLE Mouse Emulation**: Acts as a Bluetooth mouse that your computer can connect to
- **WiFi Configuration Portal**: Built-in web interface for easy configuration
  - Configure jiggle interval (5-300 seconds)
  - Choose between square or random movement patterns
  - Adjust movement distance
  - Customize BLE device name
  - Change WiFi AP credentials
  - All settings persist across reboots
- **Movement Modes**:
  - **Square Pattern**: Moves in a predictable 4-point square
  - **Random Pattern**: Random direction and distance within configured range
- **Beautiful LCD Display**: Full-featured color display with real-time status
  - State-based UI (Initializing, Waiting, Connected, Jiggling)
  - Full-width blue header bar with device name and WiFi indicator
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

### Initial Setup

1. Upload the code to your ESP32-S3-GEEK
2. The device displays a boot screen (2 seconds) showing:
   - **MOUSE JIGGLER** (white)
   - **ESP32-S3-GEEK** (cyan)
   - **YEVHENII RODIN** (yellow)
   - **BLARODIN@GMAIL.COM** (yellow)
   
3. After the boot screen, WiFi AP information alternates between 2 screens:
   
   **Screen 1 - WiFi Network:**
   - SSID: `MouseJiggler-Config` (default)
   - Password: `jiggler123` (default)
   
   **Screen 2 - Connection Info:**
   - IP Address: `192.168.4.1`
   - URL to open in browser
   
   Each screen displays for 1.5 seconds, then the device starts BLE mode

### Configuration (Optional)

4. Connect to the WiFi network on your phone or computer
5. Open a browser and navigate to `http://192.168.4.1`
6. Configure your preferred settings:
   - Jiggle interval (how often to move the mouse)
   - Movement pattern (square or random)
   - Movement distance
   - BLE device name
   - WiFi credentials (if you want to change them)
7. Click "Save Settings"
   - Settings are saved to non-volatile storage
   - **Device automatically reboots** with new configuration
   - Reconnect to WiFi after reboot to verify settings

### Normal Operation

8. On your computer, open Bluetooth settings
9. Look for a device named **"Mouse Jiggler"** (or your custom name)
10. Pair and connect to the device
11. The mouse will automatically start jiggling based on your configuration
12. Monitor the LCD display for real-time status and countdown
13. The WiFi AP remains active for future configuration changes

## Configuration

Configuration is now done through the web interface! No need to modify code.

### Web Interface Settings

**Jiggle Settings:**
- **Interval**: 5-300 seconds (default: 30 seconds)
- **Move Distance**: 1-20 pixels (default: 2 pixels)
- **Random Movements**: Enable/disable random pattern (default: off)
- **Random Min/Max Distance**: Configure random movement range (default: 1-5 pixels)

**Device Settings:**
- **BLE Device Name**: Customize Bluetooth name (default: "Mouse Jiggler")
  - Device automatically reboots after saving to apply changes

**WiFi AP Settings:**
- **SSID**: Change WiFi network name (default: "MouseJiggler-Config")
- **Password**: Change WiFi password (default: "jiggler123")
  - Minimum 8 characters required

All settings are automatically saved to non-volatile storage and persist across power cycles.

### Display Configuration

The LCD display is fully integrated and shows:
- WiFi indicator in header bar
- Connection status with visual indicator
- Real-time countdown timer
- Progress bar showing time until next jiggle
- Total jiggle count

The display uses optimized rendering to minimize flickering and power consumption.

## LCD Display

The ESP32-S3-GEEK's built-in 1.14" LCD (135x240 pixels) displays a beautiful, informative interface:

**Boot Screen (2 seconds):**
- Left-aligned display showing:
  - **MOUSE JIGGLER** (white)
  - **ESP32-S3-GEEK** (cyan)
  - **YEVHENII RODIN** (yellow - author)
  - **BLARODIN@GMAIL.COM** (yellow - contact)

**Startup Display (WiFi Configuration):**

Two alternating screens show WiFi setup information:
- **Screen 1**: WiFi network credentials (SSID and password)
- **Screen 2**: Browser connection details (IP address and URL)
- Each screen displays for 1.5 seconds using Font16 for readability
- After 3 seconds total, transitions to normal operation mode

**Normal Operation Display:**
- Full-width blue header with "MOUSE JIGGLER" title and WiFi indicator
- Connection status indicator (red dot when waiting, green when connected)
- State display: WAITING (yellow) or ACTIVE (green)
- Real-time countdown showing seconds until next jiggle
- Jiggle counter tracking total activations
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

The jiggler creates a BLE HID (Human Interface Device) that appears as a standard Bluetooth mouse to your computer.

**Square Pattern Mode** (default):
- Move right (configurable distance)
- Move down
- Move left
- Move up
- Returns cursor to approximately original position

**Random Pattern Mode**:
- Moves in a random direction with random distance
- Distance range is configurable (e.g., 1-5 pixels)
- Returns to origin after each movement

The WiFi Access Point runs continuously, allowing you to change settings at any time via the web interface. The LCD display provides real-time feedback showing the countdown until the next jiggle, connection status, and total activity count.

## Troubleshooting

**Device won't connect:**
- Ensure Bluetooth is enabled on your computer
- Try removing/forgetting the device and pairing again
- Check serial monitor for connection status

**Computer still goes to sleep:**
- The default interval is 30 seconds, which should be sufficient for most systems
- Use the web interface to reduce the interval (e.g., 15 seconds)
- Increase the movement distance via web interface (e.g., 5-10 pixels)
- Try enabling random movements for more varied activity

**Can't access web interface:**
- Ensure you're connected to the correct WiFi network
- Check the LCD display for the correct IP address (should be 192.168.4.1)
- Try forgetting the network and reconnecting
- Check serial monitor for WiFi status messages

**Upload fails:**
- Hold the BOOT button while uploading
- Ensure correct COM port is selected
- Try a different USB cable

## License

This project is open source and available under the MIT License.

## Credits

Based on the ESP32 BLE Mouse library by T-vK and inspired by the ESP32-S3-GEEK example code from Waveshare.

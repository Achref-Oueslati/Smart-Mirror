# Smart-Mirror
## Overview
The Smart Mirror is an innovative IoT application built using the ESP32 microcontroller. It combines modern technology to enhance user experience with features like voice activation, automated lighting, photo capture and upload to Dropbox, and an OLED display for status updates.

---

## Features
1. **Light Sensor-Based LED Control**:
   - Automatically adjusts LED brightness based on room lighting conditions.

2. **Voice Activation**:
   - Wake up and put the mirror to sleep with simple voice commands like "wake up" and "sleep".

3. **Camera Integration**:
   - Captures photos and uploads them directly to Dropbox for easy access.

4. **OLED Display**:
   - Shows real-time status messages such as "Mirror is awake" or "Mirror is sleeping".

5. **IoT-Enabled**:
   - Connects to WiFi for seamless Dropbox integration and potential remote control.

---

## Requirements
- ESP32 development board (with a camera module)
- OLED display (SSD1306)
- Light sensor (LDR)
- WS2812B LED strip
- Microphone module for voice activation
- WiFi network

### Software:
- Arduino IDE
- Required libraries:
  - `Adafruit_NeoPixel`
  - `Adafruit_SSD1306`
  - `SpeechRecognition`
  - `esp_camera`
  - `HTTPClient`

---

## Usage
- **Voice Commands**:
  - Say "wake up" to activate the mirror.
  - Say "sleep" to deactivate the mirror.
- **Photo Capture**:
  - When the mirror is awake, it periodically captures and uploads photos to Dropbox.
- **Automatic Lighting**:
  - The mirror adjusts its lighting based on ambient conditions using the light sensor.

---

## Future Improvements
- Add a touchscreen for enhanced interactivity.
- Integrate weather and calendar APIs for additional smart features.
- Develop a companion mobile app for better user control.

---

## Contributing
Feel free to fork the repository and submit pull requests to improve the project. Contributions are welcome!

---

## License
This project is open-sourced under the MIT License. See the `LICENSE` file for details.



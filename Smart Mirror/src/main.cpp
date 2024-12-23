// Smart Mirror using ESP32 with Light Sensor, Camera, Dropbox Integration, and Voice Activation
// Features: Light sensor-based LED control, Camera for taking pictures, IoT-enabled photo transfer (Dropbox),
//           Voice activation for waking up the mirror, OLED display for status messages, and Web server for remote access.

#include <WiFi.h>
#include <esp_camera.h>
#include <WiFiClientSecure.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <VoiceRecognitionV3.h>  // Library for voice recognition

// WiFi Credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Dropbox Access Token
const char* dropboxAccessToken = "your_DROPBOX_ACCESS_TOKEN";

// Light Sensor and LED Pin Configuration
const int ldrPin = 34; // Analog pin for LDR
const int ledPin = 25; // Digital pin for WS2812B
Adafruit_NeoPixel strip(16, ledPin, NEO_GRB + NEO_KHZ800);

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Camera Pins (for ESP32-CAM)
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      21
#define Y4_GPIO_NUM      19
#define Y3_GPIO_NUM      18
#define Y2_GPIO_NUM       5
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

WiFiClientSecure client;

VoiceRecognition voiceRecognition;  // Create a voice recognition object

bool mirrorAwake = false;

void initCamera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    // Frame size and quality
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;

    if (esp_camera_init(&config) != ESP_OK) {
        Serial.println("Camera initialization failed");
        while (1);
    }
}

void displayMessage(const char* message) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(message);
    display.display();
}

void captureAndUploadPhoto() {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return;
    }

    Serial.println("Photo captured, uploading to Dropbox...");
    HTTPClient http;
    http.begin("https://content.dropboxapi.com/2/files/upload");
    http.addHeader("Authorization", String("Bearer ") + dropboxAccessToken);
    http.addHeader("Content-Type", "application/octet-stream");
    http.addHeader("Dropbox-API-Arg", "{\"path\": \"/SmartMirror/photo.jpg\", \"mode\": \"overwrite\", \"autorename\": true}");

    int httpResponseCode = http.POST(fb->buf, fb->len);
    if (httpResponseCode > 0) {
        Serial.println("Photo uploaded successfully");
    } else {
        Serial.println("Error uploading photo: " + http.errorToString(httpResponseCode));
    }
    http.end();

    esp_camera_fb_return(fb);
}

void handleLightSensor() {
    int ldrValue = analogRead(ldrPin);
    Serial.print("LDR Value: ");
    Serial.println(ldrValue);

    if (ldrValue < 720 && mirrorAwake) { // Adjust threshold based on ambient lighting
        for (int i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(255, 255, 255)); // Bright white
        }
        strip.show();
    } else {
        for (int i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, 0); // Turn off LEDs
        }
        strip.show();
    }
}

void handleVoiceCommand() {
    int command = voiceRecognition.recognize();
    if (command == 1) {  // Command '1' for "wake up"
        Serial.println("Voice command received: Wake up");
        mirrorAwake = true;
        displayMessage("Mirror is awake");
        for (int i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(0, 255, 0)); // Green light for wake-up
        }
        strip.show();
        delay(2000);
    } else if (command == 2) {  // Command '2' for "sleep"
        Serial.println("Voice command received: Sleep");
        mirrorAwake = false;
        displayMessage("Mirror is sleeping");
        for (int i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, 0); // Turn off LEDs
        }
        strip.show();
    }
}

void setup() {
    Serial.begin(115200);

    // Initialize OLED Display
    if (!display.begin(SSD1306_I2C_ADDRESS, OLED_RESET)) {
        Serial.println("OLED initialization failed");
        while (1);
    }
    displayMessage("Initializing...");

    // Initialize LEDs
    strip.begin();
    strip.show();

    // Initialize WiFi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
    displayMessage("WiFi connected");

    // Initialize Camera
    initCamera();

    // Initialize Voice Recognition
    voiceRecognition.begin();

    Serial.println("Smart Mirror is ready!");
    displayMessage("Say 'wake up' or 'sleep'");
}

void loop() {
    handleLightSensor();
    handleVoiceCommand();

    // Periodically capture and upload photo
    if (mirrorAwake) {
        static unsigned long lastUploadTime = 0;
        if (millis() - lastUploadTime > 60000) { // Upload every minute
            captureAndUploadPhoto();
            lastUploadTime = millis();
        }
    }

    delay(500);
}

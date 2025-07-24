#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

// Wi-Fi credentials
const char* ssid = "BrubakerWifi";
const char* password = "Pre$ton01";

// UDP configuration
const unsigned int localPort = 1234; // UDP port to listen on
WiFiUDP UDP;

// LED strip configuration
#define NUM_LEDS 300
#define DATA_PIN 2 // GPIO2
#define BRIGHTNESS 255 // 0-255
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    // Initialize Serial
    Serial.begin(115200);
    delay(100);

    // Initialize LED strip
    strip.begin();
    strip.setBrightness(BRIGHTNESS);
    setLedsOff();
    strip.show();

    // Connect to Wi-Fi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println("IP address: " + WiFi.localIP().toString());

    // Start UDP server
    UDP.begin(localPort);
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localPort);
}

void setLedsOff() {
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, 0, 0, 0);
    }
    strip.show();
}

void loop() {
    // Check Wi-Fi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected, reconnecting...");
        WiFi.reconnect();
        delay(2000);
        return;
    }

    // Check for UDP packets
    int packetSize = UDP.parsePacket();
    if (packetSize) {
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, UDP.remoteIP().toString().c_str(), UDP.remotePort());

        // Expecting 300 RGB values (900 bytes)
        if (packetSize == NUM_LEDS * 3) {
            uint8_t buffer[NUM_LEDS * 3];
            int len = UDP.read(buffer, NUM_LEDS * 3);
            if (len > 0) {
                // Set LED colors from buffer
                for (int i = 0; i < NUM_LEDS; i++) {
                    uint8_t r = buffer[i * 3];
                    uint8_t g = buffer[i * 3 + 1];
                    uint8_t b = buffer[i * 3 + 2];
                    strip.setPixelColor(i, r, g, b);
                }
                strip.show();
            }
        } else {
            Serial.println("Invalid packet size");
        }
    }
}

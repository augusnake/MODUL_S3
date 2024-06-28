#ifndef pinout_H
#define pinout_H
#include <ArduinoJson.h>
#include <SD_MMC.h>
#include <SPI.h>
#include <Wire.h>

class pinout {
public:
    pinout(String model) {
        if (model == "v3.0") {
            BOOT0 = 0;
            LED = 1;
            SDA = 14;
            SCL = 21;
            clk = 48;
            cmd = 37;
            d0 = 38;
            d1 = 39;
            d2 = 35;
            d3 = 36; // GPIO 34 is not broken-out on ESP32-S3-DevKitC-1 v1.1

            ADXL375_SCK = 42;
            ADXL375_MISO = 41;
            ADXL375_MOSI = 40;
            ADXL375_CS = 2;
            LORA_CS = 12;

            ON_SICK = 13;
            SICK1 = 7;
            RFM95_RST = 10;
            RFM95_INT = 11;
            RFM95_CS = 12;
            battPin = 9;
            ledPin = 4;
            RxPin = 18;
            TxPin = 17;
        }
        if (model == "v3.1") {
            BOOT0 = 0;
            LED = 1;
            SDA = 14;
            SCL = 21;
            clk = 48;
            cmd = 37;
            d0 = 38;
            d1 = 39;
            d2 = 35;
            d3 = 36; // GPIO 34 is not broken-out on ESP32-S3-DevKitC-1 v1.1

            ADXL375_SCK = 42;
            ADXL375_MISO = 41;
            ADXL375_MOSI = 40;
            ADXL375_CS = 2;
            LORA_CS = 12;

            ON_SICK = 13;
            SICK1 = 8;
            RFM95_RST = 10;
            RFM95_INT = 11;
            RFM95_CS = 12;
            battPin = 9;
            ledPin = 4;
            RxPin = 18;
            TxPin = 17;

            Ext_SPI_CS= 04;
        }
        
    };
    int ledPin;
    String ledState;
    float bright = 12.0;
    float color[3] = {0., 0., 0.};
    int BOOT0;
    int LED;
    int SDA;
    int SCL;
    int clk;
    int cmd;
    int d0;
    int d1;
    int d2;
    int d3;

    int ADXL375_SCK;
    int ADXL375_MISO;
    int ADXL375_MOSI;
    int ADXL375_CS;
    int LORA_CS;

    int ON_SICK;
    int SICK1;
    int RFM95_RST;
    int RFM95_INT;
    int RFM95_CS;
    int battPin;
    int RxPin; 
    int TxPin;

    int Ext_SPI_CS;

    // Function to combine RGB components into a 32-bit color value
    uint32_t neopixelColor(uint8_t red, uint8_t green, uint8_t blue) {
        return (uint32_t(red) << 16) | (uint32_t(green) << 8) | blue;
    };

    // Function to convert a Wheel position to RGB color
    uint32_t Wheel(byte WheelPos) {
        WheelPos = 255 - WheelPos;
        if (WheelPos < 85) {
            return neopixelColor(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        if (WheelPos < 170) {
            WheelPos -= 85;
            return neopixelColor(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        WheelPos -= 170;
        return neopixelColor(WheelPos * 3, 255 - WheelPos * 3, 0);
    };
    void initBlink() {
        neopixelWrite(LED, bright, 0, 0); // R
        delay(200);
        neopixelWrite(LED, 0, 0, 0); // 0
        delay(200);
        neopixelWrite(LED, 0, 0, bright); // B
        delay(200);
    };
    void rainbowLoop(int wait) {
        for (int j = 0; j < 256; j++) {
            uint32_t color = Wheel(((j * 3) + 0) & 255); // Incrementing by 3 for a single RGB LED
            uint8_t red = (color >> 16) & 0xFF;
            uint8_t green = (color >> 8) & 0xFF;
            uint8_t blue = color & 0xFF;
            neopixelWrite(LED, red / 20, green / 20, blue / 20);
            delay(wait);
        }
    };
    void pinSetup() {
        pinMode(LED, OUTPUT);
        initBlink();
        Wire.begin(SDA, SCL);
        SPI.begin(ADXL375_SCK, ADXL375_MISO, ADXL375_MOSI);
    };
    void loopBlink(bool bBlink) {
        // blinking
        float colorB[3] = {};
        if (bBlink) {
            colorB[0] = color[0];
            colorB[1] = color[1];
            colorB[2] = color[2];
        } else {
            colorB[0] = 0;
            colorB[1] = 0;
            colorB[2] = 0;
        }
        Serial.print("blink :");
        Serial.println(bBlink);
        neopixelWrite(LED, colorB[0], colorB[1], colorB[2]);
    }

    bool sdmmcSetup() {
        if (!SD_MMC.setPins(clk, cmd, d0, d1, d2, d3)) {
            Serial.println("Pin change failed!");
            return false;
        }
        if (!SD_MMC.begin("/sdcard", true, false, 20000, 5)) {
            Serial.println("Card Mount Failed");
            neopixelWrite(LED, 0, bright, bright);
            delay(500);
            return false;
        }
        return true;
    }
};

#endif
// Import required libraries
#include <Preferences.h>

// Import modules
#include "capteurs.h"
#include "charger.h"
#include "comLORA.h"
#include "pinout.h"
#include "rtcClass.h"

String cardModel = "v3.1";
String breakout = "ripperv1";
String etrierModel="ripperL17";
Preferences preferences;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

pinout pins(cardModel,breakout);
rtcClass rtc(&preferences);
capteurs cap(&pins, &rtc, SD_MMC, &preferences,etrierModel);
comLORA lora(&pins, &cap);
charger charge(&pins, &rtc, SD_MMC, &preferences, &cap, &server, &ws, &lora);

void setup() {
    Serial.begin(115200);
    Serial.println("begin");

    pins.pinSetup();
    delay(1000);
    cap.pinSetup();
    lora.pinSetup();
    // lora.rf95Setup();
    pins.sdmmcSetup();
    rtc.rtcSetup();
    cap.lsmSetup();
    // cap.adxlSetup();
    charge.initSPIFFS();
    charge.initWebSocket();
    charge.setup();
    cap.measBatt();
}
byte message[100];
int ind;
#define addbyte(val){ message[ind]=val; ind++;}
#define add2byte(val){ message[ind]=lowByte(val); ind++; message[ind]=highByte(val); ind++;}
#define add3byte(val){message[ind]=lowByte(val); ind++; message[ind]=lowByte(val>>8); ind++; message[ind]=lowByte(val>>16); ind++;}
void mainRipper() {
    // init LDC
    // mesure LDC
    // mettre resultats dans message
    // rafale message
    pins.all_CS_high();
    neopixelWrite(pins.LED, 0, 12, 0);
    int batt = cap.measBatt() * 100;
    cap.mesureRipper(10,"LDC1");
    int id = 12;
    add2byte(id);
    add3byte(cap.ldc1->f1Max);
    add3byte(cap.ldc1->f1Min);
    add3byte(cap.ldc1->f1moy);
    add3byte(cap.ldc1->f2Max);
    add3byte(cap.ldc1->f2Min);
    add3byte(cap.ldc1->f2moy);
    add2byte(batt);
    lora.rafale(message, 14, id);
    rtc.goSleep(20);
}

void loop() {
    // LDCTest();
    charge.routinecharge(&mainRipper);
}

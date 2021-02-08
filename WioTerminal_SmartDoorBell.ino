// Wio Terminal Smart Doorbell
// Author: Jonathan Tan
// Feb 2021
// Written for Seeed Wio Terminal

#define BLYNK_PRINT Serial

#include <TFT_eSPI.h>
#include <rpcWiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleWioTerminal.h>
#include "Free_Fonts.h"

// Define PINs
#define PIR PIN_WIRE_SCL
#define LCD_BACKLIGHT (72Ul)

// Define Timeouts
#define LCDTimeout 20000
#define notifyTimeout 20000

TFT_eSPI tft;
BlynkTimer timer;
 
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "O7ey1op5Au5wOGVI5TQp8puPtKZGhKD3";
 
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "KRHome";
char pass[] = "whoask38KR";

// Standard Outputs
String person = "Movement Detected.";
String absent = "All clear.";

// Variables
int person_state;
int notify_me;
long LCDmillis;
long notifyMillis;
long millisMarkerDebug;

BLYNK_WRITE(V1) {
 int pinValue = param.asInt();

 // Debug: Check Toggle Inputs
 Serial.print("Input Value: ");
 Serial.println(pinValue);

 // Toggle According to Pin Value
 if (pinValue == 1) {
    notify_me = 1;
  } else {
    notify_me = 0;
 }
}

void sendPIRDetect() {
  if(digitalRead(PIR)) {
    person_state = 1;
    Blynk.virtualWrite(V0, person);
    if (notify_me == 1 && millis() - notifyMillis > notifyTimeout) {
      Blynk.notify("Movement detected.");
      notifyMillis = millis();
    }
  } else {
    person_state = 0;
    Blynk.virtualWrite(V0, absent);
  }
}

void eraseLCD() {
  tft.fillRect(0, 0, TFT_HEIGHT, TFT_WIDTH, TFT_BLACK);
}

void setup() {
  // Debug console
  Serial.begin(9600);
  // Actual Setup
  pinMode(PIR, INPUT);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.println("Initialising...");
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, sendPIRDetect);
  tft.println("Connected!");
  delay(2000);
  tft.fillScreen(TFT_BLACK);
}
 
void loop() {
    Blynk.run();
    timer.run();

    // Turn LCD off after
    if ( person_state == 0 && (millis() - LCDmillis) > LCDTimeout) {
      eraseLCD();
      digitalWrite(LCD_BACKLIGHT, LOW);
    }

    if (person_state == 1 && (millis() - LCDmillis) > LCDTimeout) {
      digitalWrite(LCD_BACKLIGHT, HIGH);
      LCDmillis = millis();
      tft.setTextDatum(MC_DATUM);
      tft.setFreeFont(FSSB18);
      tft.drawString("Hello!", 160, 100);
      tft.setFreeFont(FSS9);
      tft.drawString("A doorbell notification", 160, 135);
      tft.drawString("will be sent.", 160, 155);
    }
    
    // For Debugging in Serial Terminal
    
    if ( (millis() - millisMarkerDebug) > 1000) {
        millisMarkerDebug = millis();
        if (digitalRead(PIR)) {
          Serial.println("Movement Detected.");
        } else {
          Serial.println("All Clear.");
        }
    }
    
}

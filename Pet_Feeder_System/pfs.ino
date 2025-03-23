#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Servo.h>
#include "HX711.h"
#include <Wire.h>
#include "RTClib.h"

char auth[] = "YOUR_BLYNK_AUTH_TOKEN";  
char ssid[] = "YOUR_WIFI_SSID";         
char pass[] = "YOUR_WIFI_PASSWORD";     

#define SERVO_PIN 5
#define LOADCELL_DOUT_PIN 4
#define LOADCELL_SCK_PIN 15

Servo foodServo;
HX711 scale;
RTC_DS3231 rtc;

void setup() {
    Serial.begin(115200);
    Blynk.begin(auth, ssid, pass);

    foodServo.attach(SERVO_PIN);
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    Wire.begin();

    if (!rtc.begin()) {
        Serial.println("RTC not found!");
    }
}

BLYNK_WRITE(V0) {  // Manual Feeding Button
    dispenseFood();
}

void dispenseFood() {
    foodServo.write(90);  // Open gate
    delay(3000);
    foodServo.write(0);   // Close gate

    float weight = scale.get_units();
    Serial.println("Food Dispensed: " + String(weight) + "g");
    Blynk.virtualWrite(V1, weight);
    Blynk.notify("🐾 Pet has been fed " + String(weight) + "g of food.");
}

void loop() {
    Blynk.run();
}

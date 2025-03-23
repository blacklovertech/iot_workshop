#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = "YOUR_BLYNK_AUTH_TOKEN";  // Replace with your Blynk auth token
char ssid[] = "YOUR_WIFI_SSID";         // Replace with your Wi-Fi SSID
char pass[] = "YOUR_WIFI_PASSWORD";     // Replace with your Wi-Fi password

#define ENA 5    // Motor Speed (PWM)
#define IN1 18   // Motor Direction
#define IN2 19   // Motor Direction

void setup() {
    Serial.begin(115200);
    Blynk.begin(auth, ssid, pass);
    
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
}

BLYNK_WRITE(V0) {  // Button for Motor ON/OFF
    int motorState = param.asInt();
    if (motorState == 1) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        Serial.println("Motor ON");
    } else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        Serial.println("Motor OFF");
    }
}

BLYNK_WRITE(V1) {  // Slider for Speed Control
    int speedValue = param.asInt();
    analogWrite(ENA, speedValue);
    Serial.print("Motor Speed: ");
    Serial.println(speedValue);
}

void loop() {
    Blynk.run();
}

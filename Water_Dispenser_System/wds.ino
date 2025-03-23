#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = "YOUR_BLYNK_AUTH_TOKEN";  
char ssid[] = "YOUR_WIFI_SSID";         
char pass[] = "YOUR_WIFI_PASSWORD";     

#define WATER_FLOW_SENSOR_PIN 14
#define MOTOR_RELAY_PIN 5  

volatile int pulseCount = 0;
float flowRate;

void IRAM_ATTR countPulse() { pulseCount++; }

void setup() {
    Serial.begin(115200);
    Blynk.begin(auth, ssid, pass);

    pinMode(WATER_FLOW_SENSOR_PIN, INPUT_PULLUP);
    pinMode(MOTOR_RELAY_PIN, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(WATER_FLOW_SENSOR_PIN), countPulse, RISING);
}

BLYNK_WRITE(V0) {  // Toggle Water Pump
    int pumpState = param.asInt();
    digitalWrite(MOTOR_RELAY_PIN, pumpState);
    Serial.println(pumpState ? "Water Pump ON" : "Water Pump OFF");
}

void calculateFlow() {
    float litersPerMinute = (pulseCount / 7.5);
    flowRate = litersPerMinute;
    Blynk.virtualWrite(V1, flowRate);
    Serial.println("Water Flow Rate: " + String(flowRate) + " L/min");
    pulseCount = 0;
}

void loop() {
    Blynk.run();
    calculateFlow();
    delay(1000);
}

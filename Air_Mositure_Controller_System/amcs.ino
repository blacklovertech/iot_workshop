#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define DHTPIN 4          // DHT11/DHT22 sensor pin
#define DHTTYPE DHT11     // Change to DHT22 if needed
#define RELAY_PIN 5       // Fan relay pin
#define HUMIDITY_THRESHOLD_LOW 40
#define HUMIDITY_THRESHOLD_HIGH 70

DHT dht(DHTPIN, DHTTYPE);
char auth[] = "Your_Blynk_Auth_Token";  
char ssid[] = "Your_WiFi_SSID";  
char pass[] = "Your_WiFi_Password";  

void setup() {
    Serial.begin(115200);
    Blynk.begin(auth, ssid, pass);
    dht.begin();
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH);  // Fan OFF by default
}

void loop() {
    Blynk.run();
    float humidity = dht.readHumidity();
    Serial.println("Humidity: " + String(humidity));

    Blynk.virtualWrite(V1, humidity);  // Send humidity to Blynk app

    if (humidity < HUMIDITY_THRESHOLD_LOW) {
        digitalWrite(RELAY_PIN, LOW);  // Fan ON
        Blynk.notify("Humidity Low! Turning ON Fan.");
        Blynk.virtualWrite(V2, "Fan: ON");
    } 
    else if (humidity > HUMIDITY_THRESHOLD_HIGH) {
        digitalWrite(RELAY_PIN, HIGH);  // Fan OFF
        Blynk.notify("Humidity High! Turning OFF Fan.");
        Blynk.virtualWrite(V2, "Fan: OFF");
    }
    
    delay(2000);  // Update every 2 seconds
}

BLYNK_WRITE(V3) {  // Manual control from Blynk
    int value = param.asInt();
    digitalWrite(RELAY_PIN, value);
    Blynk.virtualWrite(V2, value ? "Fan: OFF" : "Fan: ON");
}

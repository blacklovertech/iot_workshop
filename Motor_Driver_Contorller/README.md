# 🚀 ESP32 + Blynk Motor Control

## 📌 Features
- 📡 **Control motor ON/OFF via Blynk app**
- 🔄 **Adjust speed using a slider**
- 🌐 **Wi-Fi-based remote control**
- 📲 **Real-time status updates in the app**

## 🛠️ Components
- ESP32 (Wi-Fi)
- L298N Motor Driver
- DC Motor
- Power Source (Battery or Adapter)

## 🔧 Setup Guide
1. Install **Blynk IoT App** (Android/iOS).
2. Create a new Blynk project.
3. Add:
   - **Button (V0)** → Motor ON/OFF
   - **Slider (V1)** → Motor Speed (0-255)
4. Upload the ESP32 code with your Wi-Fi credentials.
5. Open Blynk and control the motor remotely!

## 🚀 Future Upgrades
- Add **direction control** for forward/reverse.
- Create a **web dashboard** for motor control.
- Use **MQTT or Firebase** for cloud integration.

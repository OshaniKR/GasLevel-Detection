# **Gas Leak Detection System** 🚨

This project implements a **Gas Leak Detection System** using an **ESP32 microcontroller**, an **OLED display**, a **gas sensor (e.g., MQ-2)**, a **buzzer**, and **Blynk** for remote monitoring. When the gas level exceeds a certain threshold, the system triggers an alert using a buzzer and sends an SMS through the **Notify.lk API**. Additionally, the gas level is monitored and displayed in real-time using the **Blynk app**.

## **Features:**
- **Monitors gas levels** using an analog gas sensor.
- **Displays gas level and status** on an OLED screen.
- **Sends SMS alert** when a gas leak is detected.
- **Real-time monitoring** via the **Blynk app** 📱.
- **Buzzer activation** for gas leak warning ⚠️.

## **Requirements:**
- **ESP32 board** 
- **OLED Display (SSD1306)**
- **Gas sensor** (MQ-2)
- **Buzzer**
- **Blynk account** 

## **Libraries Used:**
- **Adafruit_SSD1306**: For controlling the OLED display.
- **BlynkSimpleEsp32**: For connecting the ESP32 to the Blynk app.
- **WiFi**: For Wi-Fi connectivity.
- **HTTPClient**: For sending HTTP requests to the Notify.lk API.

## **Setup:**
1. **Clone the repository** to your local machine.
2. **Install the required libraries**:
   - `Adafruit_SSD1306`
   - `BlynkSimpleEsp32`
   - `WiFi`
   - `HTTPClient`
3. **Update the code**:
   - Replace the **Wi-Fi credentials** (`ssid` and `pass`).
   - Replace the **Blynk authentication token** (`auth`).
   - Replace **Notify.lk API credentials** (`notifylk_user_id`, `notifylk_api_key`, `notifylk_sender_id`, `recipient_number`).
4. **Hardware setup**:
   - Connect the **gas sensor** (e.g., MQ-2) to the **sensor pin** (GPIO 34 on ESP32).
   - Connect the **buzzer** to the **buzzer pin** (GPIO 2 on ESP32).
   - Connect the **OLED display** (SSD1306) to the **I2C pins** (default SDA and SCL on ESP32).

5. **Upload the code** to the **ESP32**.
6. Open the **Blynk app** to monitor gas levels in real-time 📲.




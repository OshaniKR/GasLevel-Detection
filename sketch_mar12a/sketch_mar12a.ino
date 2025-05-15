#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#define BLYNK_TEMPLATE_ID "TMPL6cxu4xpHI"
#define BLYNK_TEMPLATE_NAME "gas leakage detection system"
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define sensor 34
#define buzzer 2

BlynkTimer timer;

// Enter your Auth token
char auth[] = "Z5PEq8XX-mrcsrAHfMstfd26IPH00ao0";

// Enter your WiFi SSID and password
char ssid[] = "vivo 1938";
char pass[] = "123uresha";

// Notify.lk API Credentials
const char* notifylk_user_id = "29541";
const char* notifylk_api_key = "eSp0fCQ2jLICga9Ms0VO";
const char* notifylk_sender_id = "NotifyDEMO";  // Replace with your approved sender ID
const char* recipient_number = "94764742889";  // Sri Lankan mobile format

bool smsSent = false; // To prevent multiple SMS spam
bool wifiConnected = false;

// Function to URL encode the message
String urlEncode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
  }
  return encodedString;
}

void setup() {
    Serial.begin(115200);
    
    // Initialize OLED display
    if (!display.begin(SSD1306_BLACK, 0x3C)) {
        Serial.println("SSD1306 allocation failed");
        while (true);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.println("System Loading...");
    display.display();
    
    pinMode(buzzer, OUTPUT);
    
    // Connect to WiFi
    WiFi.begin(ssid, pass);
    
    display.setCursor(10, 30);
    display.println("Connecting to WiFi...");
    display.display();
    
    int wifiAttempts = 0;
    while (WiFi.status() != WL_CONNECTED && wifiAttempts < 20) {
        delay(500);
        Serial.print(".");
        wifiAttempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        display.setCursor(10, 40);
        display.println("WiFi Connected!");
        display.display();
        Serial.println("WiFi connected");
        
        // Initialize Blynk after WiFi is connected
        Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
    } else {
        display.setCursor(10, 40);
        display.println("WiFi Failed!");
        display.display();
        Serial.println("WiFi connection failed");
    }
    
    delay(2000);
    display.clearDisplay();
    display.display();
}

// Function to send SMS via Notify.lk API
void sendSMS(String message) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        
        // URL encode the message
        String encodedMessage = urlEncode(message);
        
        String url = "https://app.notify.lk/api/v1/send?user_id=" + String(notifylk_user_id) +
                     "&api_key=" + String(notifylk_api_key) +
                     "&sender_id=" + String(notifylk_sender_id) +
                     "&to=" + String(recipient_number) +
                     "&message=" + encodedMessage;

        Serial.println("Sending SMS with URL: " + url);
        
        http.begin(url);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("SMS Sent Successfully, Response Code: " + String(httpResponseCode));
            Serial.println("Response: " + response);
        } else {
            Serial.print("Error sending SMS: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi not connected, unable to send SMS.");
    }
}

// Function to read gas level and convert to PPM
void GASLevel() {
    int analogValue = analogRead(sensor);
    float voltage = analogValue * (3.3 / 4095.0); // Convert ADC value to voltage
    int ppm = 100 * exp(1.6 * voltage); // Convert voltage to PPM using sensor formula

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(10, 10);
    display.print("Gas Level:");
    display.setCursor(10, 35);
    display.print(ppm);
    display.print(" PPM");

    // Alert conditions based on PPM levels
    if (ppm >= 2000) { // Moderate leak
        digitalWrite(buzzer, HIGH);
        display.setCursor(10, 55);
        display.print("DANGER! Gas Leak!");
        
        if (wifiConnected) {
            WidgetLED LED(V1);
            LED.on();  // Turn on LED widget (Red for danger)
        
            if (!smsSent) {  // Send SMS only once
                sendSMS("Gas Leak Detected! PPM level: " + String(ppm) + ". Take immediate action!");
                smsSent = true; // Prevent multiple SMS spam
                Serial.println("SMS alert triggered");
            }
        }
    } 
    else if (ppm >= 1000) { // Slight leakage
        digitalWrite(buzzer, LOW);
        display.setCursor(10, 55);
        display.print("WARNING: Gas Detected!");
        
        if (wifiConnected) {
            WidgetLED LED(V1);
            LED.on();  // Turn on LED widget (Orange for slight danger)
        }
    } 
    else { // Normal condition
        digitalWrite(buzzer, LOW);
        display.setCursor(10, 55);
        display.print("Normal Air");
        
        if (wifiConnected) {
            WidgetLED LED(V1);
            LED.off(); // Turn off LED widget (Green for normal condition)
        }
        
        smsSent = false; // Reset SMS flag when gas levels return to normal
    }

    display.display();
    
    if (wifiConnected) {
        Blynk.virtualWrite(V0, ppm);
    }
    
    Serial.print("PPM: ");
    Serial.println(ppm);
}

void loop() {
    GASLevel();
    
    if (wifiConnected) {
        Blynk.run();
    }
    
    delay(500);
}

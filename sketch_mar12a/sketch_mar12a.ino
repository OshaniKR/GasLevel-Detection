#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
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
char auth[] = "lv1acsTe-o8oA3tp-u9olKE55TcZ9GDm";

// Enter your WiFi SSID and password
char ssid[] = "Galaxy M110896";
char pass[] = "00000000";

// Notify.lk API Credentials
const char* notifylk_user_id = "29282";
const char* notifylk_api_key = "iEFRAqr9ILgi810CWigb";
const char* notifylk_sender_id = "NotifyDEMO";  // Replace with your approved sender ID
const char* recipient_number = "0705762745";  // Replace with the recipient's number

bool smsSent = false; // To prevent multiple SMS spam

WidgetLED gasLED(V1); // Blynk LED Widget

void setup() {
    Serial.begin(115200);
    Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

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
    
    delay(2000);
    display.clearDisplay();
    display.display();
}

// Function to send SMS via Notify.lk API
void sendSMS(String message) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://app.notify.lk/api/v1/send?user_id=" + String(notifylk_user_id) +
                     "&api_key=" + String(notifylk_api_key) +
                     "&sender_id=" + String(notifylk_sender_id) +
                     "&to=" + String(recipient_number) +
                     "&message=" + message;

        http.begin(url);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("SMS Sent Successfully");
            Serial.println(response);
        } else {
            Serial.print("Error sending SMS: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi not connected, unable to send SMS.");
    }
}

// Function to read gas level and update display
void GASLevel() {
    int value = analogRead(sensor);
    int ppm = map(value, 0, 4095, 0, 1000); // Convert to PPM

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(10, 10);
    display.print("GAS Level:");
    display.setCursor(10, 35);
    display.print(ppm);
    display.print(" PPM");

    if (ppm >= 500) {  // Danger level threshold
        digitalWrite(buzzer, HIGH);  // Beep the buzzer
        display.setCursor(10, 55);
        display.print("Warning!");

        gasLED.on();  // Turn LED Red (Blynk defaults to red)
        Blynk.setProperty(V1, "color", "#FF0000"); // Red color for danger

        if (!smsSent) {  // Send SMS only once
            sendSMS("Gas Leak Detected! Take immediate action!");
            smsSent = true; // Prevent multiple SMS spam
        }
    } else {  // Safe and Normal states
        digitalWrite(buzzer, LOW);  // Keep buzzer off
        display.setCursor(10, 55);
        display.print("Safe");

        gasLED.on(); // Turn LED Green
        Blynk.setProperty(V1, "color", "#00FF00"); // Green color for safe

        smsSent = false; // Reset SMS flag if gas level is normal
    }

    display.display();
    Blynk.virtualWrite(V0, ppm);  // Update Blynk app with correct PPM value
    Serial.print("PPM: ");
    Serial.println(ppm);
}

void loop() {
    GASLevel();
    Blynk.run(); // Run the Blynk library
    delay(1000);
}

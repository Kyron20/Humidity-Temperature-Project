#include <ThingSpeak.h>
#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>

// Constants for WiFi and ThingSpeak
#define SSID "" //Wifi username
#define WIFI_PWD "" //wifi password.
#define CHANNEL_ID ""               // ThingSpeak channel ID
#define WRITE_API_KEY "  // ThingSpeak API key

// Constants for Twilio
const char* accountSid = ""; //Twilio account SID. 
const char* authToken = ""; //Twilio account authentication key. 
const char* twilioPhoneNumber = "+"; //Make a twilio account and add the number from there.
const char* destinationPhoneNumber = "+"; // Your own personal number

// Thresholds for temperature and humidity
#define LOW_TEMP_THRESHOLD 20       // Below 20°C
#define HIGH_TEMP_THRESHOLD 25      // Above 25°C
#define LOW_HUMIDITY_THRESHOLD 30   // Below 30% humidity
#define HIGH_HUMIDITY_THRESHOLD 55  // Above 55% humidity

// Prevent constant messages from being sent
unsigned long lastNotificationTime = 0;
const unsigned long notificationInterval = 120000;  // 2 minutes in milliseconds

#define DHTPIN 15      // Pin where the DHT11 is connected (pin 15)
#define DHTTYPE DHT11  // Type of DHT sensor

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;  // Sets up the WiFi client

void setup() {
  Serial.begin(9600);
  dht.begin();
  connectWiFi();
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  // Set lastNotificationTime to a value greater than notificationInterval
  lastNotificationTime = millis() + notificationInterval;
}

void loop() {
  delay(15000);  // 15000ms (15 seconds) wait between measuring.

  float temperature = dht.readTemperature();  // Reads temperature in Celsius.
  float humidity = dht.readHumidity();        // Reads humidity.

  // Check if any reads failed and exit early and then tries again.
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read DHT sensor. Trying again.");
    Serial.println(temperature);
    return;
  }

  // Send data to ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  int response = ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY);
  if (response == 200) {
    Serial.println("Data sent to ThingSpeak.");
  } else {
    Serial.println("Couldn't send data to ThingSpeak");
  }

  unsigned long currentTime = millis();
  //checks if temp is within normal range
  if ((currentTime - lastNotificationTime >= notificationInterval)) {
    if (temperature >= LOW_TEMP_THRESHOLD && temperature <= HIGH_TEMP_THRESHOLD &&
        humidity >= LOW_HUMIDITY_THRESHOLD && humidity <= HIGH_HUMIDITY_THRESHOLD) {
      sendTwilioSMSIdealRange(temperature, humidity);
    } else if (temperature < LOW_TEMP_THRESHOLD || temperature > HIGH_TEMP_THRESHOLD ||
               humidity < LOW_HUMIDITY_THRESHOLD || humidity > HIGH_HUMIDITY_THRESHOLD) {
      sendTwilioSMS(temperature, humidity);
    }
    lastNotificationTime = currentTime;  // Update the last notification time
  }

  // Print temperature and humidity
  Serial.print("Temperature: ");
  Serial.print(temperature);  // Prints temperature.
  Serial.print(" °C\t");
  Serial.print("Humidity: ");
  Serial.print(humidity);  // Prints humidity.
  Serial.println(" %");
}

// Connect to WiFi
void connectWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(SSID, WIFI_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to Wi-Fi!");
}

// Function to send Twilio SMS for normal ranges
void sendTwilioSMSIdealRange(float temperature, float humidity) {
  Serial.println("Sending Twilio SMS for normal range...");

  HTTPClient http;

  String message = "Temperature is " + String(temperature) + "°C and humidity is " + String(humidity) + "%. Both are in the ideal range for a room. Good job!";

  String url = "https://api.twilio.com/2010-04-01/Accounts/";
  url += accountSid;
  url += "/Messages";

  http.begin(url.c_str());
  http.setAuthorization(accountSid, authToken);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String postData = "To=" + String(destinationPhoneNumber) + "&From=" + String(twilioPhoneNumber) + "&Body=" + message;
  int httpCode = http.POST(postData);

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
      Serial.println("Twilio SMS sent for ideal range!");
    } else {
      Serial.println("Failed to send Twilio SMS for ideal range");
    }
  } else {
    Serial.println("Error on HTTP request");
  }
  http.end();
}

// Function to send Twilio SMS
void sendTwilioSMS(float temperature, float humidity) {
  Serial.println("Sending Twilio SMS...");

  HTTPClient http;

  String message;
  if (temperature > HIGH_TEMP_THRESHOLD && humidity > HIGH_HUMIDITY_THRESHOLD) {
    message = "Temperature is " + String(temperature) + "°C and humidity is " + String(humidity) + "%. Both are too high. Try and lower the temperature and humidity. Recommended: Dehumidifier and opening AC (air con).";
  } else if (temperature > HIGH_TEMP_THRESHOLD) {
    message = "Temperature is " + String(temperature) + "°C. It is too high. Try and lower the temperature. Recommended: Open the windows / Buy AC (air con).";
  } else if (humidity > HIGH_HUMIDITY_THRESHOLD) {
    message = "Humidity is " + String(humidity) + "%. It is too high. Try and lower the humidity. Recommended: Dehumidifier / open the windows.";
  } else if (temperature < LOW_TEMP_THRESHOLD && humidity < LOW_HUMIDITY_THRESHOLD) {
    message = "Temperature is " + String(temperature) + "°C and humidity is " + String(humidity) + "%. Both are too low. Try and raise both the temperature and humidity. Recommended: Electric heater and a humidifier.";
  } else if (temperature < LOW_TEMP_THRESHOLD) {
    message = "Temperature is " + String(temperature) + "°C. It is too low. Try and raise the temperature. Recommended: Electric Heater.";
  } else if (humidity < LOW_HUMIDITY_THRESHOLD) {
    message = "Humidity is " + String(humidity) + "%. It is too low. Try and raise the humidity. Recommended: Humidifier.";
  } else {
    Serial.println("No threshold exceeded. ");
    // Handle other cases if needed
  }

  String url = "https://api.twilio.com/2010-04-01/Accounts/";
  url += accountSid;
  url += "/Messages";

  http.begin(url.c_str());
  http.setAuthorization(accountSid, authToken);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String postData = "To=" + String(destinationPhoneNumber) + "&From=" + String(twilioPhoneNumber) + "&Body=" + message;
  int httpCode = http.POST(postData);

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
      Serial.println("Twilio SMS sent!");
    } else {
      Serial.println("Failed to send Twilio SMS");
    }
  } else {
    Serial.println("Error on HTTP request");
  }
  http.end();
}

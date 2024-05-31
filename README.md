# Temperature and Humidity Tracker

This project tracks temperature and humidity using an Arduino, DHT11 sensor, and the ThingSpeak and Twilio APIs. It periodically measures and uploads data to ThingSpeak, and sends SMS notifications via Twilio when the readings are outside of the specified ranges.

## Features

- **Real-time Temperature and Humidity Monitoring**: Reads temperature and humidity data using a DHT11 sensor.
- **Data Upload**: Sends sensor data to ThingSpeak for storage and visualization.
- **SMS Notifications**: Sends SMS alerts using Twilio when temperature or humidity readings exceed predefined thresholds.
- **Interactive Web Interface**: Displays data in real-time graphs and color-coded text indicating the current status.

## Components Used

- **Arduino Board**
- **DHT11 Sensor**
- **WiFi Module**
- **ThingSpeak API**
- **Twilio API**

## Thresholds for Alerts

- **Temperature**: 
  - Low: Below 20°C
  - High: Above 25°C
- **Humidity**: 
  - Low: Below 30%
  - High: Above 55%

## Setup Instructions

1. **Hardware Setup**:
   - Connect the DHT11 sensor to pin 15 of the Arduino board.

2. **Software Configuration**:
   - Install the necessary libraries: `ThingSpeak`, `WiFi`, `DHT`, and `HTTPClient`.
   - Update the following constants in the code with your details:
     - `SSID`: Your WiFi SSID.
     - `WIFI_PWD`: Your WiFi password.
     - `CHANNEL_ID`: Your ThingSpeak channel ID.
     - `WRITE_API_KEY`: Your ThingSpeak API key.
     - `accountSid`: Your Twilio account SID.
     - `authToken`: Your Twilio authentication token.
     - `twilioPhoneNumber`: Your Twilio phone number.
     - `destinationPhoneNumber`: Your personal phone number.

3. **Upload Code**:
   - Upload the provided code to your Arduino board.

4. **Run the Web Interface**:
   - Open the HTML file in a web browser to view the real-time temperature and humidity data.

## Usage

- The system measures temperature and humidity every 15 seconds.
- Data is sent to ThingSpeak, and SMS notifications are sent if readings are outside the ideal range.
- The web interface displays real-time data with color-coded indicators and alerts for abnormal readings.

## Web Interface

The web interface includes:

- **Graphs**: Line charts displaying temperature and humidity trends.
- **Current Values**: Real-time display of temperature and humidity with color-coded text.
- **Alerts**: Alerts pop up when temperature or humidity exceeds the thresholds.

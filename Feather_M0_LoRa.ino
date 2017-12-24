// Include necessary libraries
#include <Arduino.h>
#include <elapsedMillis.h>
#include <avr/pgmspace.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "dtostrf.h"

/**
 * How often should data be sent?
 */
#define UpdateInterval 30    // Update Every 30 mins

/**
 * Startup message to send
 */
#define STARTUP_MESSAGE "Hello World!"

/**
 * Sensors to enable
 * Uncomment the sensors you want to use
 */
#define SENSOR_FEATHER_BATTERY  // Battery Voltage
#define SENSOR_FEATHER_MEMORY   // Free memory
//#define SENSOR_DHT11            // DHT 11
//#define SENSOR_DHT21            // DHT 21 (AM2301)
//#define SENSOR_DHT22            // DHT 22 (AM2302)
//#define SENSOR_BMP085           // BMP085
//#define SENSOR_BMP180           // BMP180
//#define SENSOR_BMP280           // BMP280
//#define SENSOR_BME280           // BME280
//#define SENSOR_TSL2561          // TLS2561
//#define SENSOR_BH1750           // BH1750FVI
//#define SENSOR_DS18B20          // DS18B20
//#define SENSOR_SR04PLUS         // HC-SR04+

// LoRaWAN Config
// Device Address
devaddr_t DevAddr = 0x00000000;

// Network Session Key
unsigned char NwkSkey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Application Session Key
unsigned char AppSkey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


/*****
 * --- Nothing needs to be changed below here ---
 *****/
// Feather M0 RFM9x pin mappings
lmic_pinmap pins = {
  .nss = 8,                // Internal connected
  .rxen = 0,               // Not used for RFM92/RFM95
  .txen = 0,               // Not used for RFM92/RFM95
  .rst = 4,                // Internal connected
  .dio = {3, 5, 6},    // Connect "i01" to "5"
                    // Connect "D2" to "6"
};

// Track if the current message has finished sending
bool dataSent = false;

/**
 * DHT Humidity/Temperature sensor
 */
#if defined(SENSOR_DHT11) || defined(SENSOR_DHT22) || defined(SENSOR_DHT21)
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#endif

#if defined(SENSOR_DHT11)
    DHT_Unified dht(9, DHT11);
#elif defined(SENSOR_DHT21)
    DHT_Unified dht(9, DHT21);
#elif defined(SENSOR_DHT22)
    DHT_Unified dht(9, DHT22);
#endif


/**
 * BMP085/BMP180
 */
#if defined(SENSOR_BMP085) || defined(SENSOR_BMP180)
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
#endif
#if defined(SENSOR_BMP280)
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp280;
#endif
#if defined(SENSOR_BME280)
#include <Adafruit_BME280.h>
Adafruit_BME280 bme280;
#endif

/**
 * TLS2561
 */
#if defined(SENSOR_TSL2561)
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
#endif

/**
 * BH1750
 */
#if defined(SENSOR_BH1750)
#include <Wire.h>
#define _delay_ms(ms) delayMicroseconds((ms) * 1000)
#include <BH1750.h>
BH1750 bh1750;
#endif

/**
 * DS18B20 temperature sensor
 */
#if defined(SENSOR_DS18B20)
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

/**
 * HC-SR04+ ultrasonic distance sensor
 */
#if defined(SENSOR_SR04PLUS)
#include <NewPing.h>
#define TRIGGER_PIN  12  // pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
#endif

/**
 * Device Start Up
 */
void setup() {
    // Startup delay for Serial interface to be ready
    Serial.begin(115200);
    delay(3000);

    // Debug message
    Serial.println("Starting...");

    // Some sensors require a delay on startup
    elapsedMillis sinceStart = 0;
    int sensorReady = 0;

    #if defined(SENSOR_DHT11) || defined(SENSOR_DHT22) || defined(SENSOR_DHT21)
        // Initialize the DHT sensor.
        dht.begin();
        sensor_t sensor;
        dht.temperature().getSensor(&sensor);
        dht.humidity().getSensor(&sensor);
        sensorReady = max(sensorReady, (sensor.min_delay / 1000) + sinceStart);
    #endif

    #if defined(SENSOR_BMP085) || defined(SENSOR_BMP180)
        // Initialize the BMP pressure sensor
        bmp.begin();
    #endif
    #if defined(SENSOR_BMP280)
        // Initialize the BMP280 pressure sensor
        bmp280.begin();
    #endif
    #if defined(SENSOR_BME280)
        // Initialize the BME280 pressure sensor
        bme280.begin();
    #endif

    #if defined(SENSOR_TSL2561)
        // Initialize the TSL2561 Digital Luminosity sensor
        tsl.begin();
        tsl.enableAutoRange(true);
        tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
    #endif

    #if defined(SENSOR_BH1750)
        bh1750.begin(BH1750_CONTINUOUS_HIGH_RES_MODE_2);
        sensorReady = max(sensorReady, 1000 + sinceStart);
    #endif

    // Setup LoRaWAN state
    initLoRaWAN();

    // Wait for all the sensors to be ready
    if (sensorReady > sinceStart) {
        // A sensor still needs some time
        delay(max(0, (int)(sensorReady - sinceStart)));
    }

    #if defined(STARTUP_MESSAGE)
        // Send Startup Message
        sendStartupMessage();
        delay(1000);
    #endif

    // Shutdown the radio
    os_radio(RADIO_RST);

    // Debug message
    Serial.println("Startup Complete");
}

void initLoRaWAN() {
    // LMIC init
    os_init();

    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    // by joining the network, precomputed session parameters are be provided.
    LMIC_setSession(0x1, DevAddr, (uint8_t*)NwkSkey, (uint8_t*)AppSkey);

    // Enabled data rate adaptation
    LMIC_setAdrMode(1);

    // Enable link check validation
    LMIC_setLinkCheckMode(0);

    // Set data rate and transmit power
    LMIC_setDrTxpow(DR_SF12, 21);
}

void loop() {
    // Start timing how long since starting to send data
    elapsedMillis sinceWake = 0;

    // Debug message
    Serial.println("\nBeginning to send data");

    // Send Battery Voltage
    #if defined(SENSOR_FEATHER_BATTERY)
        sendBattery();
        delay(1000);
    #endif

    // Send Free Memory
    #if defined(SENSOR_FEATHER_MEMORY)
        sendFreeMemory();
        delay(1000);
    #endif

    // Send DHT
    #if defined(SENSOR_DHT11) || defined(SENSOR_DHT22) || defined(SENSOR_DHT21)
        sendDHT();
        delay(1000);
    #endif

    // Send Barmometric Pressure
    #if defined(SENSOR_BMP085) || defined(SENSOR_BMP180)
        sendBMP();
        delay(1000);
    #endif
    #if defined(SENSOR_BMP280)
        sendBMP280();
        delay(1000);
    #endif
    #if defined(SENSOR_BME280)
        sendBME280();
        delay(1000);
    #endif

    // Send Light Intensity
    #if defined(SENSOR_TSL2561) || defined(SENSOR_BH1750)
        sendLux();
        delay(1000);
    #endif

    // Send Temperature
    #if defined(SENSOR_DS18B20)
        sendTemperature();
        delay(1000);
    #endif

    // Send Distance in centimeters
    #if defined(SENSOR_SR04PLUS)
        sendDistance();
        delay(1000);
    #endif
    
    // Shutdown the radio
    os_radio(RADIO_RST);

    // Output sleep time
    int sleepSeconds = 60 * UpdateInterval;
    sleepSeconds -= sinceWake/1000;
    Serial.print("Sleeping for ");
    Serial.print(sleepSeconds);
    Serial.println(" seconds");
    delay(500); // time for Serial send buffer to clear

    // Actually go to sleep
    signed long sleep = 60000 * UpdateInterval;
    sleep -= sinceWake;
    delay(constrain(sleep, 10000, 60000 * UpdateInterval));
}

/**
 * Send a message with the distance
 */
#if defined(SENSOR_SR04PLUS)
void sendDistance() {
    // Ensure there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
        // Something already in the queque
        return;
    }

    // Get the distance
    char packet[40] = "";
    strcat(packet, "Distance: ");
    strcat(packet, sonar.ping_cm());


    // Debug message
    Serial.print("  seqno ");
    Serial.print(LMIC.seqnoUp);
    Serial.print(": ");
    Serial.println(packet);

    // Add to the queque
    dataSent = false;
    uint8_t lmic_packet[40];
    strcpy((char *)lmic_packet, packet);
    LMIC_setTxData2(1, lmic_packet, strlen((char *)lmic_packet), 0);

    // Wait for the data to send or timeout after 15s
    elapsedMillis sinceSend = 0;
    while (!dataSent && sinceSend < 15000) {
        os_runloop_once();
        delay(1);
    }
    os_runloop_once();
}
#endif

/**
 * Send a message with the light intensity
 */
#if defined(SENSOR_TSL2561) || defined(SENSOR_BH1750)
void sendLux() {
    // Ensure there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
        // Something already in the queque
        return;
    }

    // Put together the data to send
    char packet[40] = "";

    #if defined(SENSOR_TSL2561)
    // Get sensor event and print its value.
    sensors_event_t event;
    tsl.getEvent(&event);
    if (event.light) {
        char floatStr[10];
        dtostrf(event.light, 3, 2, floatStr);
        strcat(packet, "Light: ");
        strcat(packet, floatStr);
            #if defined(SENSOR_BH1750)
                strcat(packet, " lux\n");
            #else
        strcat(packet, " lux");
            #endif
    } else {
        Serial.println("Error reading light intensity!");
    }
    #endif

    #if defined(SENSOR_BH1750)
        // Get the BH1750 value
        uint16_t lux = bh1750.readLightLevel();
        char floatStr[10];
        dtostrf((float)lux, 3, 2, floatStr);
        strcat(packet, "Light: ");
        strcat(packet, floatStr);
        strcat(packet, " lux");
    #endif

    if (!strlen(packet)) {
        // Don't send empty packet
        return;
    }

    // Debug message
    Serial.print("  seqno ");
    Serial.print(LMIC.seqnoUp);
    Serial.print(": ");
    Serial.println(packet);

    // Add to the queque
    dataSent = false;
    uint8_t lmic_packet[40];
    strcpy((char *)lmic_packet, packet);
    LMIC_setTxData2(1, lmic_packet, strlen((char *)lmic_packet), 0);

    // Wait for the data to send or timeout after 15s
    elapsedMillis sinceSend = 0;
    while (!dataSent && sinceSend < 15000) {
        os_runloop_once();
        delay(1);
    }
    os_runloop_once();
}
#endif

/**
 * Send a message with the temperature
 */
#if defined(SENSOR_DS18B20)
void sendTemperature() {
    // Ensure there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
        // Something already in the queque
        return;
    }

    // Put together the data to send
    char packet[40] = "Temperature: ";

    // Setup the one wire connection on pin 10
    OneWire oneWire(10);
    DallasTemperature sensors(&oneWire);
    DeviceAddress thermometer;

    // Get the temp
    sensors.begin();
    sensors.getAddress(thermometer, 0);
    sensors.setResolution(thermometer, 12);
    sensors.requestTemperatures();
    float temp = sensors.getTempC(thermometer);
    char floatStr[10];
    dtostrf(temp, 3, 2, floatStr);
    strcat(packet, floatStr);
    strcat(packet, "*C");

    // Debug message
    Serial.print("  seqno ");
    Serial.print(LMIC.seqnoUp);
    Serial.print(": ");
    Serial.println(packet);

    // Add to the queque
    dataSent = false;
    uint8_t lmic_packet[40];
    strcpy((char *)lmic_packet, packet);
    LMIC_setTxData2(1, lmic_packet, strlen((char *)lmic_packet), 0);

    // Wait for the data to send or timeout after 15s
    elapsedMillis sinceSend = 0;
    while (!dataSent && sinceSend < 15000) {
        os_runloop_once();
        delay(1);
    }
    os_runloop_once();
}
#endif

/**
 * Send a message with the temperature and pressure
 */
#if defined(SENSOR_BMP085) || defined(SENSOR_BMP180)
void sendBMP() {
    // Ensure there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
        // Something already in the queque
        return;
    }

    // Put together the data to send
    char packet[40] = "";

    // Get temperature event and print its value.
    float temperature;
    sensors_event_t event;
    bmp.getEvent(&event);
    bmp.getTemperature(&temperature);
    if (temperature) {
        char floatStr[10];
        dtostrf(temperature, 3, 2, floatStr);
        strcat(packet, "Temp: ");
        strcat(packet, floatStr);
        strcat(packet, "*C\n");
    } else {
        Serial.println("Error reading temperature!");
    }

    // Get pressure event and print its value.
    if (event.pressure) {
        char floatStr[10];
        dtostrf(event.pressure, 3, 2, floatStr);
        strcat(packet, "Pressure: ");
        strcat(packet, floatStr);
        strcat(packet, "hPa");
    } else {
        Serial.println("Error reading pressure!");
    }

    if (!strlen(packet)) {
        // Don't send empty packet
        return;
    }

    // Debug message
    Serial.print("  seqno ");
    Serial.print(LMIC.seqnoUp);
    Serial.print(": ");
    Serial.println(packet);

    // Add to the queque
    dataSent = false;
    uint8_t lmic_packet[40];
    strcpy((char *)lmic_packet, packet);
    LMIC_setTxData2(1, lmic_packet, strlen((char *)lmic_packet), 0);

    // Wait for the data to send or timeout after 15s
    elapsedMillis sinceSend = 0;
    while (!dataSent && sinceSend < 15000) {
        os_runloop_once();
        delay(1);
    }
    os_runloop_once();
}
#endif
#if defined(SENSOR_BMP280)
void sendBMP280() {
    // Ensure there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
        // Something already in the queque
        return;
    }

    // Put together the data to send
    char packet[40] = "";

    // Get temperature event and print its value.
    float temperature = bmp280.readTemperature();
    if (temperature) {
        char floatStr[10];
        dtostrf(temperature, 3, 2, floatStr);
        strcat(packet, "Temp: ");
        strcat(packet, floatStr);
        strcat(packet, "*C\n");
    } else {
        Serial.println("Error reading temperature!");
    }

    // Get pressure event and print its value.
    float pressure = bmp280.readPressure() / 100.0F;
    if (pressure) {
        char floatStr[10];
        dtostrf(pressure, 3, 2, floatStr);
        strcat(packet, "Pressure: ");
        strcat(packet, floatStr);
        strcat(packet, "hPa");
    } else {
        Serial.println("Error reading pressure!");
    }

    if (!strlen(packet)) {
        // Don't send empty packet
        return;
    }

    // Debug message
    Serial.print("  seqno ");
    Serial.print(LMIC.seqnoUp);
    Serial.print(": ");
    Serial.println(packet);
    // Add to the queque
    dataSent = false;
    uint8_t lmic_packet[40];
    strcpy((char *)lmic_packet, packet);
    LMIC_setTxData2(1, lmic_packet, strlen((char *)lmic_packet), 0);

    // Wait for the data to send or timeout after 15s
    elapsedMillis sinceSend = 0;
    while (!dataSent && sinceSend < 15000) {
        os_runloop_once();
        delay(1);
    }
    os_runloop_once();
}
#endif
#if defined(SENSOR_BME280)
void sendBME280() {
    // Ensure there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
        // Something already in the queque
        return;
    }

    // Put together the data to send
    char packet[40] = "";

    // Get temperature event and print its value.
    float temperature = bme280.readTemperature();
    if (temperature) {
        char floatStr[10];
        dtostrf(temperature, 3, 2, floatStr);
        strcat(packet, "Temp: ");
        strcat(packet, floatStr);
        strcat(packet, "*C\n");
    } else {
        Serial.println("Error reading temperature!");
    }

    // Get pressure event and print its value.
    float pressure = bme280.readPressure() / 100.0F;
    if (pressure) {
        char floatStr[10];
        dtostrf(pressure, 3, 2, floatStr);
        strcat(packet, "Pressure: ");
        strcat(packet, floatStr);
        strcat(packet, "hPa\n");
    } else {
        Serial.println("Error reading pressure!");
    }

    // Get humidity and print its value.
    float humidity = bme280.readHumidity();
    if (humidity) {
        char floatStr[10];
        dtostrf(pressure, 3, 2, floatStr);
        strcat(packet, "Humidity: ");
        strcat(packet, floatStr);
        strcat(packet, "%");
    } else {
        Serial.println("Error reading humidity!");
    }

    if (!strlen(packet)) {
        // Don't send empty packet
        return;
    }

    // Debug message
    Serial.print("  seqno ");
    Serial.print(LMIC.seqnoUp);
    Serial.print(": ");
    Serial.println(packet);
    // Add to the queque
    dataSent = false;
    uint8_t lmic_packet[40];
    strcpy((char *)lmic_packet, packet);
    LMIC_setTxData2(1, lmic_packet, strlen((char *)lmic_packet), 0);

    // Wait for the data to send or timeout after 15s
    elapsedMillis sinceSend = 0;
    while (!dataSent && sinceSend < 15000) {
        os_runloop_once();
        delay(1);
    }
    os_runloop_once();
}
#endif

/**
 * Send a message with the temperature and humidity
 */
#if defined(SENSOR_DHT11) || defined(SENSOR_DHT22) || defined(SENSOR_DHT21)
void sendDHT() {
    // Ensure there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
        // Something already in the queque
        return;
    }

    // Put together the data to send
    char packet[30] = "";

    // Get temperature event and print its value.
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (!isnan(event.temperature)) {
        char floatStr[10];
        dtostrf(event.temperature, 3, 2, floatStr);
        strcat(packet, "Temp: ");
        strcat(packet, floatStr);
        strcat(packet, "*C\n");
    } else {
        Serial.println("Error reading temperature!");
    }

    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (!isnan(event.relative_humidity)) {
        char floatStr[10];
        dtostrf(event.relative_humidity, 3, 2, floatStr);
        strcat(packet, "Humidity: ");
        strcat(packet, floatStr);
        strcat(packet, "%");
    } else {
        Serial.println("Error reading humidity!");
    }

    if (!strlen(packet)) {
        // Don't send empty packet
        return;
    }

    // Debug message
    Serial.print("  seqno ");
    Serial.print(LMIC.seqnoUp);
    Serial.print(": ");
    Serial.println(packet);

    // Add to the queque
    dataSent = false;
    uint8_t lmic_packet[30];
    strcpy((char *)lmic_packet, packet);
    LMIC_setTxData2(1, lmic_packet, strlen((char *)lmic_packet), 0);

    // Wait for the data to send or timeout after 15s
    elapsedMillis sinceSend = 0;
    while (!dataSent && sinceSend < 15000) {
        os_runloop_once();
        delay(1);
    }
    os_runloop_once();
}
#endif

/**
 * Send a message with the free memory
 */
#if defined(SENSOR_FEATHER_MEMORY)
extern "C" char *sbrk(int i);
void sendFreeMemory() {
    // Ensure there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
        // Something already in the queque
        return;
    }

    // Get the free memory
    // https://learn.adafruit.com/adafruit-feather-m0-basic-proto/adapting-sketches-to-m0#how-much-ram-available
    char stack_dummy = 0;
    int freeMem = &stack_dummy - sbrk(0);

    // Convert to a string
    char intStr[10];
    itoa(freeMem, intStr, 10);

    // Put together the data to send
    char packet[25] = "Free Memory: ";
    strcat(packet, intStr);

    // Debug message
    Serial.print("  seqno ");
    Serial.print(LMIC.seqnoUp);
    Serial.print(": ");
    Serial.println(packet);

    // Add to the queque
    dataSent = false;
    uint8_t lmic_packet[25];
    strcpy((char *)lmic_packet, packet);
    LMIC_setTxData2(1, lmic_packet, strlen((char *)lmic_packet), 0);

    // Wait for the data to send or timeout after 15s
    elapsedMillis sinceSend = 0;
    while (!dataSent && sinceSend < 15000) {
        os_runloop_once();
        delay(1);
    }
    os_runloop_once();
}
#endif

/**
 * Send a message with the battery voltage
 */
#if defined(SENSOR_FEATHER_BATTERY)
void sendBattery() {
    // Ensure there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
        // Something already in the queque
        return;
    }

    // Get the battery voltage
    float measuredvbat = analogRead(A7); // Hard wired to pin A7 on Adafruit Feather
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage

    // Convert to a string
    char floatStr[10];
    dtostrf(measuredvbat, 3, 2, floatStr);

    // Put together the data to send
    char packet[20] = "Battery: ";
    strcat(packet, floatStr);

    // Debug message
    Serial.print("  seqno ");
    Serial.print(LMIC.seqnoUp);
    Serial.print(": ");
    Serial.println(packet);

    // Add to the queque
    dataSent = false;
    uint8_t lmic_packet[20];
    strcpy((char *)lmic_packet, packet);
    LMIC_setTxData2(1, lmic_packet, strlen((char *)lmic_packet), 0);

    // Wait for the data to send or timeout after 15s
    elapsedMillis sinceSend = 0;
    while (!dataSent && sinceSend < 15000) {
        os_runloop_once();
        delay(1);
    }
    os_runloop_once();
}
#endif

/**
 * Send a startup message
 */
#if defined(STARTUP_MESSAGE)
void sendStartupMessage() {
    // Ensure there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
        // Something already in the queque
        return;
    }

    // Put together the data to send
    char packet[41] = STARTUP_MESSAGE;

    // Debug message
    Serial.print("  seqno ");
    Serial.print(LMIC.seqnoUp);
    Serial.print(": ");
    Serial.println(packet);

    // Add to the queque
    dataSent = false;
    uint8_t lmic_packet[41];
    strcpy((char *)lmic_packet, packet);
    LMIC_setTxData2(1, lmic_packet, strlen((char *)lmic_packet), 0);

    // Wait for the data to send or timeout after 15s
    elapsedMillis sinceSend = 0;
    while (!dataSent && sinceSend < 15000) {
        os_runloop_once();
        delay(1);
    }
    os_runloop_once();
}
#endif

// ----------------------------------------------------------------------------
// LMIC CALLBACKS
// ----------------------------------------------------------------------------

// LoRaWAN Application identifier (AppEUI)
static const u1_t AppEui[8] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/**
 * Call back to get the AppEUI
 */
void os_getArtEui (u1_t* buf) {
    memcpy(buf, AppEui, 8);
}

/**
 * Call back to get the Network Session Key
 */
void os_getDevKey (u1_t* buf) {
    memcpy(buf, NwkSkey, 16);
}

/**
 * Callback after a LMIC event
 */
void onEvent (ev_t ev) {
    if (ev == EV_TXCOMPLETE) {
        dataSent = true;
    }
    if (ev == EV_LINK_DEAD) {
        initLoRaWAN();
    }
}

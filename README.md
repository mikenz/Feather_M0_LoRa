# Feather M0 LoRa Node
Example Arduino code of using an Adafruit Feather M0 LoRa module to send sensor data.

This code has been tested with KotahiNet in New Zealand. It should be able to be used on any LoraWAN network with a little [modification to the frequency plan](https://github.com/mikenz/LoRa-LMIC-1.51/blob/master/src/lmic/config.h#L5).

## Things you need

### Hardware

- [Adafruit Feather M0 with RFM95 LoRa Radio - 900MHz](https://www.adafruit.com/products/3178).
- (optional) BMP085/BMP180 Barometric pressure sensor
- (optional) DHT11/DHT21/DHT22 Humity and Temperature sensor

### Software

- [Download and Install the Arduino IDE](https://www.arduino.cc/en/Main/Software)
- [Import the Adafruit boards](https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module/setup)
- Install the [elapsedMillis](https://github.com/pfeerick/elapsedMillis) Library ([Adafruit All About Arduino Libraries](https://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use))
- Install my fork of the [LoRa-LMIC-1.51](https://github.com/mikenz/LoRa-LMIC-1.51) Library

## Sending your first message

### Connecting it up

To send the most basic "Hello World!" message you need to add two connections on the Feather board, and add an antenna. A straight 87mm long wire makes a perfect antenna to get started. Below are the connections needed:

![Minimum wiring](https://github.com/mikenz/Feather_M0_LoRa/raw/master/fritzing/Hello%20World_bb.png)

### Configuring the code

Open Feather_M0_LoRa.ino in the Arduino IDE and find the LoRaWAN Config section in the code.

Enter the Device Address, Network Session Key, and Application Session Key you recieved from [KotahiNet](http://kotahi.net/connect/).

eg If you recieved
```
Device address: 01234567
Network Session Key: 0123456789ABCDEF0123456789ABCDEF
Application Session Key: 0123456789ABCDEF0123456789ABCDEF
```

You would enter it as
```Arduino
// LoRaWAN Config
// Device Address
devaddr_t DevAddr = 0x01234567;

// Network Session Key
unsigned char NwkSkey[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };

// Application Session Key
unsigned char AppSkey[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
```

Then click the Upload button in the Arduino IDE to compile the code and send it to your Feather. It will upload the code and it will start running. If you've configured the code correctly and you're in range of a KotahiNet receiver then you will have sucessfully sent your first messages.

### Changing the startup message

To change the startup message, edit the `STARTUP_MESSAGE` value near the top of the code. This must be LESS THAN 40 CHARACTERS.

```Arduino
/**
 * Startup message to send
 */
#define STARTUP_MESSAGE "Hello World!"
```

Then Upload the code to the Feather again.

## Adding a sensor

### Barometric Pressure

#### Hardware

Easiest is to get a BMP085/BMP180 breakbout board
- [Adafruit BMP180 Barometric Pressure/Temperature/Altitude Sensor](https://www.adafruit.com/products/1603)
- [SparkFun Barometric Pressure Sensor Breakout](https://www.sparkfun.com/products/11824)

And connect it up
![BMP085/BMP180 wiring](https://github.com/mikenz/Feather_M0_LoRa/raw/master/fritzing/BMP085-BMP180_bb.png)

#### Software

- Install the [Adafruit Unified Sensor Library](https://github.com/adafruit/Adafruit_Sensor) library
- Install the [Adafruit_BMP085_Unified](https://github.com/adafruit/Adafruit_BMP085_Unified) library

Remove the comments infront of the sensor you have, eg

```Arduino
//#define SENSOR_BMP085             // BMP085
#define SENSOR_BMP180             // BMP180
```

Then Upload the code to the Feather again.

### Humidity and Temperature

#### Hardware

- DHT11 or DHT22 [[Adafruit](https://www.adafruit.com/products/385)] [[Sparkfun](https://www.sparkfun.com/products/10167)]
- 4.7K - 10K resistor

And connect it up
![DHT11/DHT22 wiring](https://github.com/mikenz/Feather_M0_LoRa/raw/master/fritzing/DHT11-DHT22_bb.png)

#### Software

- Install the [Adafruit Unified Sensor Library](https://github.com/adafruit/Adafruit_Sensor) library
- Install the [Adafruit DHT Humidity & Temperature Unified Sensor](https://github.com/adafruit/Adafruit_DHT_Unified) Library
- Install the [Adafruit DHT Sensor](https://github.com/adafruit/DHT-sensor-library) Library

Remove the comments infront of the sensor you have, eg

```Arduino
//#define SENSOR_DHT11            // DHT 11
//#define SENSOR_DHT21            // DHT 21 (AM2301)
#define SENSOR_DHT22            // DHT 22 (AM2302)
```

Then Upload the code to the Feather again.


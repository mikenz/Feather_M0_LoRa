# Feather M0 LoRa Node
Example Arduino code of using an Adafruit Feather M0 LoRa module to send sensor data.

This code has been tested with KotahiNet in New Zealand. It should be able to be used on any LoraWAN network with a little [modification to the frequency plan](https://github.com/mikenz/LoRa-LMIC-1.51/blob/master/src/lmic/config.h#L5).

Where possible I have included links to New Zealand suppliers of components. My go to is [nicegear](https://nicegear.co.nz/), followed by  [MindKits](http://www.mindkits.co.nz/). Jaycar Electronics have a range of sensors but they're more expensive.

## Things you need

### Hardware

- [Adafruit Feather M0 with RFM95 LoRa Radio - 900MHz](https://www.adafruit.com/products/3178) [[nicegear](https://nicegear.co.nz/arduino/adafruit-feather-m0-with-rfm95-lora-radio-900mhz/)]
- (optional) BMP085/BMP180/BMP280/BME280 Barometric pressure sensor
- (optional) DHT11/DHT21/DHT22 Humidity and Temperature sensor
- (optional) TSL2561 Light Intensity sensor
- (optional) BH1750FVI Light Intensity sensor
- (optional) HC-SR04+ Ultrasonic Distance sensor

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

Easiest is to get a BMP085/BMP180/BMP280/BME280 breakbout board
- [Adafruit BME280 I2C or SPI Temperature Humidity Pressure Sensor](https://www.adafruit.com/product/2652) [[nicegear](https://nicegear.co.nz/sensors/adafruit-bme280-i2c-or-spi-temperature-humidity-pressure-sensor/)]
- [Adafruit BMP280 I2C or SPI Barometric Pressure & Altitude Sensor](https://www.adafruit.com/products/2651) [[nicegear](https://nicegear.co.nz/sensors/adafruit-bmp280-i2c-or-spi-barometric-pressure-altitude-sensor/)]
- [Adafruit BMP180 Barometric Pressure/Temperature/Altitude Sensor](https://www.adafruit.com/products/1603)
- or [SparkFun Barometric Pressure Sensor Breakout](https://www.sparkfun.com/products/11824) [[nicegear](https://nicegear.co.nz/sensors/barometric-pressure-sensor-bmp180-breakout/)]
- or Generic breakout [[Mindkits](http://www.mindkits.co.nz/store/p/9195-BMP180-Barometer-Pressure/Temperature/Altitude-Sensor.aspx)]

And connect it up
![BMP085/BMP180 wiring](https://github.com/mikenz/Feather_M0_LoRa/raw/master/fritzing/BMP085-BMP180_bb.png)

#### Software

- Install the [Adafruit Unified Sensor Library](https://github.com/adafruit/Adafruit_Sensor) library
- Install the [Adafruit_BME280](https://github.com/adafruit/Adafruit_BME280_Library) library
- Install the [Adafruit_BMP280](https://github.com/adafruit/Adafruit_BMP280_Library) library
- Install the [Adafruit_BMP085_Unified](https://github.com/adafruit/Adafruit_BMP085_Unified) library

Remove the comments in front of the sensor you have, eg

```Arduino
//#define SENSOR_BMP085           // BMP085
//#define SENSOR_BMP180           // BMP180
//#define SENSOR_BMP280           // BMP280
#define SENSOR_BME280           // BME280
```

Then Upload the code to the Feather again.

### Humidity and Temperature

#### Hardware

- DHT11 or DHT22 [[Adafruit](https://www.adafruit.com/products/385)] [[Sparkfun](https://www.sparkfun.com/products/10167)] [[nicegear](https://nicegear.co.nz/sensors/humidity-and-temperature-sensor-dht22/)] [[MindKits](http://www.mindkits.co.nz/store/p/8352-Humidity-and-Temperature-Sensor-DHT22.aspx)]
- and 4.7K - 10K resistor

Or a breakout board that has the resistor included
- [Freetronics Humidity / Temperature Sensor Module](http://www.freetronics.com.au/products/humidity-and-temperature-sensor-module) [[nicegear](https://nicegear.co.nz/sensors/freetronics-humidity-and-temperature-sensor-module/)]

And connect it up
![DHT11/DHT22 wiring](https://github.com/mikenz/Feather_M0_LoRa/raw/master/fritzing/DHT11-DHT22_bb.png)

#### Software

- Install the [Adafruit Unified Sensor Library](https://github.com/adafruit/Adafruit_Sensor) library
- Install the [Adafruit DHT Humidity & Temperature Unified Sensor](https://github.com/adafruit/Adafruit_DHT_Unified) Library
- Install the [Adafruit DHT Sensor](https://github.com/adafruit/DHT-sensor-library) Library

Remove the comments in front of the sensor you have, eg

```Arduino
//#define SENSOR_DHT11            // DHT 11
//#define SENSOR_DHT21            // DHT 21 (AM2301)
#define SENSOR_DHT22            // DHT 22 (AM2302)
```

Then Upload the code to the Feather again.


### Light Intensity (TSL2561 sensor)

#### Hardware

- [Adafruit TSL2561 Digital Luminosity/Lux/Light Sensor Breakout](https://www.adafruit.com/product/439)
- or [SparkFun Luminosity Sensor Breakout - TSL2561
](https://www.sparkfun.com/products/12055) [[nicegear](https://nicegear.co.nz/sensors/tsl2561-luminosity-sensor-breakout/)]
[[MindKits](http://www.mindkits.co.nz/store/p/8421-TSL2561-Luminosity-Sensor-Breakout.aspx)]
- or Generic breakout [[MindKits](http://www.mindkits.co.nz/store/p/9263-Luminosity-Sensor-TSL2561-Breakout.aspx)]

And connect it up
![TLS2561  wiring](https://github.com/mikenz/Feather_M0_LoRa/raw/master/fritzing/TLS2561_bb.png)

#### Software

- Install the [Adafruit Unified Sensor Library](https://github.com/adafruit/Adafruit_Sensor) library
- Install the [Adafruit TSL2561 Light Sensor Driver](https://github.com/adafruit/Adafruit_TSL2561) Library

Remove the comments in front of the sensor you have, eg

```Arduino
#define SENSOR_TSL2561          // TLS2561
```

Then Upload the code to the Feather again.


### Temperature (DS18B20 sensor)

#### Hardware

- DS18B20 [[Adafruit](https://www.adafruit.com/products/374)]
[[SparkFun](https://www.sparkfun.com/products/245)] [[nicegear](https://nicegear.co.nz/sensors/one-wire-digital-temperature-sensor-ds18b20/)] [[MindKits](http://www.mindkits.co.nz/store/p/9303-DS18B20-Temperature-Sensor.aspx)]
- Waterproof DS18B20 Digital temperature sensor [[Adafruit](https://www.adafruit.com/products/381)] [[SparkFun](https://www.sparkfun.com/products/11050)] [[nicegear](https://nicegear.co.nz/sensors/temperature-sensor-waterproof-ds18b20/)] [[MindKits](http://www.mindkits.co.nz/store/p/9156-One-Wire-Waterproof-Temperature-Sensor.aspx)]
- [Adafruit High Temp Waterproof DS18B20 Digital temperature sensor](https://www.adafruit.com/products/642) [[nicegear](https://nicegear.co.nz/sensors/high-temp-waterproof-ds18b20-digital-temperature-sensor/)]
- and 4.7k Resistor

Or a breakout board that has the resistor included
- [Freetronics Temperature Sensor Module](http://www.freetronics.com.au/products/temperature-sensor-module) [[nicegear](https://nicegear.co.nz/sensors/freetronics-temperature-sensor-module/)]

And connect it up
![DS18B20  wiring](https://github.com/mikenz/Feather_M0_LoRa/raw/master/fritzing/DS18B20_bb.png)


#### Software

- Install the [OneWire](https://github.com/PaulStoffregen/OneWire) library
- Install the [Arduino Temperature Control](https://github.com/milesburton/Arduino-Temperature-Control-Library) library

Remove the comments in front of the sensor you have, eg

```Arduino
#define SENSOR_DS18B20         // DS18B20
```

Then Upload the code to the Feather again.


### Distance (HC-SR04+ sensor)

#### Hardware

- [Ultrasonic module MC-SR04+ (3.3V/5V version)](https://www.aliexpress.com/item/Ultrasonic-Module-HC-SR04-Distance-Measuring-Transducer-Sensor-HC-SR04-perfect-upgrade-support-3-3V-work/32303225320.html)

And connect it up
![HC-SR04+  wiring](https://github.com/encodeltd/Feather_M0_LoRa/raw/master/fritzing/HC-SR04plus_bb.png)


#### Software

- Install Marek Kuziel's fork of [NewPing](https://gitlab.com/markuz/NewPing-SAMD) Library

Remove the comments in front of the sensor you have, eg

```Arduino
#define SENSOR_SR04PLUS         // HC-SR04+
```

Then Upload the code to the Feather again.

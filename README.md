# Feather M0 LoRa Node
Example Arduino code of using an Adafruit Feather M0 LoRa module to send sensor data.

This code has been tested with KotahiNet in New Zealand. It should be able to be used on any LoraWAN network with little modification

## Things you need

### Hardware

- A [Adafruit Feather M0 with RFM95 LoRa Radio - 900MHz](https://www.adafruit.com/products/3178) to connect to [KotahiNet](http://kotahi.net/) in New Zealand
- (optional) BMP085/BMP180 Barometric pressure sensor
- (optional) DHT11/DHT21/DHT22 Humity and Temperature sensor

### Software

- [Download and Install the Arduino IDE](https://www.arduino.cc/en/Main/Software)
- [Import the Adafruit] boards(https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module/setup)
- [elapsedMillis](https://github.com/pfeerick/elapsedMillis) Library
- [LoRa-LMIC-1.51](https://github.com/mikenz/LoRa-LMIC-1.51) Library

## Sending your first message

### Connecting it up

To send the most basic "Hello World!" message you need to add two connections on the Feather board, and add an antenna. A straight 87mm long wire makes a perfect antenna to get started. Below are the connections needed:

fritzing/Hello World_bb.png

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
```
// LoRaWAN Config
// Device Address
devaddr_t DevAddr = 0x01234567;

// Network Session Key
unsigned char NwkSkey[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };

// Application Session Key
unsigned char AppSkey[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
```

Then click the Upload button in the Arduino IDE to compile the code and send it to your Feather. It will upload the code and it will start running. If you've configured the code correctly and you're in range of a KotahiNet receiver then you will have sucessfully sent your first messages.
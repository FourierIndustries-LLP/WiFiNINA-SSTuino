/*
  SerialNINAPassthrough - Use esptool to flash the ESP32 module
  For use with PyPortal, Metro M4 WiFi, SSTuino II...
  
  Modified by FourierIndustries for exclusive use with SSTuino II
  This code might not work with other Arduino variants

  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
  NOTE: This passthrough will only work if you have soldered the jumper on the
        back of the board labeled "IO0", or if you have a voltage level
        shifter that connects to pin 4, in which you need to modify the
        definition of ESP32_GPIO0 below
*/

unsigned long baud = 115200;

#define SerialESP32   Serial1 // Serial1, because Serial2 is not broken out
#define SPIWIFI       SPI
#define SPIWIFI_SS    35   // Chip select pin PF2
#define SPIWIFI_ACK   36   // a.k.a BUSY or READY pin PF3
#define ESP32_RESETN  29   // Reset pin PA7
#define ESP32_GPIO0   28   // If you are soldering the jumper, use pin 28
// #define ESP32_GPIO0   4   // If you are using our flashing jig, use pin 4

char ledstate = LOW;
unsigned long last_time = 0UL;
bool resetted = false;

void setup() {
  Serial.begin(baud);

  while (!Serial);

  SerialESP32.begin(baud);

  pinMode(SPIWIFI_SS, OUTPUT);
  pinMode(ESP32_GPIO0, OUTPUT);
  pinMode(ESP32_RESETN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // manually put the ESP32 in upload mode
  digitalWrite(ESP32_GPIO0, LOW); //set GPIO0 to low (DFU mode)
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(ESP32_GPIO0, LOW);
  digitalWrite(ESP32_RESETN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(ESP32_RESETN, HIGH);
}

void loop() {
  // Flash the LED on the board to indicate serial comms activity

  ledstate = LOW;
  digitalWrite(LED_BUILTIN, ledstate);
  while (Serial.available()) {
    SerialESP32.write(Serial.read());

    if (last_time < 10UL + millis()) {
      ledstate = HIGH;
      last_time = millis();
    }
  }

  while (SerialESP32.available()) {
    Serial.write(SerialESP32.read());
    if (last_time < 10UL + millis()) {
      ledstate = HIGH;
      last_time = millis();
    }
  }
  
  digitalWrite(LED_BUILTIN, ledstate);
}

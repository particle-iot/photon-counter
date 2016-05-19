/**
******************************************************************************
* @file	sales-counter.cpp
* @authors Brett Walach, Ido Kleinman, Jeff Eiden
* @version V1.0.0
* @date	18-May-2016
* @brief   Increase count displayed on very large 6.5" 7-segment digits from external internet button event / attached mechanical button - runs on Electron
******************************************************************************
 Copyright (c) 2013-16 Particle Industries, Inc.  All rights reserved.
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation, either
 version 3 of the License, or (at your option) any later version.
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, see <http://www.gnu.org/licenses/>.
******************************************************************************
*/

/* Put Core in Semi-automatic mode to resubscribe to events on connection drop*/
//SYSTEM_MODE(SEMI_AUTOMATIC);

/* Includes ------------------------------------------------------------------*/
#include "application.h"

typedef unsigned char byte;
const int kNumOfSevenSegments = 6;

struct Counter {
	 int value;
};

Counter counter;

#define OE_PIN A1 // output enable
#define LE_PIN A2 // latch enable
#define BUTTON_PIN A0 // physical button that does the same as the event 

/* Prototypes ----------------------------------------------------------------*/
void numPhotonsSoldHandler(const char *event, const char *data);
byte digit2SevenSeg(char c);

/* Helper functions ----------------------------------------------------------*/
byte digit2SevenSeg(char c)
{

 byte digit2SevenSegArr[17];

 // if it's a space then don't show any digit
 if (c == ' ')
		return 0;

		// COMMON ANODE       HGFEDCBA
 digit2SevenSegArr[0] = 0b00111111; // 0
 digit2SevenSegArr[1] = 0b00000110; // 1
 digit2SevenSegArr[2] = 0b01011011; // 2
 digit2SevenSegArr[3] = 0b01001111; // 3
 digit2SevenSegArr[4] = 0b01100110; // 4
 digit2SevenSegArr[5] = 0b01101101; // 5
 digit2SevenSegArr[6] = 0b01111101; // 6
 digit2SevenSegArr[7] = 0b00000111; // 7
 digit2SevenSegArr[8] = 0b01111111; // 8
 digit2SevenSegArr[9] = 0b01101111; // 9

 // digit2SevenSegArr[10] = 0b10000001; // FIGURE 8 SECTION 'a'
 // digit2SevenSegArr[11] = 0b10100000; // 'f'
 // digit2SevenSegArr[12] = 0b11000000; // 'g'
 // digit2SevenSegArr[13] = 0b10000100; // 'c'
 // digit2SevenSegArr[14] = 0b10001000; // 'd'
 // digit2SevenSegArr[15] = 0b10010000; // 'e'
 // digit2SevenSegArr[16] = 0b11000000; // 'g'
 // digit2SevenSegArr[17] = 0b10000010; // 'b'

 int ai = c - '0';
 if ((ai>=0) && (ai<=9))
 {
		return digit2SevenSegArr[ai];
 }
 else
 {
	 return -1;
 }
}

int setCount(String valueStr)
{
	 int value = atoi(valueStr.c_str());
	 if ((value >= 0) && (value < 1000000)) {
			 counter.value = value;
			 EEPROM.put(0,counter);
			 setCountInt(counter.value);
	 }

	 return 1;
}


void setCountInt(int value)
{
	 if ((value > 0) && (value < 1000000)) {
			 char cStr[7];
			 // itoa(value,cStr,10);
			 sprintf(cStr, "%6d",value);
			 writeToDigits(String(cStr));
	 }
}

int animate(String params)
{
	 byte animateDigit;
	 int maxAnim = atoi(params.c_str());
	 if (maxAnim <= 0) {
			 maxAnim=1;
	 }

	 for (int j=0; j<maxAnim; j++) {
			 animateDigit = 0b00000001;
			 for (int i=0; i<6; i++) {

				 digitalWrite(LE_PIN, LOW);

				 for (int j=0; j<kNumOfSevenSegments; j++)
				 {
					 SPI.transfer(animateDigit); // send value (0~255)
				 }
				 animateDigit = animateDigit << 1;

				 digitalWrite(LE_PIN, HIGH);
				 delayMicroseconds(1);
				 digitalWrite(LE_PIN, LOW);
				 delay(80);
			 }
	 }

	 return 1;

}


int writeToDigits(String digitString)
{
 byte sendByte;

 digitalWrite(LE_PIN, LOW);

 for (int i=digitString.length()-1; i>=0; i--)
 {
	 sendByte = digit2SevenSeg(digitString.c_str()[i]);
	 SPI.transfer(sendByte); // send value (0~255)
 }

 digitalWrite(LE_PIN, HIGH);
 delayMicroseconds(1);
 digitalWrite(LE_PIN, LOW);

	 return 1;
}


void increaseEventHandler(const char *event, const char *data)
{
	 counter.value++;
	 EEPROM.put(0, counter);
	 animate(String(2));
	 setCountInt(counter.value);
}


/* Setup ---------------------------------------------------------------------*/
void setup()
{
	 STARTUP(System.enableFeature(FEATURE_RETAINED_MEMORY));
 Particle.connect();
 pinMode(OE_PIN, OUTPUT);
 pinMode(LE_PIN, OUTPUT);
 digitalWrite(OE_PIN, LOW);
 SPI.begin();
 SPI.setBitOrder(MSBFIRST);
 SPI.setClockDivider(SPI_CLOCK_DIV128);
// 	Particle.function("writeDigits", writeToDigits);
 Particle.function("animate",animate);
 Particle.function("setCount",setCount);
	 Particle.subscribe("increase-counter",increaseEventHandler, MY_DEVICES);

	 EEPROM.get(0, counter);
	 delay(100);
	 setCountInt(counter.value);

	 pinMode(BUTTON_PIN, INPUT_PULLUP);

// 	Particle.subscribe("numPhotonsSold", numPhotonsSoldHandler);
 Serial.begin(9600);
}


/* Loop ----------------------------------------------------------------------*/
void loop()
{
 if(!Particle.connected()) {
	 System.reset();
 }

 if (digitalRead(BUTTON_PIN) == LOW) {
	 delay(40); // debounce
	 if (digitalRead(BUTTON_PIN) == LOW) {
			 increaseEventHandler("increase-counter", NULL);
	 }
 }
}

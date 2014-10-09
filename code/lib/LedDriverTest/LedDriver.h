// TLC5917IN LED Driver Library
//     by Ben Wang, 10/5/14
//     EECS 473 - University of Michigan - Ann Arbor

#include <arduino.h>

const unsigned int LED_CLK = 2;	
const unsigned int LED_LE  = 7;	
const unsigned int LED_OE  = 3;		// Can always tie to low for LEDs to always be enabled
const unsigned int LED_SDI = 4;	

void LED_initialize();
void LED_write(char data);
void LED_setBrightness(int brightness);
void LED_enableOutput();
void LED_disableOutput();



void LED_initialize(){
	// Initialize pins
	pinMode(LED_CLK, OUTPUT);
	pinMode(LED_LE, OUTPUT);
	pinMode(LED_OE, OUTPUT);
	pinMode(LED_SDI, OUTPUT);

	// Initial conditions
	digitalWrite(LED_CLK, 0);
	digitalWrite(LED_LE, 0);
	digitalWrite(LED_OE, 1);
	digitalWrite(LED_SDI, 0);
}

void LED_write(char data){
	// Write LED data bits
	for (int led = 0; led < 8; ++led){
		// Write a bit
		digitalWrite(LED_SDI, (data & 0x80));
		digitalWrite(LED_CLK, 1);
		digitalWrite(LED_CLK, 0);
		
		// Shift data
		data = data << 1;
	}

        // Set SDI wire to low
        digitalWrite(LED_SDI, 0);
	
	// Latch data
	digitalWrite(LED_LE, 1);
	digitalWrite(LED_LE, 0);
}

void LED_setBrightness(int brightness){
	analogWrite(LED_OE, 255 - brightness);  // "255 - " Because active LOW
}

void LED_enableOutput(){
	digitalWrite(LED_OE, 0);
}

void LED_disableOutput(){
	digitalWrite(LED_OE, 1);
}

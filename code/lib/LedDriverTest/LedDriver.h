// TLC5917IN LED Driver Library
//     by Ben Wang, 10/5/14
//     EECS 473 - University of Michigan - Ann Arbor

#include <arduino.h>
#include <SPI.h>

const unsigned int LED_CLK = 13;	
const unsigned int LED_LE  = 12;	
const unsigned int LED_OE  = 10;		// Can always tie to low for LEDs to always be enabled
const unsigned int LED_SDI = 11;	

void LED_initialize();
void LED_write(char data);
void LED_setBrightness(int brightness);
void LED_enableOutput();
void LED_disableOutput();


void LED_initialize(){
	// Initialize pins
	pinMode(LED_LE, OUTPUT);
	pinMode(LED_OE, OUTPUT);

	// Initial conditions
        SPI.begin();
        SPI.setBitOrder(MSBFIRST);
        SPI.setDataMode(SPI_MODE0);
}

void LED_write(char data){
	// Write LED data bits
        SPI.transfer(data);
	
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

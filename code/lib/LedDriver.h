// TLC5917IN LED Driver Library
//     by Ben Wang, 10/5/14
//     EECS 473 - University of Michigan - Ann Arbor

//*****************************************************************************
//******************** Configuration and Interface ****************************
//*****************************************************************************

const unsigned int LED_CLK = 2;	
const unsigned int LED_LE  = 3;	
const unsigned int LED_OE  = 4;		// Can always tie to low for LEDs to always be enabled
const unsigned int LED_SDI = 5;	

void LED_initialize();
void LED_write(byte data);
void LED_enableOutput();
void LED_disableOutput();


//*****************************************************************************
//****************************** Functions ************************************
//*****************************************************************************

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

void LED_write(byte data){
	// Write LED data bits
	for (int led = 0; led < 8; ++led){
		// Write a bit
		digitalWrite(LED_LE, (data & 0x1));
		digitalWrite(LED_CLK, 1);
		digitalWrite(LED_CLK, 0);
		
		// Shift data
		data = data >> 1;
	}
	
	// Latch data
	digitalWrite(LED_LE, 1);
	digitalWrite(LED_LE, 0);
}

void LED_enableOutput(){
	digitalWrite(LED_OE, 0);
}

void LED_disableOutput(){
	digitalWrite(LED_OE, 1);
}
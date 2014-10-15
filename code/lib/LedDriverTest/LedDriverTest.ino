// Sample program using TLC5917IN LED driver

#include "LedDriver.h"
#include <SPI.h>

void setup() {
  Serial.begin(115200);
  
  // Initialize LEDs
  LED_initialize();
  
  // Set all off
  LED_write(0xA7);
  
  // Enable
  LED_enableOutput();
  
  // Wait and show debug status
  pinMode(13, OUTPUT);
  digitalWrite(13, 1);  // Debug status LED
  delay(100);
  digitalWrite(13,0);
  
  // Disable
  LED_disableOutput();
  
  // Pulse all LEDs to indicate transition
  pulseAll();
}

void loop() {
  
//********************************************************************
//********** Turn all LEDs on, ramp up then down *********************
//********************************************************************
  Serial.println("Ramp up");
  
  // Set LEDs on
  LED_write(0xAA);
  
  // Ramp up and down brightness
  for (int j = 0; j < 256; ++j){
    LED_setBrightness(j);
    delay(5);
    
    Serial.print("  "); Serial.println(j);
  }
  
  // Switch LEDs
  LED_write(0x55);
  
  Serial.println("Ramp down");
  for (int j = 255; j >= 0; --j){
    LED_setBrightness(j);
    delay(5);
    
    Serial.print("  "); Serial.println(j);
  }
  
  LED_disableOutput();
  
  // Transition
  delay(100);
  
//********************************************************************
//********** Light up individual LEDs ********************************
//********************************************************************
  
  // Light each LED individually
  for (int j = 0; j < 8; ++j){
Serial.print("Lighting LED: "); Serial.println(j);
    LED_write(1 << j);
    LED_enableOutput();
    delay(300);
  }
  
  LED_disableOutput();
  
  // Transition
  delay(100);
  
//********************************************************************
//**** Binary counter Demo *******************************************
//********************************************************************

  // Enable LEDs throughout entire 
  LED_enableOutput();

  // Light each LED individually
  for (int j = 0; j < 128; ++j){
Serial.print("  Writing to LEDs: "); Serial.println((int)j);
    LED_write(j);
    delay(5);
  }
  
  LED_disableOutput();
  
  // Transition
  delay(100);
}






//********************************************************************
//********** Pulse all LEDs to indicate transistion ******************
//********************************************************************

void pulseAll(){
  Serial.print("Pulsing");
  
  // Set all on
  LED_write(0xff);
  
  // Enable
  LED_enableOutput();
  
  // Wait and show debug status
  digitalWrite(13, 1);  // Debug status LED
  delay(100);
  digitalWrite(13,0);
  
  // Disable
  LED_disableOutput();
}
//#include "LedDriver.cpp.h"

/*
 * sketch: mega_olimex_agniheart.ino
 *
 *  Created on: Apr 24, 2015
 *      Author: DevbNJ
 *  Copyright and Product Acknowledgments:
 *      Arduino Mega: Arduico.cc
 *      Olimex EKG/EMG Hardware / Software: 
 *      https://www.olimex.com/Products/Duino/Shields/SHIELD-EKG-EMG/
 *      LCD Driver: Adafruit 
 *      https://github.com/adafruit/Adafruit_SSD1306
 *      Arduino Board: SainSmart Mega 2560
 *      LCD Screen: DiyMall 0.96" OLED SSD3306 Screen White Display
 *      Shield: GikFun Prototype Shield for Arduino
 */
#include <compat/deprecated.h>
#include <FlexiTimer2.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Panchanga.h"

#define SAMPFREQ 256                      // ADC sampling rate 256
#define TIMER2VAL (1024/(SAMPFREQ))       // Set 256Hz sampling frequency                    
volatile unsigned char CurrentCh = 0;     //Current channel being sampled.
volatile unsigned int ADC_Value = 0;	  //ADC current value
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// Needed by Panchanga
uint8_t nDay = 6;
uint8_t nDD = 11;
uint8_t nMM = 4;
unsigned int nYY = 2015;
uint8_t nHour = 23;
uint8_t nMin = 18;
int nTZ = -4;
char buf[2];
char sbuf[15];
Panchanga pch;

void setup() {
  // Set the heart machine first
  noInterrupts();  // Disable all interrupts before initialization

  FlexiTimer2::set(TIMER2VAL, Timer2_Overflow_ISR);
  FlexiTimer2::start();

  // Serial Port
  Serial.begin(57600);

  // Enable all interrupts after initialization has been completed
  interrupts();  
  // Heart machine done

  doPanchanga();
}

void doPanchanga() {
  // Do the Panchanga Next
  pch.initialize_panchanga(nDD, nMM, nYY, nHour, nMin, nTZ);
  
  delay(200);
  
  // Set the OLED Now
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Clear the buffer.
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.println("CardioAgni");
  display.setTextSize(0);
  display.setCursor(0, 18);
  display.println("EKG/EMG with");
  display.setCursor(0, 28);
  display.println("Passive Electrodes");
  display.setCursor(0, 38);
  display.println("(c) Agnicure");
  display.setCursor(0, 48);
  display.println("All Rights Reserved");
  display.display();
  
  /*
  delay(10000);
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Panchanga");
  display.setTextSize(0);
  sprintf(buf, "%d", pch.n_tithi);
  display.setCursor(0, 18);
  display.println("Tithi:");
  display.println(buf);
  display.display();
  delay(2000);
  */
}    

void Timer2_Overflow_ISR() {
  ADC_Value = analogRead(CurrentCh);
  // Spit out the values
  Serial.print("ADC, ");    
  Serial.println(ADC_Value);
}

void loop() {
  __asm__ __volatile__ ("sleep");
}

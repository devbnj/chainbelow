/*
 * Sketch: msp4305969_panch_kal.ino
 *
 *  Created on: Apr 24, 2015
 *      Author: DevbNJ
 *  Copyright and Product Acknowledgments:
 *      TI MSP4305969 Launchpad
 *      Adafruit BMP085 weather sensor
 *      BMP template library astuder
 *      https://github.com/astuder/BMP085-template-library-Energia
 *      Energia Sharp Memory LCD Booster Pack
 *      http://energia.nu/reference/sharp-memory-lcd-boosterpack/
 *  Distributed under Open Source Apache License
 *  Copyright Devb Inc. All Rights Reserved
 *  Please do not remove the copyright notice
 */

#include "Energia.h"
#include "Timer.h"
#include "SPI.h"
#include "LCD_SharpBoosterPack_SPI.h"
#include "Panchanga.h"
// required by BMP085 library
#include "Wire.h"          
// import BMP085 template library
#include "BMP085_t.h"      

// BMP085<2, 0, BMP085_F> PSensor;
BMP085<3, 0, BMP085_F> PSensor;        

// Variables
#ifndef SPI_CLOCK_DIV2
#define SPI_CLOCK_DIV2   2
#endif

LCD_SharpBoosterPack_SPI myScreen;
uint8_t k = 0;
Timer t;

// leds on the launchpad
#define LRED RED_LED
#define LYEL 55
#define LGRE GREEN_LED

// variables for the timer
int starttime;
int activetime;
int previoustime = 0;
double ntemp;
double npres;

// set the day and time
uint8_t nDay = 6;
uint8_t nDD = 11;
uint8_t nMM = 4;
unsigned int nYY = 2015;
uint8_t nHour = 23;
uint8_t nMin = 18;
int nTZ = -4;
char buf[4];
char sbuf[15];

// setup code
void setup() {
  pinMode(P1_6, OUTPUT);
  pinMode(P1_7, OUTPUT);
  digitalWrite(P1_7, LOW);
  Serial.begin(9600);

  // Barometer Sensor
  digitalWrite(P1_7, HIGH);
  Serial.println("Wire begin...");
  // initialize I2C that connects to sensor
  Wire.begin();            
  // initalize pressure sensor
  PSensor.begin();         
  Serial.println("Wire began...");
  showPressureTemp();
  Wire.flush();
  digitalWrite(P1_7, LOW);
  // digitalWrite(P1_6, HIGH);
  Serial.println("Wire flushed...");

  // Setup LCD
  Serial.println("Initializing SPI...");
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  Serial.println("Initialized SPI...");

  myScreen.begin();
  Serial.println("Screen began...");
  
  myScreen.setFont(1);
  myScreen.text(10, 5, "Vedic");
  myScreen.setFont(0);
  myScreen.text(10, 30, "Jotiz Clock");
  myScreen.text(10, 40, "(c)devb inc");
  myScreen.flush();  

  delay(2000);
  myScreen.clearBuffer();
  doTime();
  // one minute
  int tickEvent = t.every(10000, doTime); 
}

// loop code
void loop() {
  t.update();
}

// show kaala and shoola
void showTime(char *kaal, char *shoola, char *day) {
  myScreen.setFont(0);
  itoa(nHour, buf);
  myScreen.text(46, 30, buf);
  itoa(nMin, buf);
  myScreen.text(66, 30, buf);
  myScreen.text(6, 30, day);
  myScreen.text(6, 10, kaal);
  myScreen.text(46, 10, shoola);
  for (uint8_t i=10; i<LCD_HORIZONTAL_MAX-10; i++) {
    myScreen.setXY(i,20,1);
  }
  myScreen.flush(); 
}

// show pressure and temperature
void showPressureTemp() {
  // read current sensor data
  PSensor.refresh();                    
  // run calculations for temperature and pressure
  PSensor.calculate();                  
  ntemp = PSensor.temperature/10;
  npres = (PSensor.pressure+50)/3376;
  Serial.println(ntemp);
  Serial.print("Pressure:    ");
  Serial.print(npres);   // display pressure in hPa
  Serial.println("inHG");
  // PSensor.pause();
  delay(5000);
}  

// show time only
void showTimeOnly(char *day) {
  myScreen.setFont(0);
  myScreen.text(46, 10, "Jotiz");
  itoa(nHour, buf);
  myScreen.text(46, 30, buf);
  itoa(nMin, buf);
  myScreen.text(66, 30, buf);
  myScreen.text(6, 30, day);
  myScreen.text(6, 10, "      ");
  for (uint8_t i=10; i<LCD_HORIZONTAL_MAX-10; i++) {
    myScreen.setXY(i,20,1);
  }
  for (uint8_t i=10; i<LCD_HORIZONTAL_MAX-10; i++) {
    myScreen.setXY(i,50,1);
  }
  myScreen.text(6, 60, "Temp");
  ftoa(buf, ntemp, 4);
  myScreen.text(46, 60, buf);
  myScreen.text(70, 60, "F");
  myScreen.text(6, 70, "Pres");
  ftoa(buf, npres, 4);
  myScreen.text(46, 70, buf);
  myScreen.text(70, 70, "inHG");
  myScreen.flush(); 
}

// compute time
void doTime() {
  float h1;
  nMin++;
  if (nMin > 59) {
      nHour++;
      nMin = 0;
  }
  if (nHour > 23) {
      nDay++;
      nHour = 0;
  }
  if (nDay > 6) nDay = 0;
  h1 = nHour + (nMin / 60);

  switch (nDay) {
  case 0:
    if ((h1 >= 16.5) && (h1 <= 18)) showTime("Rahu  ","W","Sun");
    else if (h1 >= 15 && h1 < 16.5) showTime("Gulika","W","Sun");
    else if ((h1 >= 12 && h1 <= 13.5) || (h1 >= 18 && h1 <= 19.5)) showTime("Yama  ","W","Sun");
    else showTimeOnly("Sun");
    break;
  case 1:
    if (h1 >= 7.5 && h1 <= 9) showTime("Rahu  ","E","Mon");
    else if (h1 >= 13.5 && h1 < 15) showTime("Gulika","E","Mon");
    else if ((h1 >= 10.5 && h1 <= 12) || (h1 >= 3 && h1 <= 4.5)) showTime("Yama  ","E","Mon");
    else showTimeOnly("Mon");
    break;
  case 2:
    if (h1 >= 15 && h1 <= 16.5) showTime("Rahu  ","N","Tue");
    else if (h1 >= 12 && h1 < 13.5) showTime("Gulika","N","Tue");
    else if ((h1 >= 9 && h1 <= 10.5) || (h1 >= 1.5 && h1 <= 3)) showTime("Yama  ","N","Tue");
    else showTimeOnly("Tue");
    break;
  case 3:
    if (h1 >= 12 && h1 <= 13.5) showTime("Rahu  ","N","Wed");
    else if (h1 >= 10.5 && h1 < 12) showTime("Gulika","N","Wed");
    else if (h1 >= 7.5 && h1 <= 9) showTime("Yama  ","N","Wed");
    else showTimeOnly("Wed");
    break;
  case 4:
    if (h1 >= 13.5 && h1 <= 15) showTime("Rahu  ","S","Thu");
    else if (h1 >= 9 && h1 < 10.5) showTime("Gulika","S","Thu");
    else if ((h1 >= 6 && h1 <= 7.5) || (h1 >= 22 && h1 <= 24)) showTime("Yama  ","S","Thu");
    else showTimeOnly("Thu");
    break;
  case 5:
    if (h1 >= 10.5 && h1 <= 12) showTime("Rahu  ","W","Fri");
    else if (h1 >= 7.5 && h1 < 9) showTime("Gulika","W","Fri");
    else if ((h1 >= 15 && h1 <= 16.5) || (h1 >= 19.5 && h1 <= 21)) showTime("Yama  ","W","Fri");
    else showTimeOnly("Fri");
    break;
  case 6:
    if (h1 >= 9 && h1 <= 10.5) showTime("Rahu  ","E","Sat");
    else if (h1 >= 6 && h1 < 7.5) showTime("Gulika","E","Sat");
    else if ((h1 >= 13.5 && h1 <= 15) || (h1 >= 19.5 && h1 <= 21)) showTime("Yama  ","E","Sat");
    else showTimeOnly("Sat");
    break;
  default: 
    break;
  }
}

// draw box
void drawBox () {
  for (uint8_t i=0; i<=20; i++) {
    myScreen.setXY(50+i,30,1);
  }
  for (uint8_t i=0; i<=20; i++) {
    myScreen.setXY(50,30+i,1);
  }
  for (uint8_t i=0; i<=20; i++) {
    myScreen.setXY(50+i,50,1);
  }
  for (uint8_t i=0; i<=20; i++) {
    myScreen.setXY(70,30+i,1);
  }
}

// k&r extensions to std C
char *ftoa(char *a, double f, int precision) {
	long p[] = { 0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 };

	char *ret = a;
	long heiltal = (long) f;
	itoa(heiltal, a);
	while (*a != '\0')
		a++;
	*a++ = '.';
	long desimal = abs((long) ((f - heiltal) * p[precision]));
	itoa(desimal, a);
	return ret;
}

void itoa(int n, char s[]) {
     int i, sign;

     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void reverse(char s[])  {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }




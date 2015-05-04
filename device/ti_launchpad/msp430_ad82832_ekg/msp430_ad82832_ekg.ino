/*
 * msp430_ad8232_ekg.ino
 *
 *  Created on: Apr 24, 2015
 *      Author: DevbNJ
 *  Copyright and Product Acknowledgments:
 *      TI EXP430G2553 Launchpad
 *      Sparkfun AD8232 EKG Single channel sensor
 *  Distributed under Open Source Apache License
 *  Copyright Devb Inc. All Rights Reserved
 *  Please do not remove the copyright notice
 */

// Sparkfun AD8232 connected to 
// MSP430 G2553 Launchpad
// **********************************
// 3.3V * *                   * * Gnd
//      * * A0/Output         * * 
//      * *                   * * 
//      * *                   * * 
//      * *                   * * 
//      * *                   * * 
//      * *                   * * 
//      * *                   * * Lo+/13 
//      * *                   * * Lo-/12 
//      * *                   * * 
// **********************************

#include "energia.h"
int starttime;
int activetime;
int previoustime = 0;
int cnt = 0;
volatile unsigned int inADC = 0;
#define PLPLUS P2_5
#define PLMINUS P2_4

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
  // PinDirModeSet(6, PIN_DIR_MODE_IN);
  // PinConfigSet(6, PIN_STRENGTH_2MA, PIN_TYPE_ANALOG);
  // PinTypeADC(6, PIN_MODE_6);
  pinMode(PLPLUS, INPUT); // Setup for leads off detection LO +
  pinMode(PLMINUS, INPUT); // Setup for leads off detection LO -
  starttime = millis();
}

void loop() {

  if ((digitalRead(PLPLUS) == 1) || (digitalRead(PLMINUS) == 1)) {
    // Leads not connected
    Serial.println('!');
  } else{
    // send the value of analog input 0:
    inADC = analogRead(A0);  // PIN_06
    // inADC = map(inADC, -4096, 4096, 1024, 1024);
    Serial.print("ADC,");
    Serial.print(inADC);
    Serial.print(",");
    Serial.println(activetime);
    ++cnt;
  }
  //Wait for a bit to keep serial data from saturating
  delay(1);
  activetime = millis() - starttime;
}



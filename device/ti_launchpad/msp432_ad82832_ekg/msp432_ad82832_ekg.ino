/*
 * msp432_ad8232_ekg.ino
 *
 *  Created on: Apr 24, 2015
 *      Author: DevbNJ
 *  Copyright and Product Acknowledgments:
 *      TI EXP432P410R Launchpad
 *      Sparkfun AD8232 EKG Single channel sensor
 *  Distributed under Open Source Apache License
 *  Copyright Devb Inc. All Rights Reserved
 *  Please do not remove the copyright notice
 */

// Sparkfun AD8232 connected to 
// MSP432P401R Launchpad
// **********************************
// 3.3V * *                   * * Gnd
// Lo+/2* *                   * * 
// Lo-/3* *                   * * 
//      * *                   * * 
//      * *                   * * 
//      * *                   * * 
//      * *                   * * 
//      * *                   * * 
//      * *                   * * 
//      * * A0/Output         * * 
// **********************************

#include "energia.h"
int starttime;
int activetime;
int previoustime = 0;
int cnt = 0;
volatile unsigned int inADC = 0;

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
  // PinDirModeSet(6, PIN_DIR_MODE_IN);
  // PinConfigSet(6, PIN_STRENGTH_2MA, PIN_TYPE_ANALOG);
  // PinTypeADC(6, PIN_MODE_6);
  pinMode(2, INPUT); // Setup for leads off detection LO +
  pinMode(3, INPUT); // Setup for leads off detection LO -
  starttime = millis();
}

void loop() {

  if ((digitalRead(2) == 1) || (digitalRead(3) == 1)) {
    // Leads not connected
    // Serial.println('!');
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
  /*
  if (previoustime < (activetime - 59)) {
    Serial.print("Count = ");
    Serial.println(cnt);
    Serial.flush();
    Serial.end();
    previoustime = activetime;
    return;
  }
  */
}



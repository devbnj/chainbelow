/*
 * CC3200_ad8232_ekg.ino
 *
 *  Created on: Apr 24, 2015
 *      Author: DevbNJ
 *  Copyright and Product Acknowledgments:
 *      TI CC3200 Launchpad
 *      Sparkfun AD8232 EKG Single channel sensor
 *  Distributed under Open Source Apache License
 *  Copyright Devb Inc. All Rights Reserved
 *  Please do not remove the copyright notice
 */

// Sparkfun AD8232 connected to 
// CC3200 Launchpad
// **********************************
// 3.3V * *                   * * Gnd
//      * *                   * * 
//      * *                   * * 
//      * *                   * * 
//      * *                   * * 
// Out/6* *                   * * 
// Lo+/7* *                   * * 
// Lo-/8* *                   * * 
//      * *                   * * 
//      * *                   * * 
// **********************************

#include "energia.h"
int starttime;
int activetime;
int previoustime = 0;
int cnt = 0;
volatile unsigned int inADC = 0;

void setup() {
  // initialize the serial communication:
  Serial.begin(57600);
  // PinDirModeSet(6, PIN_DIR_MODE_IN);
  // PinConfigSet(6, PIN_STRENGTH_2MA, PIN_TYPE_ANALOG);
  // PinTypeADC(6, PIN_MODE_6);
  pinMode(7, INPUT); // Setup for leads off detection LO +
  pinMode(8, INPUT); // Setup for leads off detection LO -
  starttime = millis();
}

void loop() {

  if((digitalRead(8) == 1)||(digitalRead(7) == 1)){
    // Serial.println('!');
  }
  else{
    // send the value of analog input 0:
    inADC = analogRead(6);  // PIN_06
    // inADC = map(inADC, -4096, 4096, -2048, 2048);
    // if (inADC != 0)
    Serial.print("ADC,");
    Serial.print(inADC);
    Serial.print(",");
    Serial.println(activetime);
    ++cnt;
  }
  //Wait for a bit to keep serial data from saturating
  delay(1);
  activetime = millis()- starttime;
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



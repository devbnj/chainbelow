/*
 * sketch: due_olimex_agniheart.ino
 *
 *  Created on: Apr 24, 2015
 *      Author: DevbNJ
 *  Copyright and Product Acknowledgments:
 *      Arduino Due: Arduico.cc
 *      Arduino Due Timer: http://forum.arduino.cc/index.php?topic=130423.0
 *      Timer (RTC): https://github.com/PaulStoffregen/Time
 *      Olimex EKG/EMG Hardware / Software:
 *      https://www.olimex.com/Products/Duino/Shields/SHIELD-EKG-EMG/
 *      LCD Driver: U8Lib - https://code.google.com/p/u8glib/
 *      Arduino Board: SainSmart Due SAM3X8E
 *      LCD Screen: DiyMall 0.96" OLED SSD3306 Screen White Display
 *      Shield: GikFun Prototype Shield for Arduino
 *  Distributed under Open Source Apache License
 *  Copyright Devb Inc. All Rights Reserved
 *  Please do not remove the copyright notice
 */
 
// ******************************************
// Due Hardware connections
// Res *                                    * 
// 3.3 * OLED VCC                           *
// 5.0 *                                    *
// Gnd * OLED GND                           *
// Gnd *                                    *
// Vin *                                    *
//
//     *                                    *
//     *                                    *
//     *                                    *
//     *                                    *
//     *                                    *
//     *                                    *
//     *                                    *
//     *                                    *
//     *                                    *
//     *                                    * 10
//     *                                    * 11
//     **************************************
//     **************************************

#include "Time.h"
#include "U8glib.h"
#include "Panchanga.h"

volatile boolean l;
boolean pTime = false;
boolean pDone = false;
// ADC = Analog to Digital converter
volatile unsigned int inADC = 0;
// toggle
boolean timerInitiated = false;
// Fast I2C / TWI - LCD Screen
U8GLIB_SSD1306_128X64 screen(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);

// Needed by Panchanga
uint8_t nDay = 6;
uint8_t nDD = 11;
uint8_t nMM = 4;
unsigned int nYY = 2015;
uint8_t nHour = 23;
uint8_t nMin = 18;
uint8_t nSec = 0;
int nTZ = -4;
char buf[2];
char sbuf[15];
Panchanga pch;
unsigned int starttime;
unsigned int activetime;

void draw(void) {
  screen.setFont(u8g_font_gdr12r);
  screen.drawStr(0, 30, "Vedic Machine");
  screen.drawHLine(2, 35, 47);
  screen.drawVLine(45, 32, 12);
  screen.setFont(u8g_font_4x6);
  screen.drawStr(1, 54, "jotiz.com/vm");
}

void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  uint32_t rc = VARIANT_MCK / 128 / frequency; //128 because we selected TIMER_CLOCK4 above
  TC_SetRA(tc, channel, rc / 2); //50% high, 50% low
  TC_SetRC(tc, channel, rc);
  TC_Start(tc, channel);
  tc->TC_CHANNEL[channel].TC_IER = TC_IER_CPCS;
  tc->TC_CHANNEL[channel].TC_IDR = ~TC_IER_CPCS;
  NVIC_EnableIRQ(irq);
}

void setup() {
  screen.firstPage();
  do {
    draw();
    screen.setColorIndex(1);
  } while ( screen.nextPage() );

  starttime = millis();
  Serial.begin(57600);
  Serial.setTimeout(2000);
  Serial.println("Vedic Heart Machine");
  Serial.println(">> Please input Time : Example \"21,53,0,25,4,2015,-4\"");
  if (!pTime) {
    processTime();
    pTime = true;
  } 
  if (!pDone) {
    doPanchanga();
    pDone = true;
  }
}

void doPanchanga() {
  Serial.print("PCH, Date ");
  Serial.print(nYY);
  Serial.print("-");
  Serial.print(nMM);
  Serial.print("-");
  Serial.print(nDD);
  Serial.print(" Hour ");
  Serial.print(nHour);
  Serial.print(":");
  Serial.print(nMin);
  Serial.print(" TimeZone ");
  Serial.println(nTZ);

  pch.initialize_panchanga(nDD, nMM, nYY, nHour, nMin, nTZ);

  delay(200);
  pDone = true;

  // Set the timer for EKG Reading
  // TC1 : timer counter. Can be TC0, TC1 or TC2
  // 0   : channel. Can be 0, 1 or 2
  // TC3_IRQn: irq number.
  // 250 : frequency (in Hz)
  // The interrupt service routine is TC3_Handler.
  startTimer(TC1, 0, TC3_IRQn, 250);
}

void processTime() {
  nHour = Serial.parseInt();
  nMin = Serial.parseInt();
  nSec = Serial.parseInt();
  nDD = Serial.parseInt();
  nMM = Serial.parseInt();
  nYY = Serial.parseInt();
  nTZ = Serial.parseInt();
  if (nYY == 0) nYY = 2015;
  if (nDD == 0) nDD = 25;
  if (nMM == 0) nMM = 4;
  if (nHour == 0) nHour = 23;
  if (nMin == 0) nMin = 36;
  if (nTZ == 0) nTZ = -4;
  setTime(nHour, nMin, 0, nDD, nMM, nYY);
}

void loop() {
  activetime = millis() - starttime;
  if (timerInitiated) {
    inADC = analogRead(0);
    Serial.print("ADC,");
    Serial.print(inADC);
    Serial.print(",");
    Serial.println(activetime);
    timerInitiated = false;
  }
}


// This function is called every 1/250 sec.
void TC3_Handler() {
  TC_GetStatus(TC1, 0);
  timerInitiated = true;
}

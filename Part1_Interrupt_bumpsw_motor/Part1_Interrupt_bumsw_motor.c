// InputOutput.c
// Runs on MSP432
// Test the GPIO initialization functions by setting the LED
// color according to the status of the switches.
// Only SW1 makes color LED blue, and red LED on
// Only SW2 makes color LED red, and red LED on
// Both SW1 and SW2 makes color LED purple, and red LED on
// Neither SW1 or SW2 turns LEDs off
// Daniel and Jonathan Valvano
// September 23, 2017

/* This example accompanies the books
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
       ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
       ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
       ISBN: 978-1466468863, , Jonathan Valvano, copyright (c) 2017
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2017, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// built-in LED1 connected to P1.0
// negative logic built-in Button 1 connected to P1.1
// negative logic built-in Button 2 connected to P1.4
// built-in red LED connected to P2.0
// built-in green LED connected to P2.1
// built-in blue LED connected to P2.2
// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// yellow   RG-    0x03
// sky blue -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05

// Modified by: Mohd A. Zainol

#include <stdint.h>
#include "msp.h"
#include "Clock.h"
#include "SysTick.h"
#include "CortexM.h"

#define SW1       0x02                  // on the left side of the LaunchPad board
#define SW2       0x10                  // on the right side of the LaunchPad board

#define RED       0x01
#define GREEN     0x02
#define YELLOW    0x03
#define BLUE      0x04
#define PINK      0x05
#define SKYBLUE   0x06
#define WHITE     0x07

// switch button using interrupt
void EdgeTrigger_Init(void){
  P1->SEL0 &= ~0x12;
  P1->SEL1 &= ~0x12;                 // configure P1.4 and P1.1 as GPIO
  P1->DIR &= ~0x12;                  // make P1.4 and P1.1 in (built-in Button 1 and Button 2)
  P1->REN |= 0x12;                   // enable pull resistors on P1.4 and P1.1
  P1->OUT |= 0x12;                   // P1.4 and P1.1 are pull-up
  P1->IES |= 0x12;                   // P1.4 and P1.1 are falling edge event
  P1->IFG &= ~0x12;                  // clear flag4 and flag1 (reduce possibility of extra interrupt)
  P1->IE |= 0x12;                    // arm interrupt on P1.4 and P1.1
  NVIC->IP[8] = (NVIC->IP[8]&0x00FFFFFF)|0x40000000; // priority 2
  NVIC->ISER[1] = 0x00000008;        // enable interrupt 35 in NVIC
}

// Initialize Bump sensors using interrupt
// Make six Port 4 pins inputs
// Activate interface pullup
// pins 7,6,5,3,2,0
void BumpEdgeTrigger_Init(void){
    P4->SEL0 &= ~0xED;
    P4->SEL1 &= ~0xED;    // 1) configure as GPIO
    P4->DIR &= ~0xED;     // 2) make in
    P4->REN |= 0xED;      // 3) enable pull resistors
    P4->OUT |= 0xED;      //    pull-up
    P4->IES |= 0xED;                   // falling edge event
    P4->IFG &= ~0xED;                  // clear flag
    P4->IE |= 0xED;                    // arm interrupt
    NVIC->IP[9] = (NVIC->IP[9]&0xFF00FFFF)|0x00400000; // priority 2 on port4
    NVIC->ISER[1] = 0x00000040;        // enable interrupt 38 in NVIC on port4
}

// Uses P4IV IRQ handler to solve critical section/race
void PORT4_IRQHandler(void){

    uint8_t status;
    uint32_t H,L;

    H = 750; // PWM using H and L
    L = 1000-H;

    // Movement
    // RED: Forward
    // BLUE: Turn right
    // YELLOW: Turn left
    // GREEN: Backward

      //status = Bump_Read_Input();

    // Interrupt Vector of Port4
      status = P4->IV; // 2*(n+1) where n is highest priority

      switch(status){
        case 0x02: // Bump switch 1
            Motor_BackwardSimple(500, 100);
            Motor_StopSimple(100);
            Motor_LeftSimple(500, 50);
            Motor_StopSimple(100);
          break;
        case 0x06: // Bump switch 2
            Motor_BackwardSimple(500, 100);
            Motor_StopSimple(100);
            Motor_LeftSimple(500, 100);
            Motor_StopSimple(100);
          break;
        case 0x08: // Bump switch 3
            Motor_BackwardSimple(500, 100);
            Motor_StopSimple(100);
            Motor_LeftSimple(500, 150);
            Motor_StopSimple(100);
          break;
        case 0x0C: // Bump switch 4
            Motor_BackwardSimple(500, 100);
            Motor_StopSimple(100);
            Motor_RightSimple(500, 150);
            Motor_StopSimple(100);
          break;
        case 0x0E: // Bump switch 5
            Motor_BackwardSimple(500, 100);
            Motor_StopSimple(100);
            Motor_RightSimple(500, 100);
            Motor_StopSimple(100);
          break;
        case 0x10: // Bump switch 6
            Motor_BackwardSimple(500, 100);
            Motor_StopSimple(100);
            Motor_RightSimple(500, 50);
            Motor_StopSimple(100);
          break;
        case 0xED: // neither switch pressed

          break;
      }
      P4->IFG &= ~0xED; // clear flag
}

// Read current state of 6 switches
// Returns a 6-bit positive logic result (0 to 63)
// bit 5 Bump5
// bit 4 Bump4
// bit 3 Bump3
// bit 2 Bump2
// bit 1 Bump1
// bit 0 Bump0
uint8_t Bump_Read_Input(void){
  return (P4->IN&0xED); // read P4.7, 4.6, 4.5, 4.3, 4.2, 4.0 inputs
}

// Version 2 version is friendly
void Port1_Init2(void){
  P1->SEL0 &= ~0xD3;
  P1->SEL1 &= ~0xD3;   // 1) configure as GPIO
  P1->DIR &= ~0x12;    // 2) make P1.4 and P1.1 in
  P1->DIR |= 0x01;     // 2) make P1.0 out
  P1->DIR |= 0x00;      //   Motor Forward  (Module 12.2) ////
  P1->REN |= 0x12;     // 3) enable pull resistors on P1.4 and P1.1
  P1->OUT |= 0xD2;     //    P1.4 and P1.1 are pull-up

  //////////////////////////////////////
}

void Port2_Init2(void){
  P2->SEL0 &= ~0xC7;
  P2->SEL1 &= ~0xC7;    // 1) configure P2.2 P2.1 P2.0, and P2.6 P2.7 as GPIO
  P2->DIR |= 0x07;      // 2) make P2.2-P2.0 out
  P2->DS |= 0x07;       // 3) activate increased drive strength
  P2->OUT &= ~0x07;     //    all LEDs off
  P2->DIR |= 0xC0;      //    Motor
}

void Port2_Output2(uint8_t data){  // write three outputs bits of P2
  P2->OUT = (P2->OUT&0xF8)|data;
}

void SysTick_Wait1ms(uint32_t delay){
    // 48000 = 1ms
    // 4800 = 100us
    // 480 = 10us <--- the lowest you can see in TExaS
    // 48 = 1us <--- the lowest you can go
    uint32_t i;
    for(i=0; i<delay; i++){
      SysTick_Wait(48000);  // wait 1ms (assumes 48 MHz clock)
    }
}

int main(void){

    uint32_t H,L;
    uint16_t duty;

  Clock_Init48MHz();

  SysTick_Init();

  EdgeTrigger_Init();              // initialize P1.4 and P1.1 interrupts

  BumpEdgeTrigger_Init();

  Port1_Init2();                    // initialize P1.1 and P1.4 and make them inputs (P1.1 and P1.4 built-in buttons)
                                    // initialize P1.0 as output to red LED
  Port2_Init2();                    // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in LEDs)

  EnableInterrupts();                // clear the I bit

  while(1){
      Motor_ForwardSimple(500, 100000);
  }
}

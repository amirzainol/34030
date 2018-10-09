/*
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

// Modified by: Mohd A. Zainol

#include <stdint.h>
#include "msp.h"
#include "Clock.h"
#include "SysTick.h"
#include "CortexM.h"

// Color    LED(s) Port2
// dark     ---    0
// blue     --B    0x04
// green    -G-    0x02
// sky blue -GB    0x06
// red      R--    0x01
// pink     R-B    0x05
// yellow   RG-    0x03
// white    RGB    0x07
#define RED       0x01
#define GREEN     0x02
#define YELLOW    0x03
#define BLUE      0x04
#define PINK      0x05
#define SKYBLUE   0x06
#define WHITE     0x07

// Initialize Bump sensors using interrupt
// Make six from Port 4 input pins
// Activate interface pull-up
// The pins are P4.7, 4.6, 4.5, 4.3, 4.2, 4.0
void BumpEdgeTrigger_Init(void){
    P4->SEL0 &= ~0xED;
    P4->SEL1 &= ~0xED;      // configure as GPIO
    P4->DIR &= ~0xED;       // make in
    P4->REN |= 0xED;        // enable pull resistors
    P4->OUT |= 0xED;        // pull-up
    P4->IES |= 0xED;        // falling edge event
    P4->IFG &= ~0xED;       // clear flag
    P4->IE |= 0xED;         // arm the interrupt

    // priority 2 on port4
    NVIC->IP[9] = (NVIC->IP[9]&0xFF00FFFF)|0x00400000;
    // enable interrupt 38 in NVIC on port4
    NVIC->ISER[1] = 0x00000040;
}

// Uses P4IV IRQ handler to solve critical section/race
void PORT4_IRQHandler(void){

    uint8_t status;

    // The movement for coloured LED
    // WHITE:   Forward
    // BLUE:    Turn right
    // YELLOW:  Turn left
    // GREEN:   Backward

    // Interrupt Vector of Port4
      status = P4->IV; // 2*(n+1) where n is highest priority

      switch(status){
        case 0x02: // Bump switch 1
            // Move backward at 500 duty for 1000ms
            Motor_BackwardSimple(500, 100);
            // Stop for 1000ms
            Motor_StopSimple(100);
            // Make a left turn at 500 duty for 500ms
            Motor_LeftSimple(500, 50);
            // Stop for 1000ms
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

void Port1_Init(void){
  P1->SEL0 &= ~0xD3;
  P1->SEL1 &= ~0xD3;        // configure as GPIO
  P1->DIR |= 0x01;          // make P1.0 out, the built-in LED1
  P1->DIR |= 0x00;          // Motor Forward which is in Module 12.2
  P1->OUT |= 0xD2;          // P1.4 and P1.1 are pull-up
}

void Port2_Init(void){
    P2->SEL0 &= ~0xC7;
    P2->SEL1 &= ~0xC7;        // configure P2.2 P2.1 P2.0, and P2.6 P2.7 as GPIO
    P2->DIR |= 0x07;          // make P2.2-P2.0 out
    P2->DS |= 0x07;           // activate increased drive strength
    P2->OUT &= ~0x07;         // all LEDs off
    P2->DIR |= 0xC0;          // Dirction of the motor
}

void Port2_Output(uint8_t data){
    // built-in red LED connected to P2.0
    // built-in green LED connected to P2.1
    // built-in blue LED connected to P2.2
    // write three outputs bits of P2
    P2->OUT = (P2->OUT&0xF8)|data;
}

void Switch_Init(void){
    // negative logic built-in Button 1 connected to P1.1
    // negative logic built-in Button 2 connected to P1.4
    P1->SEL0 &= ~0x12;
    P1->SEL1 &= ~0x12;      // configure P1.4 and P1.1 as GPIO
    P1->DIR &= ~0x12;       // make P1.4 and P1.1 in
    P1->REN |= 0x12;        // enable pull resistors on P1.4 and P1.1
    P1->OUT |= 0x12;        // P1.4 and P1.1 are pull-up
}

// bit-banded addresses, positive logic
#define SW2IN ((*((volatile uint8_t *)(0x42098010)))^1)

int main(void){

  Clock_Init48MHz();
  Switch_Init();
  SysTick_Init();

  // waiting for SW2 switch (on the right side of the LaunchPad board)
  while(!SW2IN){
      SysTick_Wait10ms(1);
  }

  BumpEdgeTrigger_Init();

  // initialize P1.1 and P1.4 and make them inputs (P1.1 and P1.4 built-in buttons)
  Port1_Init();

  // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in LEDs)
  Port2_Init();

  EnableInterrupts();       // clear the I bit

  // Run forever
  while(1){
      Motor_ForwardSimple(500, 100000);
  }
}

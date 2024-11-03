/**********************************************************************************
*
*   MPS_Lab02:   MSP430 Assembler code example
*   LED1 at pin p1.0 (bit0 of port1) blinks
*
*   Toggle pin p1.0
*
*   LED1 at P1.0
*
*   Quartz: -
*
*   ACLK = n/a, MCLK = SMCLK = default DCO
*
*   Note:
*   The "volatile" keyword indicates that a value may change between different accesses,
*   even if it does not appear to be modified.
*   This keyword prevents an optimizing compiler from optimizing away subsequent reads
*   or writes and thus incorrectly reusing a stale value or omitting writes.
*
***********************************************************************************/


#include <msp430.h>

//Main function
int main(void)
{
    //configure watchdog Time control: STOP to prevent unwanted resets
    WDTCTL = WDTPW | WDTHOLD;

    //configure pins on port 1 to 0x00
    //Select default digital I/O
    P1SEL = 0x00;
    P1SEL2 = 0x00;

    //Select direction : pin 1.6 connected to the LED       -> OUT PUT -> high [1]
    //                   pin 1.3 connected to the button    -> IN  PUT -> high [0]
    P1DIR = 0x40;        //Direct pin 1.6 to out put and 1.3 to input

    //Enable pull-up/-down register on pin P1.3
    P1REN = 0x08;

    //Set pull-up resistor on P1.3
    P1OUT = 0x08;           //Button released then P1OUT = 0x00

    //ISR (Interrupt service routine)
    P1SEL &= 0x08;          //Select rising edge: trigger interruption when button released
    P1IFG &= ~0x08;         //Clear previous interrupts flags
    P1IE |= 0x08;           //Setting pin P1.3 to interrupt : Enabling Interrupt

    TA0CCR0 = 32768;           // Timer A0 count up to this value (1 second at 32kHz)
        TA0CTL = TASSEL_1 + MC_1;  // ACLK, up mode
        TA0CCTL0 = CCIE;

    __bis_SR_register(GIE);    // Enable global interrupts

    //condition to light up the LED
    while(1)
    {
        if ( (P1IN & 0x08) == 0 )
        {
            P1OUT = 0x40;
        }
        else
        {
            P1OUT = 0x00;
        }
    }
}

//Start interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if(P1IFG & BIT3)
    {
        P1OUT |= 0x40; //toggle P1.6
        TA0CTL |= TASSEL_1 + MC_1;
        P1IFG &= ~BIT3;
    }
}

// Timer A0 interrupt service routine (ISR)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    P1OUT &= ~0x40;           // Turn off P1.6 (LED)
    TA0CTL &= ~MC_1;          // Stop Timer A0

}

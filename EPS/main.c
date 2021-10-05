/******************************************************************************

:::====  :::==== :::====    Copyright (C) 2021 Death Star Development, LLC
 :::  === :::     :::  ===  All rights reserved.  This file contains
 ===  ===  =====  ===  ===  confidential and proprietary information of
 ===  ===     === ===  ===  Death Star Development, LLC and is protected
 =======  ======  =======   under U.S. and international copyright and
other intellectual property laws.  Distribution of this file requires
written consent from Death Star Development, LLC legal team.  Any person or
entity that is found to be re-distributing this material will be subject to
criminal and civil prosecution.  DO NOT TAKE CREDIT FOR OUR WORK.

*******************************************************************************

Vendor: Death Star Development, LLC
Date Created: 2021-09-30
Version: 1.0.0

Associated Filename: main.c
Associated Repository: https://git.deathstardev.com/dsd-projects/duplex/
Supported Device(s): UNIX

*******************************************************************************

Disclaimer:

      This disclaimer is not a license and does not grant any rights to
      the materials distributed herewith. Except as otherwise provided in
      a valid license issued to you by DSD, and to the maximum extent
      permitted by applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE
      "AS IS" AND WITH ALL FAULTS, AND DSD HEREBY DISCLAIMS ALL
      WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY,
      INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
      NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
      (2) DSD shall not be liable (whether in contract or tort,
      including negligence, or under any other theory of liability) for
      any loss or damage of any kind or nature related to, arising under
      or in connection with these materials, including any direct, or
      any indirect, special, incidental, or consequential loss or damage
      (including loss of data, profits, goodwill, or any type of loss or
      damage suffered as a result of any action brought by a third party)
      even if such damage or loss was reasonably foreseeable or DSD
      had been advised of the possibility of the same.

Critical Applications:

      DSD products are not designed or intended to be fail-safe, or
      for use in any application requiring fail-safe performance, such as
      life-support or safety devices or systems, Class III medical
      devices, nuclear facilities, applications related to the deployment
      of airbags, or any other applications that could lead to death,
      personal injury, or severe property or environmental damage
      (individually and collectively, "Critical Applications"). Customer
      assumes the sole risk and liability of any use of DSD products
      in Critical Applications, subject only to applicable laws and
      regulations governing limitations on product liability.

THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS
FILE AT ALL TIMES.

*******************************************************************************

Please refer to the documentation at
https://git.deathstardev.com/dsd-projects/eps/
for the following topics:

1. Revision History
2. Overview
3. Software Tools and System Requirements
4. Installation and Operating Instructions
5. Demo Instructions
6. Design Files
7. Other Information

Access is limited to those with specific rights as a Death Star Development
employee, and those individuals with a need to know.

******************************************************************************/
#include <msp430.h>
#include "uart.h"

/******************************************************************************
 * @brief  Prototype functions
 *****************************************************************************/
void BUSY_init(void);

/******************************************************************************
 * @brief  Main function
 * @param  none
 * @return int
 *****************************************************************************/
int main(void)
{
    /* Stop the watchdog timer */
    WDTCTL = WDTPW | WDTHOLD;

    /* Start UART Operations */
    BUSY_init();
    GPIO_init();
    CLOCK_init();
    UART_init();

    /* Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PM5CTL0 &= ~LOCKLPM5;

    /* Load Memory Constants */
    __delay_cycles(10000000);

    /* Enable Interrupts */
    __bis_SR_register(GIE);

    /* Turn off the BUSY */
    P2OUT ^= BIT7;

    /* Enter the Process Loop */


    __no_operation();
}

/******************************************************************************
 * @brief  BUSY Initialization
 * @param  none
 * @return none
 *****************************************************************************/
void BUSY_init(void) {
    /* Turn on BUSY */
    P2DIR |= BIT7;
    P2OUT |= BIT7;
}

/******************************************************************************
 * @brief  UART Interrupt
 * @param  none
 * @return none
 *****************************************************************************/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    #pragma vector=USCI_A0_VECTOR
    __interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
    void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
    #error Compiler not supported!
#endif
{
  switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
      while(!(UCA0IFG&UCTXIFG));
      UCA0TXBUF = UCA0RXBUF;                                                    // Link to main functions
      __no_operation();
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}

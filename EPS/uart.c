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

Associated Filename: uart.c
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
 * @brief  Configures UART
 * @param  none
 * @return none
 *****************************************************************************/

void UART_init(void) {
    /* Put the eUSCI in reset */
    UCA0CTLW0 = UCSWRST;

    /* Configure Baud Rate
     * Use Table 30-5 in Family User Guide
     */
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BR0 = 26;
    UCA0BR1 = 0;
    UCA0MCTLW |= UCOS16 | UCBRF_0 | 0xB600;

    /* Initialize eUSCI */
    UCA0CTLW0 &= ~UCSWRST;

    /* Enable USCI_A0 RX Interrupt */
    UCA0IE |= UCRXIE;
}

void GPIO_init(void) {
    /* Configure UART */
    P2SEL0 |= BIT0 | BIT1;
    P2SEL1 &= ~(BIT0 | BIT1);

    /* Configure PJ.5 PJ.4 for external crystal oscillator */
    PJSEL0 |= BIT4 | BIT5;
}

void CLOCK_init(void) {
    /* Configure one FRAM waitstate as required by the device datasheet for MCLK
     * operation beyond 8MHz _before_ configuring the clock system.
     */
    FRCTL0 = FRCTLPW | NWAITS_1;

    /* Clock System setup */
    CSCTL0_H = CSKEY_H;
    CSCTL1 = DCOFSEL_0;
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__4 | DIVS__4 | DIVM__4;
    CSCTL1 = DCOFSEL_4 | DCORSEL;
    __delay_cycles(60);
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;
    CSCTL4 &= ~LFXTOFF;
    do {
        CSCTL5 &= ~LFXTOFFG;
        SFRIFG1 &= ~OFIFG;
    } while(SFRIFG1&OFIFG);

    CSCTL0_H = 0;

}

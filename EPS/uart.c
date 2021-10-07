/******************************************************************************
 *   Includes
 *****************************************************************************/
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

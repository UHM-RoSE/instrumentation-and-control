#include <msp430.h>
#include <stdint.h>
#include "uart.h"

/******************************************************************************
 * @brief  Prototype functions
 *****************************************************************************/
void BUSY_init(void);

/******************************************************************************
 * @brief  Main function
 * @param  none
 * @return none
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

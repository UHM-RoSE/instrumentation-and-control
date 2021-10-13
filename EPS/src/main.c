/******************************************************************************
 *   Includes
 *****************************************************************************/
#include <msp430.h>
#include "uart.h"

/******************************************************************************
 *   Definitions
 *****************************************************************************/
#define BOARD_STATUS                          0x01
#define GET_LAST_ERROR                        0x03
#define GET_VERSION                           0x04
#define GET_TELEMETRY                         0x10
#define GET_WATCHDOG                          0x20
#define SET_WATCHDOG                          0x21
#define RESET_WATCHDOG                        0x22
#define ALL_PDMS_ON                           0x40
#define ALL_PDMS_OFF                          0x41
#define GET_PDMS_ACTUAL                       0x42
#define GET_PDMS_EXPECTED                     0x43
#define SET_PDM_ON                            0x50
#define SET_PDM_OFF                           0x51
#define EPS_RESET                             0x80

/******************************************************************************
 * @brief  Prototype functions
 *****************************************************************************/
void boardStatus(void);
void getLastError(void);
void getVersion(void);
void getTelemetry(unsigned char value_l, unsigned char value_h);
void telemetryE1(unsigned char value_l);
void telemetryE2(unsigned char value_l);
void telemetryE3(unsigned char value_l);
void telemetryE4(unsigned char value_l);
void getWatchdog(void);
void setWatchdog(unsigned char value_);
void resetWatchdog(void);
void allPDMsOn(void);
void allPDMsOff(void);
void getPDMsActual(void);
void getPDMsExpected(void);
void setPDMOn(unsigned char value_);
void setPDMOff(unsigned char value_);
void epsReset(void);
void uartTx(unsigned char* buffer_, int size);
void SYS_init(void);

/******************************************************************************
 * @brief  Global Variables
 *****************************************************************************/
unsigned int rx_count = 0;
unsigned char rx_buffer[3] = {0x00, 0x00, 0x00};
unsigned char two_byte[2] = {0x00, 0x00};
unsigned char four_byte[4] = {0x00, 0x00, 0x00, 0x00};

unsigned char board_status[2] = {0x00, 0x00};
unsigned char last_error[2] = {0x00, 0x00};
unsigned char version[2] = {0x10, 0x00};

unsigned char vbmm1[2] = {0x00, 0x00};
unsigned char ibmm1[2] = {0x00, 0x00};
unsigned char vbmm2[2] = {0x00, 0x00};
unsigned char ibmm2[2] = {0x00, 0x00};
unsigned char vbmm3[2] = {0x00, 0x00};
unsigned char ibmm3[2] = {0x00, 0x00};

unsigned char iidiode_out[2] = {0x00, 0x00};
unsigned char vidiode_out[2] = {0x00, 0x00};
unsigned char i3v3_drw[2] = {0x00, 0x00};
unsigned char i5v_drw[2] = {0x00, 0x00};

unsigned char ipcm12v[2] = {0x00, 0x00};
unsigned char vpcm12v[2] = {0x00, 0x00};
unsigned char ipcmbatv[2] = {0x00, 0x00};
unsigned char vpcmbatv[2] = {0x00, 0x00};
unsigned char ipcm5v[2] = {0x00, 0x00};
unsigned char vpcm5v[2] = {0x00, 0x00};
unsigned char ipcm3v3[2] = {0x00, 0x00};
unsigned char vpcm3v3[2] = {0x00, 0x00};

unsigned char vsw1[2] = {0x00, 0x00};
unsigned char isw1[2] = {0x00, 0x00};
unsigned char vsw2[2] = {0x00, 0x00};
unsigned char isw2[2] = {0x00, 0x00};
unsigned char vsw3[2] = {0x00, 0x00};
unsigned char isw3[2] = {0x00, 0x00};
unsigned char vsw4[2] = {0x00, 0x00};
unsigned char isw4[2] = {0x00, 0x00};
unsigned char vsw5[2] = {0x00, 0x00};
unsigned char isw5[2] = {0x00, 0x00};
unsigned char vsw6[2] = {0x00, 0x00};
unsigned char isw6[2] = {0x00, 0x00};
unsigned char vsw7[2] = {0x00, 0x00};
unsigned char isw7[2] = {0x00, 0x00};
unsigned char vsw8[2] = {0x00, 0x00};
unsigned char isw8[2] = {0x00, 0x00};
unsigned char vsw9[2] = {0x00, 0x00};
unsigned char isw9[2] = {0x00, 0x00};
unsigned char vsw10[2] = {0x00, 0x00};
unsigned char isw10[2] = {0x00, 0x00};

unsigned char tbrd[2] = {0x00, 0x00};

unsigned char watchdog_period[2] = {0x00, 0x00};
unsigned char pdm_expected[4] = {0x00, 0x00, 0x00, 0x00};
unsigned char pdm_initial[4] = {0x00, 0x00, 0x00, 0x00};
unsigned char pdm_actual[4] = {0x00, 0x00, 0x00, 0x00};

/******************************************************************************
 * @brief  Main function
 * @param  none
 * @return int
 *****************************************************************************/
int main(void) {
    /* Stop the watchdog timer */
    WDTCTL = WDTPW | WDTHOLD;

    /* Start UART Operations */
    SYS_init();
    GPIO_init();
    CLOCK_init();
    UART_init();

    /* Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PM5CTL0 &= ~LOCKLPM5;

    /* Enable Interrupts */
    __bis_SR_register(GIE);

    /* Enter the Process Loop */
    while(1) {
        //fetch internal status data
    }
}

/******************************************************************************
 * @brief  Board Status
 * @param  none
 * @return none
 *****************************************************************************/
void boardStatus(void) {
    uartTx(board_status, 2);
}

/******************************************************************************
 * @brief  Get Last Error
 * @param  none
 * @return none
 *****************************************************************************/
void getLastError(void) {
    uartTx(last_error, 2);
}

/******************************************************************************
 * @brief  Get Version
 * @param  none
 * @return none
 *****************************************************************************/
void getVersion(void) {
    uartTx(version, 2);
}

/******************************************************************************
 * @brief  Get Telemetry
 * @param  none
 * @return none
 *****************************************************************************/
void getTelemetry(unsigned char value_l, unsigned char value_h) {
    switch(value_h) {
        case 0xE1:
            telemetryE1(value_l);
            break;
        case 0xE2:
            telemetryE2(value_l);
            break;
        case 0xE3:
            telemetryE3(value_l);
            break;
        case 0xE4:
            telemetryE4(value_l);
            break;
    }
}

/******************************************************************************
 * @brief  Telemetry E1
 * @param  none
 * @return none
 *****************************************************************************/
void telemetryE1(unsigned char value_l) {
    switch(value_l) {
        case 0x10:
            uartTx(vbmm1, 2);
            break;
        case 0x14:
            uartTx(ibmm1, 2);
            break;
        case 0x20:
            uartTx(vbmm2, 2);
            break;
        case 0x24:
            uartTx(ibmm2, 2);
            break;
        case 0x30:
            uartTx(vbmm3, 2);
            break;
        case 0x34:
            uartTx(ibmm3, 2);
            break;
        default:
            board_status[0] = board_status[0] | 0x01;
            last_error[0] = 0x13;
            break;
    }
}

/******************************************************************************
 * @brief  Telemetry E2
 * @param  none
 * @return none
 *****************************************************************************/
void telemetryE2(unsigned char value_l) {
    switch(value_l) {
        case 0x84:
            uartTx(iidiode_out, 2);
            break;
        case 0x80:
            uartTx(vidiode_out, 2);
            break;
        case 0x05:
            uartTx(i3v3_drw, 2);
            break;
        case 0x15:
            uartTx(i5v_drw, 2);
            break;
        case 0x34:
            uartTx(ipcm12v, 2);
            break;
        case 0x30:
            uartTx(vpcm12v, 2);
            break;
        case 0x24:
            uartTx(ipcmbatv, 2);
            break;
        case 0x20:
            uartTx(vpcmbatv, 2);
            break;
        case 0x14:
            uartTx(ipcm5v, 2);
            break;
        case 0x10:
            uartTx(vpcm5v, 2);
            break;
        case 0x04:
            uartTx(ipcm3v3, 2);
            break;
        case 0x00:
            uartTx(vpcm3v3, 2);
            break;
        default:
            board_status[0] = board_status[0] | 0x01;
            last_error[0] = 0x13;
            break;
    }
}

/******************************************************************************
 * @brief  Telemetry E3
 * @param  none
 * @return none
 *****************************************************************************/
void telemetryE3(unsigned char value_l) {
    if(value_l == 0x08) {
        uartTx(tbrd, 2);
    } else {
        board_status[0] = board_status[0] | 0x01;
        last_error[0] = 0x13;
    }
}

/******************************************************************************
 * @brief  Telemetry E4
 * @param  none
 * @return none
 *****************************************************************************/
void telemetryE4(unsigned char value_l) {
    switch(value_l) {
        case 0x10:
            uartTx(vsw1, 2);
            break;
        case 0x14:
            uartTx(isw1, 2);
            break;
        case 0x20:
            uartTx(vsw2, 2);
            break;
        case 0x24:
            uartTx(isw2, 2);
            break;
        case 0x30:
            uartTx(vsw3, 2);
            break;
        case 0x34:
            uartTx(isw3, 2);
            break;
        case 0x40:
            uartTx(vsw4, 2);
            break;
        case 0x44:
            uartTx(isw4, 2);
            break;
        case 0x50:
            uartTx(vsw5, 2);
            break;
        case 0x54:
            uartTx(isw5, 2);
            break;
        case 0x60:
            uartTx(vsw6, 2);
            break;
        case 0x64:
            uartTx(isw6, 2);
            break;
        case 0x70:
            uartTx(vsw7, 2);
            break;
        case 0x74:
            uartTx(isw7, 2);
            break;
        case 0x80:
            uartTx(vsw8, 2);
            break;
        case 0x84:
            uartTx(isw8, 2);
            break;
        case 0x90:
            uartTx(vsw9, 2);
            break;
        case 0x94:
            uartTx(isw9, 2);
            break;
        case 0xA0:
            uartTx(vsw10, 2);
            break;
        case 0xA4:
            uartTx(isw10, 2);
            break;
        default:
            board_status[0] = board_status[0] | 0x01;
            last_error[0] = 0x13;
            break;
    }
}

/******************************************************************************
 * @brief  Get Watchdog
 * @param  none
 * @return none
 *****************************************************************************/
void getWatchdog(void) {
    uartTx(watchdog_period, 2);
}

/******************************************************************************
 * @brief  Set Watchdog
 * @param  none
 * @return none
 *****************************************************************************/
void setWatchdog(unsigned char value_) {
    //
}

/******************************************************************************
 * @brief  Reset Watchdog
 * @param  none
 * @return none
 *****************************************************************************/
void resetWatchdog(void) {
    //
}

/******************************************************************************
 * @brief  All PDMs ON
 * @param  none
 * @return none
 *****************************************************************************/
void allPDMsOn(void) {
    //
}

/******************************************************************************
 * @brief  All PDMs OFF
 * @param  none
 * @return none
 *****************************************************************************/
void allPDMsOff(void) {
    //
}

/******************************************************************************
 * @brief  Get PDMs Actual State
 * @param  none
 * @return none
 *****************************************************************************/
void getPDMsActual(void) {
    //
}

/******************************************************************************
 * @brief  Get PDMs Expected State
 * @param  none
 * @return none
 *****************************************************************************/
void getPDMsExpected(void) {
    //
}

/******************************************************************************
 * @brief  Set PDM ON
 * @param  none
 * @return none
 *****************************************************************************/
void setPDMOn(unsigned char value_) {
    //
}

/******************************************************************************
 * @brief  Set PDM OFF
 * @param  none
 * @return none
 *****************************************************************************/
void setPDMOff(unsigned char value_) {
    //
}

/******************************************************************************
 * @brief  EPS Reset
 * @param  none
 * @return none
 *****************************************************************************/
void epsReset(void) {
    //
}

/******************************************************************************
 * @brief  UART Tx Send
 * @param  none
 * @return none
 *****************************************************************************/
void uartTx(unsigned char* buffer_, int size) {
    while(size) {
        while(!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = *buffer_;
        buffer_++;
        size--;
    }
    //while(UCA0STAT & UCBUSY);
}

/******************************************************************************
 * @brief  System Initiation
 * @param  none
 * @return int
 *****************************************************************************/
void SYS_init(void) {
    /* Terminate unused GPIOs for Low Power Consumption */
    P1OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P1DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P1REN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);

    P2OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P2DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P2REN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);

    P3OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P3DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P3REN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);

    P4OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P4DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P4REN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);

    P5OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P5DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P5REN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);

    P6OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P6DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P6REN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);

    P7OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P7DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P7REN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);

    P8OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P8DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P8REN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);

    P9OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P9DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P9REN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);

    P10OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P10DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P10REN |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
}

/******************************************************************************
 * @brief  UART Interrupt Service Routine
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
      while(!(UCA0IFG & UCTXIFG)) {
          UCA0IFG = UCA0IFG & (~UCRXIFG);
          if(rx_count == 0) {
              rx_count++;
              rx_buffer[0] = UCA0RXBUF;
          } else if((rx_buffer[0] == GET_TELEMETRY) && (rx_count == 1)) {
              rx_count++;
              rx_buffer[1] = UCA0RXBUF;
          } else {
              if(rx_count == 2) {
                  rx_count = 0;
                  rx_buffer[2] = UCA0RXBUF;
              } else {
                  rx_count = 0;
                  rx_buffer[1] = UCA0RXBUF;
              }
              switch(rx_buffer[0]) {
                  case BOARD_STATUS:
                      boardStatus();
                      break;
                  case GET_LAST_ERROR:
                      getLastError();
                      break;
                  case GET_VERSION:
                      getVersion();
                      break;
                  case GET_TELEMETRY:
                      getTelemetry(rx_buffer[1], rx_buffer[2]);
                      break;
                  case GET_WATCHDOG:
                      getWatchdog();
                      break;
                  case SET_WATCHDOG:
                      setWatchdog(rx_buffer[1]);
                      break;
                  case RESET_WATCHDOG:
                      resetWatchdog();
                      break;
                  case ALL_PDMS_ON:
                      allPDMsOn();
                      break;
                  case ALL_PDMS_OFF:
                      allPDMsOff();
                      break;
                  case GET_PDMS_ACTUAL:
                      getPDMsActual();
                      break;
                  case GET_PDMS_EXPECTED:
                      getPDMsExpected();
                      break;
                  case SET_PDM_ON:
                      setPDMOn(rx_buffer[1]);
                      break;
                  case SET_PDM_OFF:
                      setPDMOff(rx_buffer[1]);
                      break;
                  case EPS_RESET:
                      epsReset();
                      break;
                  default:
                      board_status[0] = board_status[0] | 0x01;
                      last_error[0] = 0x13;
                      break;
              }
              rx_buffer[0] = 0x00;
              rx_buffer[1] = 0x00;
          }
      }
      __no_operation();
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}

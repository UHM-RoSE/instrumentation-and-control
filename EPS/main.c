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
void BUSY_init(void);

/******************************************************************************
 * @brief  Global Variables
 *****************************************************************************/
unsigned int rx_count = 0;
unsigned char* rx_buffer = [0x00, 0x00, 0x00];
unsigned char* two_byte = [0x00, 0x00];
unsigned char* four_byte = [0x00, 0x00, 0x00, 0x00];

unsigned char* board_status = [0x00, 0x00];
unsigned char* last_error = [0x00, 0x00];
unsigned char* version = [0x01, 0x00];

unsigned char* vbmm1 = [0x00, 0x00];
unsigned char* ibmm1 = [0x00, 0x00];
unsigned char* vbmm2 = [0x00, 0x00];
unsigned char* ibmm2 = [0x00, 0x00];
unsigned char* vbmm3 = [0x00, 0x00];
unsigned char* ibmm3 = [0x00, 0x00];

unsigned char* iidiode_out = [0x00, 0x00];
unsigned char* vidiode_out = [0x00, 0x00];
unsigned char* i3v3_drw = [0x00, 0x00];
unsigned char* i5v_drw = [0x00, 0x00];

unsigned char* ipcm12v = [0x00, 0x00];
unsigned char* vpcm12v = [0x00, 0x00];
unsigned char* ipcmbatv = [0x00, 0x00];
unsigned char* vpcmbatv = [0x00, 0x00];
unsigned char* ipcm5v = [0x00, 0x00];
unsigned char* vpcm5v = [0x00, 0x00];
unsigned char* ipcm3v3 = [0x00, 0x00];
unsigned char* vpcm3v3 = [0x00, 0x00];

unsigned char* vsw1 = [0x00, 0x00];
unsigned char* isw1 = [0x00, 0x00];
unsigned char* vsw2 = [0x00, 0x00];
unsigned char* isw2 = [0x00, 0x00];
unsigned char* vsw3 = [0x00, 0x00];
unsigned char* isw3 = [0x00, 0x00];
unsigned char* vsw4 = [0x00, 0x00];
unsigned char* isw4 = [0x00, 0x00];
unsigned char* vsw5 = [0x00, 0x00];
unsigned char* isw5 = [0x00, 0x00];
unsigned char* vsw6 = [0x00, 0x00];
unsigned char* isw6 = [0x00, 0x00];
unsigned char* vsw7 = [0x00, 0x00];
unsigned char* isw7 = [0x00, 0x00];
unsigned char* vsw8 = [0x00, 0x00];
unsigned char* isw8 = [0x00, 0x00];
unsigned char* vsw9 = [0x00, 0x00];
unsigned char* isw9 = [0x00, 0x00];
unsigned char* vsw10 = [0x00, 0x00];
unsigned char* isw10 = [0x00, 0x00];

unsigned char* tbrd = [0x00, 0x00];

unsigned char* watchdog_period = [0x00, 0x00];
unsigned char* pdm_expected = [0x00, 0x00, 0x00, 0x00];
unsigned char* pdm_initial = [0x00, 0x00, 0x00, 0x00];
unsigned char* pdm_actual = [0x00, 0x00, 0x00, 0x00];

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

    /* Enable Interrupts */
    __bis_SR_register(GIE);

    /* Enter the Process Loop */
    while(1) {
        //fetch internal status data
    }

    return 0;
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
void getTelemetry(value_l, value_h) {
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
void telemetryE1(value_l) {
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
void telemetryE2(value_l) {
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
void telemetryE3(value_l) {
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
void telemetryE4(value_l) {
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
void setWatchdog(value_) {
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
void setPDMOn(value_) {
    //
}

/******************************************************************************
 * @brief  Set PDM OFF
 * @param  none
 * @return none
 *****************************************************************************/
void setPDMOff(value_) {
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
 * @brief  BUSY Initialization
 * @param  none
 * @return none
 *****************************************************************************/
void uartTx(unsigned char* buffer_, int size) {
    while(size) {
        while(!(UCA1IFG & UCTXIFG));
        UCA0TXBUF = *buffer_;
        buffer_++;
        size--;
    }
    while(UCA0STAT & UCBUSY);
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

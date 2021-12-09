/******************************************************************************
 *   Includes
 *****************************************************************************/
#include <msp430.h>
#include <stdint.h>
#include "uart.h"

/******************************************************************************
 *   Definitions
 *****************************************************************************/
#define I2C_ADDR                              0x20
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
#define MAX_I2C_BUFFER_SIZE                     20
#define CMD_TYPE_0_SLAVE                        0
#define CMD_TYPE_1_SLAVE                        1
#define CMD_TYPE_2_SLAVE                        2
#define CMD_TYPE_0_MASTER                       3
#define CMD_TYPE_1_MASTER                       4
#define CMD_TYPE_2_MASTER                       5
#define TYPE_0_LENGTH                           1
#define TYPE_1_LENGTH                           2
#define TYPE_2_LENGTH                           6

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
void I2C_init(void);
void I2C_Slave_ProcessCMD(unsigned char cmd);
void I2C_Slave_TransactionDone(unsigned char cmd);
void CopyArray(unsigned char *source, unsigned char *dest, unsigned char count);

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

unsigned char MasterType2 [TYPE_2_LENGTH] = {0};
unsigned char MasterType1 [TYPE_1_LENGTH] = { 0, 0};
unsigned char MasterType0 [TYPE_0_LENGTH] = { 0};

unsigned char SlaveType2 [TYPE_2_LENGTH] = {'A', 'B', 'C', 'D', '1', '2'};
unsigned char SlaveType1 [TYPE_1_LENGTH] = {15, 16};
unsigned char SlaveType0 [TYPE_0_LENGTH] = {12};

unsigned char ReceiveRegAddr = 0;
unsigned char ReceiveBuffer[MAX_I2C_BUFFER_SIZE] = {0};
unsigned char RXByteCtr = 0;
unsigned char ReceiveIndex = 0;
unsigned char TransmitBuffer[MAX_I2C_BUFFER_SIZE] = {0};
unsigned char TXByteCtr = 0;
unsigned char TransmitIndex = 0;

typedef enum I2C_ModeEnum {
    IDLE_MODE,
    NACK_MODE,
    TX_REG_ADDRESS_MODE,
    RX_REG_ADDRESS_MODE,
    TX_DATA_MODE,
    RX_DATA_MODE,
    SWITCH_TO_RX_MODE,
    SWITCH_TO_TX_MODE,
    TIMEOUT_MODE
} I2C_Mode;

I2C_Mode SlaveMode = RX_REG_ADDRESS_MODE;

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
    I2C_init();
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
 * @brief  I2C Initiation
 * @param  none
 * @return int
 *****************************************************************************/
void I2C_init()
{
    UCB0CTLW0 = UCSWRST;                                                        // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;                                             // I2C mode, sync mode
    UCB0I2COA0 = I2C_ADDR | UCOAEN;                                             // Own Address and enable
    UCB0CTLW0 &= ~UCSWRST;                                                      // clear reset register
    UCB0IE |= UCRXIE + UCSTPIE;
}

/******************************************************************************
 * @brief  I2C State Machine
 * @param  none
 * @return int
 *****************************************************************************/
void I2C_Slave_ProcessCMD(unsigned char cmd)
{
    ReceiveIndex = 0;
    TransmitIndex = 0;
    RXByteCtr = 0;
    TXByteCtr = 0;

    switch (cmd)
    {
        case (CMD_TYPE_0_SLAVE):                                                //Send slave device id (This device's id)
            SlaveMode = TX_DATA_MODE;
            TXByteCtr = TYPE_0_LENGTH;
            //Fill out the TransmitBuffer
            CopyArray(SlaveType0, TransmitBuffer, TYPE_0_LENGTH);
            UCB0IE &= ~UCRXIE;                                                  // Disable RX interrupt
            UCB0IE |= UCTXIE;                                                   // Enable TX interrupt
            break;
        case (CMD_TYPE_1_SLAVE):                                                //Send slave device time (This device's time)
            SlaveMode = TX_DATA_MODE;
            TXByteCtr = TYPE_1_LENGTH;
            //Fill out the TransmitBuffer
            CopyArray(SlaveType1, TransmitBuffer, TYPE_1_LENGTH);
            UCB0IE &= ~UCRXIE;                                                  // Disable RX interrupt
            UCB0IE |= UCTXIE;                                                   // Enable TX interrupt
            break;
        case (CMD_TYPE_2_SLAVE):                                                //Send slave device location (This device's location)
            SlaveMode = TX_DATA_MODE;
            TXByteCtr = TYPE_2_LENGTH;
            //Fill out the TransmitBuffer
            CopyArray(SlaveType2, TransmitBuffer, TYPE_2_LENGTH);
            UCB0IE &= ~UCRXIE;                                                  // Disable RX interrupt
            UCB0IE |= UCTXIE;                                                   // Enable TX interrupt
            break;
        case (CMD_TYPE_0_MASTER):
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = TYPE_0_LENGTH;
            UCB0IE &= ~UCTXIE;                                                  // Disable RX interrupt
            UCB0IE |= UCRXIE;                                                   // Enable TX interrupt
            break;
        case (CMD_TYPE_1_MASTER):
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = TYPE_1_LENGTH;
            UCB0IE &= ~UCTXIE;                                                  // Disable RX interrupt
            UCB0IE |= UCRXIE;                                                   // Enable TX interrupt
            break;
        case (CMD_TYPE_2_MASTER):
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = TYPE_2_LENGTH;
            UCB0IE &= ~UCTXIE;                                                  // Disable RX interrupt
            UCB0IE |= UCRXIE;                                                   // Enable TX interrupt
            break;
        default:
            __no_operation();
            break;
    }
}


void I2C_Slave_TransactionDone(unsigned char cmd)
{
    switch (cmd)
    {
        case (CMD_TYPE_0_SLAVE):                                                //Slave device id was sent(This device's id)
            break;
        case (CMD_TYPE_1_SLAVE):                                                //Slave device time was sent(This device's time)
            break;
        case (CMD_TYPE_2_SLAVE):                                                //Send slave device location (This device's location)
            break;
        case (CMD_TYPE_0_MASTER):
            CopyArray(ReceiveBuffer, MasterType0, TYPE_0_LENGTH);
            break;
        case (CMD_TYPE_1_MASTER):
            CopyArray(ReceiveBuffer, MasterType1, TYPE_1_LENGTH);
            break;
        case (CMD_TYPE_2_MASTER):
            CopyArray(ReceiveBuffer, MasterType2, TYPE_2_LENGTH);
            break;
        default:
            __no_operation();
            break;
    }
}

void CopyArray(unsigned char *source, unsigned char *dest, unsigned char count)
{
    unsigned char copyIndex = 0;
    for (copyIndex = 0; copyIndex < count; copyIndex++)
    {
        dest[copyIndex] = source[copyIndex];
    }
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

/******************************************************************************
 * @brief  I2C Interrupt Service Routine
 * @param  none
 * @return none
 *****************************************************************************/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  //Must read from UCB0RXBUF
  unsigned char rx_val = 0;
  switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
  {
    case USCI_NONE:          break;                                             // Vector 0: No interrupts
    case USCI_I2C_UCALIFG:   break;                                             // Vector 2: ALIFG
    case USCI_I2C_UCNACKIFG:                                                    // Vector 4: NACKIFG
      break;
    case USCI_I2C_UCSTTIFG:  break;                                             // Vector 6: STTIFG
    case USCI_I2C_UCSTPIFG:
        UCB0IFG &= ~(UCTXIFG0);
        break;                                                                  // Vector 8: STPIFG
    case USCI_I2C_UCRXIFG3:  break;                                             // Vector 10: RXIFG3
    case USCI_I2C_UCTXIFG3:  break;                                             // Vector 12: TXIFG3
    case USCI_I2C_UCRXIFG2:  break;                                             // Vector 14: RXIFG2
    case USCI_I2C_UCTXIFG2:  break;                                             // Vector 16: TXIFG2
    case USCI_I2C_UCRXIFG1:  break;                                             // Vector 18: RXIFG1
    case USCI_I2C_UCTXIFG1:  break;                                             // Vector 20: TXIFG1
    case USCI_I2C_UCRXIFG0:                                                     // Vector 22: RXIFG0
        rx_val = UCB0RXBUF;
        switch (SlaveMode)
        {
          case (RX_REG_ADDRESS_MODE):
              ReceiveRegAddr = rx_val;
              I2C_Slave_ProcessCMD(ReceiveRegAddr);
              break;
          case (RX_DATA_MODE):
              ReceiveBuffer[ReceiveIndex++] = rx_val;
              RXByteCtr--;
              if (RXByteCtr == 0)
              {
                  //Done Receiving MSG
                  SlaveMode = RX_REG_ADDRESS_MODE;
                  UCB0IE &= ~(UCTXIE);
                  UCB0IE |= UCRXIE;                                             // Enable RX interrupt
                  I2C_Slave_TransactionDone(ReceiveRegAddr);
              }
              break;
          default:
              __no_operation();
              break;
        }
        break;
    case USCI_I2C_UCTXIFG0:                                                     // Vector 24: TXIFG0
        switch (SlaveMode)
        {
          case (TX_DATA_MODE):
              UCB0TXBUF = TransmitBuffer[TransmitIndex++];
              TXByteCtr--;
              if (TXByteCtr == 0)
              {
                  //Done Transmitting MSG
                  SlaveMode = RX_REG_ADDRESS_MODE;
                  UCB0IE &= ~(UCTXIE);
                  UCB0IE |= UCRXIE;                                             // Enable RX interrupt
                  I2C_Slave_TransactionDone(ReceiveRegAddr);
              }
              break;
          default:
              __no_operation();
              break;
        }
        break;                                                                  // Interrupt Vector: I2C Mode: UCTXIFG
    default: break;
  }
}


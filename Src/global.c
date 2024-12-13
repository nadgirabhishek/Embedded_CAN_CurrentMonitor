/*
 * global.c
 *
 *  Created on: Nov 4, 2024
 *      Author: Abhishek Nadgir
 */


#include "global.h"
volatile uint32_t tick_counter = 0;
volatile bool read_all_flag = false;
#define ID_SHUNT_VOLTAGE    0x100
#define ID_BUS_VOLTAGE      0x101
#define ID_CURRENT          0x102
#define ID_POWER            0x103

#ifdef DEBUG
#define ID_TEST_DATA        0x600
#define ID_ECHO_TEST_DATA   0x601
#endif
/*
 * ISR FOR SYSTICK
 *
 * Parameters:
 *  None
 *
 * Returns:
 *   None
 */
void SysTick_Handler(void) {
    tick_counter++;
    read_all_flag = true;

}
/*
 * ISR for USART2
 *
 * Parameters:
 * None
 *
 * Returns:
 *   None
 */
void USART2_IRQHandler(void) {
	// Check if RXNE flag is set (data received)
	if (USART2->ISR & USART_ISR_RXNE) {
		char newval = USART2->RDR; // Read the received character
		cbfifo_enqueue(&fiforx, &newval, sizeof(newval));
	}
	if (USART2->ISR & USART_ISR_TXE) {
		char newval; // Read the received character
		if (cbfifo_dequeue(&fifotx, &newval, sizeof(newval)) > 0) {
			if (newval == '\n') {
				USART2->TDR = '\r';
				while (!(USART2->ISR & USART_ISR_TXE))
					;
				USART2->TDR = '\n';
			} else {
				USART2->TDR = newval; // Send the character
			}
		} else {
			USART2->CR1 &= (~(USART_CR1_TXEIE));
		}
	}
}
/*
 * ISR for CAN Reception
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   None
 */
void CEC_CAN_IRQHandler(void) {
    static int count = 0;
    count++;

    // Check FIFO 0 for pending messages
    if (CAN->RF0R & CAN_RF0R_FMP0) {
        uint16_t id = (CAN->sFIFOMailBox[0].RIR >> CAN_TI0R_STID_Pos) & 0x7FF; // Extract ID
        float value = *((float *)&CAN->sFIFOMailBox[0].RDLR); // Extract float value

        // Process based on ID
        switch (id) {
           case ID_SHUNT_VOLTAGE: // Shunt Voltage
                cbfifo_enqueue_float(&fifosv, value);
                break;

            case ID_BUS_VOLTAGE: // Bus Voltage
                cbfifo_enqueue_float(&fifobv, value);
                break;

            case ID_CURRENT: // Current
                cbfifo_enqueue_float(&fifocu, value);
                break;

            case ID_POWER: // Power
                cbfifo_enqueue_float(&fifopo, value);
                break;
#ifdef DEBUG
            case ID_TEST_DATA: // Test Data Received, echo it back on ID 0x601
                can_transmit_float(value, ID_ECHO_TEST_DATA); // Echo back the received test data
                break;

            case ID_ECHO_TEST_DATA: // Echoed Test Data
                cbfifo_enqueue_float(&fifotest, value); // Store echoed data for validation
                break;
#endif
            default: // Unrecognized ID
                printf("INT: Unrecognized ID: 0x%X, Data: %.2f\n", id, value);
                break;
        }

        CAN->RF0R |= CAN_RF0R_RFOM0; // Release FIFO 0
    }
}


/*
 * can.c
 *
 *  Created on: Dec 4, 2024
 *      Author: Abhishek Nadgir
 */
#include "can.h"

#define LOOPBACKMODE 1
#define NORMALMODE 2
#define CAN_MODE NORMALMODE

#define TEST_BUFFER_SIZE 10
#define TEST_ID_TRANSMIT 0x600 // ID for transmitting test data
#define TEST_ID_RECEIVE 0x601  // ID for receiving echoed test data
#define TRANSMIT_DELAY 100000  // Delay after each transmission


// CAN Filter IDs
#define BASE_ID_100         0x100
#define MASK_ID_100_103     0x7FC

#define BASE_ID_600         0x600
#define MASK_ID_600_601     0x7FE

// CAN Filter Indexes
#define FILTER_INDEX_10     10
#define FILTER_INDEX_1      1

// CAN Bit Positions
#define ID_BIT_POSITION     21

#define CAN_BTR_VAL 0X2F
/*can_initl
 * CAN Initalization
 *
 * Parameters:
 * 	 None.
 *
 * Returns:
 *   None.
 */
void can_init(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable GPIOA clock
	GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);			// Clear
	GPIOB->MODER |= (GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1);// Set AF mode
	GPIOB->AFR[1] |= (4 << GPIO_AFRH_AFSEL8_Pos) | (4 << GPIO_AFRH_AFSEL9_Pos); // AF7 (USART2) for PA2 and PA3
	RCC->APB1ENR |= RCC_APB1ENR_CANEN;
	CAN->MCR |= CAN_MCR_INRQ;
	while (((CAN->MSR) & CAN_MSR_INAK) == 0)
		;
	CAN->MCR |= CAN_MCR_NART | CAN_MCR_TXFP; //auto retransmission disabled priority disabled
	CAN->MCR &= (~CAN_MCR_AWUM); //auto wakeup disabled
	CAN->MCR &= (~CAN_MCR_ABOM); //automatic bus off diabaled
	CAN->BTR &= (~CAN_MCR_TTCM); //time trigger mode disabled
	CAN->BTR &= (~CAN_MCR_RFLM); //receive fifo lock disabled
#if CAN_MODE == LOOPBACKMODE
	CAN->BTR |= CAN_BTR_SILM | CAN_BTR_LBKM; //LOOP BACK MODE
#endif
#if CAN_MODE == NORMALMODE
	CAN->BTR &= (~CAN_BTR_SILM);//NORMAL MODE
	CAN->BTR &= (~CAN_BTR_LBKM); //nORMAL MODE
#endif
	CAN->BTR |= CAN_BTR_VAL;//Buadrate
	CAN->BTR |= CAN_BTR_TS1_1 | CAN_BTR_TS2_0 | CAN_BTR_SJW_0;
	CAN->IER |= CAN_IER_FMPIE0 | CAN_IER_FMPIE1; //MESSAGE PENDING INTERRUPT FOR FIFO 0
	CAN->MCR &= (~CAN_MCR_SLEEP); // ENTERING INTO NORMAL MODE FROM SLEEP
	CAN->MCR &= (~CAN_MCR_INRQ); //DISABLING INITALIZATION MODE
	while (((CAN->MSR) & CAN_MSR_INAK) == 1)
		; //tHIS BIT IS CLEARED WHEN IT ENTERS NORMAL MODE FROM INITALIZATION MODE
	NVIC_EnableIRQ(CEC_CAN_IRQn);
	NVIC_SetPriority(CEC_CAN_IRQn, 1);
	printf("\n\rCAN configuration succesfull\n\r");
	//Initialization mode  for filters enables
	CAN->FMR |= CAN_FMR_FINIT;

	// Configure Filter 10: IDs 0x100-0x103
	CAN->FM1R &= ~CAN_FM1R_FBM10;  // Mask mode
	CAN->FS1R |= CAN_FS1R_FSC10;   // Single 32-bit scale configuration
	CAN->FFA1R &= ~CAN_FFA1R_FFA10; // Assign to FIFO 0
	CAN->FA1R |= CAN_FA1R_FACT10;  // Activate filter 10
	CAN->sFilterRegister[FILTER_INDEX_10].FR1 = (BASE_ID_100 << ID_BIT_POSITION); // Base ID 0x100
	CAN->sFilterRegister[FILTER_INDEX_10].FR2 = (MASK_ID_100_103 << ID_BIT_POSITION); // Mask for IDs 0x100-0x103

	// Configure Filter 1: IDs 0x600 and 0x601
	CAN->FM1R &= ~CAN_FM1R_FBM1;  // Mask mode for filter 1
	CAN->FS1R |= CAN_FS1R_FSC1;   // Single 32-bit scale configuration
	CAN->FFA1R &= ~CAN_FFA1R_FFA1; // Assign to FIFO 0
	CAN->FA1R |= CAN_FA1R_FACT1;  // Activate filter 1
	CAN->sFilterRegister[FILTER_INDEX_1].FR1 = (BASE_ID_600 << ID_BIT_POSITION); // Base ID 0x600
	CAN->sFilterRegister[FILTER_INDEX_1].FR2 = (MASK_ID_600_601 << ID_BIT_POSITION); // Mask for IDs 0x600 and 0x601

	// Exit filter initialization mode
	CAN->FMR &= ~CAN_FMR_FINIT;
	printf("CAN filter configuration succesfull\n\r");
}
/*can_transmit_0
 * CAN Transmission of 8bit data to FIFO 0
 *
 * Parameters:
 * 	 data and id .
 *
 * Returns:
 *   None.
 */
void can_transmit_0(uint8_t data, uint16_t id) {
	while ((CAN->TSR & CAN_TSR_TME0) == 0)
		;
	CAN->sTxMailBox[0].TIR &= (~CAN_TI0R_IDE);
	CAN->sTxMailBox[0].TIR &= (~CAN_TI0R_RTR);
	CAN->sTxMailBox[0].TIR &= (~CAN_TI0R_STID);
	CAN->sTxMailBox[0].TIR &= (~CAN_TI0R_EXID);
	CAN->sTxMailBox[0].TIR |= (id << CAN_TI0R_STID_Pos);
	CAN->sTxMailBox[0].TDTR &= (~CAN_TDT0R_TGT);
	CAN->sTxMailBox[0].TDTR &= (~CAN_TDT0R_DLC);
	CAN->sTxMailBox[0].TDTR |= (1 << CAN_TDT0R_DLC_Pos);
	CAN->sTxMailBox[0].TDLR &= (~CAN_TDL0R_DATA0);
	CAN->sTxMailBox[0].TDLR |= (data << CAN_TDL0R_DATA0_Pos);
	CAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	printf("\n\rCAN : Data 0x%X trasnmitted with ID 0x%X.\n\r", data, id);
}
/*can_transmit_16bit_0
 * CAN Transmission of 16bit data to FIFO 0
 *
 * Parameters:
 * 	 data and id .
 *
 * Returns:
 *   None.
 */
void can_transmit_16bit_0(uint16_t data, uint16_t id) {
	// Wait until mailbox 0 is empty
	while ((CAN->TSR & CAN_TSR_TME0) == 0)
		;

	// Clear and configure the Identifier fields (Standard ID, no extended ID)
	CAN->sTxMailBox[0].TIR &= ~CAN_TI0R_IDE;       // Standard ID
	CAN->sTxMailBox[0].TIR &= ~CAN_TI0R_RTR;    // Data frame (not remote frame)
	CAN->sTxMailBox[0].TIR &= ~CAN_TI0R_STID;      // Clear previous ID
	CAN->sTxMailBox[0].TIR |= (id << CAN_TI0R_STID_Pos); // Set new Standard ID

	// Configure DLC (Data Length Code) for 2 bytes (16 bits)
	CAN->sTxMailBox[0].TDTR &= ~CAN_TDT0R_DLC;
	CAN->sTxMailBox[0].TDTR |= (2 << CAN_TDT0R_DLC_Pos); // DLC = 2 bytes

	// Split 16-bit data into two 8-bit bytes and load into data registers
	CAN->sTxMailBox[0].TDLR &= ~CAN_TDL0R_DATA0; // Clear DATA0
	CAN->sTxMailBox[0].TDLR &= ~CAN_TDL0R_DATA1; // Clear DATA1
	CAN->sTxMailBox[0].TDLR |= ((data & 0xFF) << CAN_TDL0R_DATA0_Pos); // Lower 8 bits in DATA0
	CAN->sTxMailBox[0].TDLR |= (((data >> 8) & 0xFF) << CAN_TDL0R_DATA1_Pos); // Upper 8 bits in DATA1

	// Request transmission
	CAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;

	// Print transmission info
	printf("\n\rCAN: 16-bit Data 0x%04X transmitted with ID 0x%X.\n\r", data,
			id);
}
/*can_transmit_0
 * CAN Transmission of float data to fifo 0
 *
 * Parameters:
 * 	 data and id .
 *
 * Returns:
 *   None.
 */
void can_transmit_float(float data, uint16_t id) {
	// Wait until mailbox 0 is empty
	while ((CAN->TSR & CAN_TSR_TME0) == 0)
		;

	// Clear and configure the Identifier fields (Standard ID, no extended ID)
	CAN->sTxMailBox[0].TIR &= ~CAN_TI0R_IDE;       // Standard ID
	CAN->sTxMailBox[0].TIR &= ~CAN_TI0R_RTR;    // Data frame (not remote frame)
	CAN->sTxMailBox[0].TIR &= ~CAN_TI0R_STID;      // Clear previous ID
	CAN->sTxMailBox[0].TIR |= (id << CAN_TI0R_STID_Pos); // Set new Standard ID

	// Configure DLC (Data Length Code) for 4 bytes (float size)
	CAN->sTxMailBox[0].TDTR &= ~CAN_TDT0R_DLC;
	CAN->sTxMailBox[0].TDTR |= (4 << CAN_TDT0R_DLC_Pos); // DLC = 4 bytes

	CAN->sTxMailBox[0].TDLR &= ~CAN_TDL0R_DATA0; // Clear DATA0
	CAN->sTxMailBox[0].TDLR &= ~CAN_TDL0R_DATA1; // Clear DATA1
	CAN->sTxMailBox[0].TDLR &= ~CAN_TDL0R_DATA2; // Clear DATA0
	CAN->sTxMailBox[0].TDLR &= ~CAN_TDL0R_DATA3; // Clear DATA1
	// Convert float to 4 bytes (IEEE 754 format)
	uint8_t *dataBytes = (uint8_t*) &data;

	// Load the float bytes into the data registers
	CAN->sTxMailBox[0].TDLR = (dataBytes[0]) | (dataBytes[1] << 8)
			| (dataBytes[2] << 16) | (dataBytes[3] << 24);

	CAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
}
#ifdef DEBUG
/*can_test_transmit
 * Automatic Testing function to test the CAN communication
 *
 * Parameters:
 * 	 None.
 *
 * Returns:
 *   None.
 */
void can_test_transmit(void) {
	float testData[TEST_BUFFER_SIZE];
	float receivedData[TEST_BUFFER_SIZE];

	// Initialize test data with random float values
	for (int i = 0; i < TEST_BUFFER_SIZE; i++) {
		testData[i] = (float) (rand() % 1000) / 10.0; // Random float values
	}

	// Transmit all test data
	printf("Starting CAN Transmit Test...\n");
	for (int i = 0; i < TEST_BUFFER_SIZE; i++) {
		can_transmit_float(testData[i], TEST_ID_TRANSMIT);

		// Delay to allow receiver to process and echo
		int delay = TRANSMIT_DELAY;
		while (delay-- > 0)
			;
	}

	// Receive and validate echoed data
	printf("Receiving and validating echoed data...\n");
	int receivedCount = 0;
	int timeout = 100000000; // Timeout for receiving all test data
	while (receivedCount < TEST_BUFFER_SIZE && timeout-- > 0) {
		float dequeuedValue;

		// Dequeue echoed data for ID 0x601
		if (cbfifo_dequeue_float(&fifotest, &dequeuedValue)) {
			receivedData[receivedCount] = dequeuedValue;
			receivedCount++;
		}
	}

	if (timeout <= 0) {
		printf("CAN Transmit Test FAILED: Timeout waiting for data.\n");
		assert(0 && "Timeout while receiving data in CAN test");
		return;
	}

	// Validate transmitted vs. received data using assert
	for (int i = 0; i < TEST_BUFFER_SIZE; i++) {
		assert(
				testData[i] == receivedData[i]
						&& "Data mismatch in CAN test: Transmitted and received data do not match");
	}

	printf("CAN Transmit Test PASSED: All data matched.\n");
}
#endif

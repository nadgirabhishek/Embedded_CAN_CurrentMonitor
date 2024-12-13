/*
 * uart_functions.c
 *
 *  Created on: Nov 4, 2024
 *      Author: nadgi
 */

#include "uart_functions.h"
/*
 * Blocking Getchar FUnction
 *
 * Parameters:
 *
 * Returns:
 *   None.
 */
char uart_getchar(void)
{
	while (!(USART2->ISR & USART_ISR_RXNE))
		;
	return USART2->RDR;
}
/*
 * Blocking Putcahr FUnction
 *
 * Parameters:
 *
 * Returns:
 *   None.
 */
void uart_putchar(char data)
{
	while (!(USART2->ISR & USART_ISR_TXE));
	USART2->TDR = data;
}
/*
 * Glue logic to connect UART2 to standard library function getchar
 *
 * Parameters:
 *
 * Returns:
 * either received char or -1 if not
 */
int __io_getchar(void)
{
	char ch;
	if(cbfifo_dequeue(&fiforx, &ch, 1)==1)
	{
		return ch;
	}
	else
	{
		return -1;
	}
}
/*
 * Glue logic to connect UART2 to standrd library function printf.
 *
 * Parameters:
 * ch : charcter to be printed
 *
 * Returns:
 *  either received 1 or -1(if transmission not successful)
 */
int __io_putchar(int ch)
{
	while (cbfifo_length(&fifotx) >= 127);
	if(cbfifo_enqueue(&fifotx,&ch,1)==1)
	{
		USART2->CR1 |= USART_CR1_TXEIE;
		return 1;
	}
	else
	{
		return -1;
	}
}
/*
 * Echoes out Input character
 *
 * Parameters:

 *
 * Returns:
 *   None.
 */
void uart_echo(void)
{
	char received_char;
	received_char = uart_getchar();
	uart_putchar(received_char);
	uart_putchar('\r');
	uart_putchar('\n');
}
/*
 * Echoes out Input string
 *
 * Parameters:
 * String to be printed
 *
 * Returns:
 *   None.
 */
void uart_printstring(char *message)
{
	while (*message)
	{
		uart_putchar(*message++);
	}
}


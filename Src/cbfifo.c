/*
 * cbfifo.c
 *
 *  Created on: Nov 5, 2024
 *      Author: Abhishek Nadgir
 */
/*******************************************************************************
 * Copyright (C) 2023 by <Abhishek Nadgir>
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are
 * permitted to modify this and use it to learn about the field of embedded
 * software. <Abhishek Nadgir> and the University of Colorado are not liable for
 * any misuse of this material.
 *
 * Reference used: Programming in C, Fourth Edition by Stephen G. Kochan
 *
 ******************************************************************************/
 // This code is taken from Assignment 2 of PES
/**
 * @file    cbfifo.c
 * @brief   Implementation of Circular Buffer FIFO
 *
 * @author  <Abhishek Nadgir>
 * @date    <09/17/2024>
 *
 */

#include "cbfifo.h"


/*
 * Initialize the FIFO structure.
 */
void cbfifo_init(cbfifo_t *fifo) {
    if (fifo != NULL) {
        fifo->write = 0;
        fifo->read = 0;
        fifo->length = 0;
    }
}

/*
 * Enqueues data onto the FIFO, up to the limit of the available FIFO capacity.
 *
 * Parameters:
 *   fifo    Pointer to the FIFO structure
 *   buf     Pointer to the data
 *   nbyte   Number of bytes to enqueue
 *
 * Returns:
 *   The number of bytes actually enqueued, which could be 0.
 *   In case of an error, returns (size_t) -1.
 */
size_t cbfifo_enqueue(cbfifo_t *fifo, void *buf, size_t nbyte) {
    if (fifo == NULL || buf == NULL) {
        return FIFO_ERROR;
    }
    if (nbyte == 0) {
        return 0;
    }
    size_t blank = (MAX_FIFO_SIZE - (fifo->length));
    if (blank == 0) {
        return 0;
    }
    size_t endq = (nbyte <= blank) ? nbyte : blank;
    uint8_t *temp = (uint8_t*)buf;

    for (size_t i = 0; i < endq; i++) {
        fifo->buffer[fifo->write] = temp[i];
        fifo->length++;
        fifo->write = ((fifo->write + 1) & (MAX_FIFO_SIZE-1)); // Increment write pointer with wrap-around
    }
    return endq;
}

/*
 * Attempts to remove data from the FIFO.
 *
 * Parameters:
 *   fifo    Pointer to the FIFO structure
 *   buf     Destination for the dequeued data
 *   nbyte   Bytes of data requested
 *
 * Returns:
 *   The number of bytes actually removed.
 */
size_t cbfifo_dequeue(cbfifo_t *fifo, void *buf, size_t nbyte) {
    if (fifo == NULL || buf == NULL || nbyte == 0 || fifo->length == 0) {
        return 0;
    }
    size_t endq = (nbyte <= fifo->length) ? nbyte : fifo->length;
    uint8_t *temp = (uint8_t*)buf;

    for (size_t i = 0; i < endq; i++) {
        temp[i] = fifo->buffer[fifo->read];
        fifo->length--;
        fifo->read = ((fifo->read + 1) & (MAX_FIFO_SIZE-1)); // Increment read pointer with wrap-around
    }
    return endq;
}

/*
 * Peeks the byte that will be available for a dequeue.
 * Does not remove the element from the queue.
 *
 * Parameters:
 *   fifo    Pointer to the FIFO structure
 *   byte    Returns the byte that will be available for next dequeue.
 *
 * Returns:
 *   Returns 1 if a byte is available to dequeue, otherwise 0.
 */
size_t cbfifo_peek(cbfifo_t *fifo, uint8_t *byte) {
    if (fifo == NULL || fifo->length == 0 || byte == NULL) {
        return 0;
    }
    byte[0] = fifo->buffer[fifo->read]; // Copying byte value into the provided buffer
    return 1;
}

/*
 * Returns the number of bytes currently on the FIFO.
 *
 * Parameters:
 *   fifo    Pointer to the FIFO structure
 *
 * Returns:
 *   Number of bytes currently available to be dequeued from the FIFO.
 */
size_t cbfifo_length(cbfifo_t *fifo) {
    return (fifo != NULL) ? fifo->length : 0; // Return length or 0 if fifo is NULL
}

/*
 * Returns the FIFO's capacity
 *
 * Parameters:
 *   fifo    Pointer to the FIFO structure
 *
 * Returns:
 *   The capacity, in bytes, for the FIFO.
 */
size_t cbfifo_capacity(cbfifo_t *fifo) {
    return (fifo != NULL) ? MAX_FIFO_SIZE : 0; // Return max size or 0 if fifo is NULL
}


size_t cbfifo_enqueue_float(cbfifo_t *fifo, float value) {
    uint8_t bytes[4];
    memcpy(bytes, &value, sizeof(float)); // Split float into bytes
    return cbfifo_enqueue(fifo, bytes, sizeof(float)); // Enqueue all 4 bytes
}


size_t cbfifo_dequeue_float(cbfifo_t *fifo, float *value) {
    uint8_t bytes[4];
    size_t dequeued = cbfifo_dequeue(fifo, bytes, sizeof(float)); // Dequeue 4 bytes
    if (dequeued == sizeof(float)) {
        memcpy(value, bytes, sizeof(float)); // Merge bytes back into float
    }
    return dequeued;
}

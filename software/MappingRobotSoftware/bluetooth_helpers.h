/*
 * bluetooth_helpers.h
 *
 *  Created on: Mar 8, 2016
 *      Author: cjamison
 */

#ifndef BLUETOOTH_HELPERS_H_
#define BLUETOOTH_HELPERS_H_

#include "includes.h"

/* Bytes that signify the start and end of packets */
#define START_BYTE 0x2
#define END_BYTE 0x3
#define ESCAPE_BYTE 0x4

/* Queue Configuration */
/*
 * 	Byte Queue:
 * 		Filled by bluetoothIRQ with serial bytes from the bluetooth
 * 		connection
 *
 * 	Receive Queue:
 * 		Filled with complete packets that are concatenated from the
 * 		byte queue by the receive task.
 *
 * 	Send Queue:
 * 		Put packets here to be sent. The send task waits on it and
 * 		to send over the bluetooth connection.
 */
#define BYTE_QUEUE_LENGTH 256
OS_EVENT* byteQueue;
void* byteQueueBuf[BYTE_QUEUE_LENGTH];

#define RECEIVE_QUEUE_LENGTH 10
OS_EVENT* receiveQueue;
void* receiveQueueBuf[RECEIVE_QUEUE_LENGTH];

#define SEND_QUEUE_LENGTH 10
OS_EVENT* sendQueue;
void* sendQueueBuf[SEND_QUEUE_LENGTH];

/* Custom buffer with length */
typedef struct {
	char* buf;
	int len;
} LenBuffer;

// Set up queues
int initBluetooth();

// Send the data contained in the buffer
void send(LenBuffer data);
void sendTask(void* pdata);

void receiveTask(void* pdata);
void bluetoothIRQ(void* context);

// For testing purposes. Simply takes packets from the receive queue
// and puts them in the send queue.
void echoTask(void* pdata);


#endif /* BLUETOOTH_HELPERS_H_ */

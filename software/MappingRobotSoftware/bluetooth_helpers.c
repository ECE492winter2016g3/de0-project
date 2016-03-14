/*
 * bluetooth_helpers.c
 *
 *  Created on: Mar 8, 2016
 *      Author: cjamison
 */

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <altera_avalon_uart_regs.h>
#include "bluetooth_helpers.h"
#include "packet_buffer.h"

void initBluetooth() {
	sendQueue = OSQCreate(sendQueueBuf, SEND_QUEUE_LENGTH);
	receiveQueue = OSQCreate(receiveQueueBuf, RECEIVE_QUEUE_LENGTH);
	byteQueue = OSQCreate(byteQueueBuf, BYTE_QUEUE_LENGTH);
}

void send(mBuffer data) {
	int nextByte = 0;
	while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_BLUETOOTH_BASE) & ALTERA_AVALON_UART_STATUS_TRDY_MSK));
	IOWR_ALTERA_AVALON_UART_TXDATA(UART_BLUETOOTH_BASE, START_BYTE);

	printf("send :: Into Loop\n");
	while(nextByte < data.len) {
		printf("send :: Sending byte %i: '%c'\n", nextByte, data.buf[nextByte]);
		while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_BLUETOOTH_BASE) & ALTERA_AVALON_UART_STATUS_TRDY_MSK));
		IOWR_ALTERA_AVALON_UART_TXDATA(UART_BLUETOOTH_BASE, data.buf[nextByte++]);
	}
	printf("send :: Out of Loop\n");
	while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_BLUETOOTH_BASE) & ALTERA_AVALON_UART_STATUS_TRDY_MSK));
	IOWR_ALTERA_AVALON_UART_TXDATA(UART_BLUETOOTH_BASE, END_BYTE);
}

void sendTask(void* pdata) {
	mBuffer* buf;
	INT8U err;
	static unsigned char packetId = 4;

	printf("sendTask :: started!\n");
	while(1) {
		buf = (mBuffer*) OSQPend(sendQueue, 0, &err);

		// TODO: Send 3 times for parity checking

		memmove(buf->buf+2, buf->buf, buf->len);
		buf->buf[0] = packetId++;
		buf->buf[1] = buf->len;
		buf->len += 2;
		printf("sendTask :: sending!\n");
		send(*buf);
		send(*buf);
		send(*buf);

		free(buf->buf);
		free(buf);
	}

}

void receiveTask(void* pdata) {
	char byte = '1';
	INT8U err;
	PacketBuffer pb;
	init(&pb);
	mBuffer* buf;

	printf("receiveTask :: started!\n");
	while(1) {
		printf("receiveTask :: waiting on queue!\n");
		byte = (char) OSQPend(byteQueue, 0, &err);
		printf("Received: %c\n", byte);
		if(byte == START_BYTE) {
			printf("Start of Packet!\n");
			clear(&pb);

		} else if(byte == END_BYTE) {
			printf("End of Packet!\n");

			// We have to malloc this because it's
			buf = (mBuffer*) malloc(sizeof(mBuffer));
			buf->buf = (char*) malloc(BUF_SIZE * sizeof(char));
			memset(buf->buf, 0, BUF_SIZE);
			buf->len = mRead(&pb, buf->buf);

			printf("Packet Contents: %s\n", buf->buf);
			OSQPost(receiveQueue, (void*) buf);
		} else {
			if(pushChar(&pb, byte)) {
				printf("Putting char: %c\n", byte);
			} else {
				printf("No room in buffer!\n");
			}
		}
	}
}

void bluetoothIRQ(void* context) {
	char read;
	read = IORD_ALTERA_AVALON_UART_RXDATA(UART_BLUETOOTH_BASE);
	OSQPost(byteQueue, (void*) read);
}

void echoTask(void* pdata) {
	void* packet;
	INT8U err;
	printf("Echo Task :: started!\n");
	while(1) {
		packet = OSQPend(receiveQueue, 0, &err);
		printf("Echo Task :: echoing packet!\n");
		OSQPost(sendQueue, packet);
	}
}


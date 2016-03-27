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

//#define printf() (void)0

int initBluetooth() {
	sendQueue = OSQCreate(sendQueueBuf, SEND_QUEUE_LENGTH);
	receiveQueue = OSQCreate(receiveQueueBuf, RECEIVE_QUEUE_LENGTH);
	byteQueue = OSQCreate(byteQueueBuf, BYTE_QUEUE_LENGTH);
	if(sendQueue == (OS_EVENT*) 0 || receiveQueue == (OS_EVENT*) 0 || byteQueue == (OS_EVENT*) 0 ) {
		return 0;
	} else {
		return 1;
	}
}

void send(LenBuffer data) {
	int nextByte = 0;
//	printf("send :: start of send\n");
	while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_BLUETOOTH_BASE) & ALTERA_AVALON_UART_STATUS_TRDY_MSK));
	IOWR_ALTERA_AVALON_UART_TXDATA(UART_BLUETOOTH_BASE, START_BYTE);

//	printf("send :: sending %i\n", START_BYTE);
	while(nextByte < data.len) {
		char byte = data.buf[nextByte++];
//		printf("send :: Sending byte %i: '%i'\n", nextByte, byte);
		if(byte == START_BYTE || byte == END_BYTE || byte == ESCAPE_BYTE) {
//			printf("send :: Escaping byte %i: %i\n", nextByte, byte);
			while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_BLUETOOTH_BASE) & ALTERA_AVALON_UART_STATUS_TRDY_MSK));
			IOWR_ALTERA_AVALON_UART_TXDATA(UART_BLUETOOTH_BASE, ESCAPE_BYTE);
//			printf("send :: sending %i\n", ESCAPE_BYTE);
		}
		while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_BLUETOOTH_BASE) & ALTERA_AVALON_UART_STATUS_TRDY_MSK));
		IOWR_ALTERA_AVALON_UART_TXDATA(UART_BLUETOOTH_BASE, byte);
//		printf("send :: sending %i\n", byte);
	}
	//printf("send :: Out of Loop\n");
	while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_BLUETOOTH_BASE) & ALTERA_AVALON_UART_STATUS_TRDY_MSK));
	IOWR_ALTERA_AVALON_UART_TXDATA(UART_BLUETOOTH_BASE, END_BYTE);
//	printf("send :: sending %i\n", END_BYTE);
//	printf("send :: end of send\n");
}

extern char charInter;
extern char charRecv;
extern char curCommand;
extern char sendCount;
void sendTask(void* pdata) {
	LenBuffer* buf;
	INT8U err;
	static unsigned char packetId = 4;

	while(1) {
		buf = (LenBuffer*) OSQPend(sendQueue, 0, &err);
		--sendCount;
		IOWR_8DIRECT(PIO_LEDS_BASE, 0, charInter | charRecv | curCommand | (sendCount << 4));
		memmove(buf->buf+2, buf->buf, buf->len);
		buf->buf[0] = packetId++;
		buf->buf[1] = buf->len;
		buf->len += 2;
		send(*buf);
		send(*buf);
		send(*buf);

		free(buf->buf);
		free(buf);
	}

}

char count = 0;
void receiveTask(void* pdata) {
	char byte = '1';
	INT8U err;
	PacketBuffer pb;
	init(&pb);
	LenBuffer* buf;

	printf("ReceiveTask :: started!\n");
	while(1) {
		byte = (char) OSQPend(byteQueue, 0, &err);

		if(byte != START_BYTE && byte != END_BYTE) {
			if(byte == 'f') {
				charRecv = 0x2;
			} else if(byte == 'b') {
				charRecv = 0x2;
			} else if(byte == 'l') {
				charRecv = 0x2;
			} else if(byte == 'r') {
				charRecv = 0x2;
			} else if(byte == 's') {
				charRecv = 0x0;
			} else {
				charRecv = 0;
			}
			IOWR_8DIRECT(PIO_LEDS_BASE, 0, charInter | charRecv | curCommand | (sendCount << 4));
		}
		if(err != OS_NO_ERR) {
			printf("ReceiveTask :: ERROR: %i", err);
			charRecv = 0x0;
			IOWR_8DIRECT(PIO_LEDS_BASE, 0, charInter | charRecv | curCommand | (sendCount << 4));
			continue;
		}
		printf("ReceiveTask :: Received: %i\n", byte);
		if(byte == START_BYTE) {
			clear(&pb);
			pb.isStarted = 1;
		} else if(byte == END_BYTE) {
			if(pb.isStarted) {
				// We have to malloc this because it's
				buf = (LenBuffer*) malloc(sizeof(LenBuffer));
				buf->buf = (char*) malloc(BUF_SIZE * sizeof(char));
				memset(buf->buf, 0, BUF_SIZE);
				buf->len = mRead(&pb, buf->buf);
				pb.isStarted = 0;

				OSQPost(receiveQueue, (void*) buf);
			}

		} else {
			if(pb.isStarted) {
				if(pushChar(&pb, byte)) {
//					printf("ReceiveTask :: %i\n", byte);
				} else {
//					printf("ReceiveTask :: No room in buffer!\n");
				}
			} else {
//				printf("ReceiveTask :: %i - discarded\n", byte);
			}
		}
	}
}

char first = 0;
void bluetoothIRQ(void* context) {
	char read = 0;
	while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_BLUETOOTH_BASE) & ALTERA_AVALON_UART_STATUS_RRDY_MSK));
	read = IORD_ALTERA_AVALON_UART_RXDATA(UART_BLUETOOTH_BASE);
	if(read == -1) {

	} else {
		// Debug stuff for LEDs
		if(read != START_BYTE && read != END_BYTE) {
			if(read == 'f') {
				charInter = 0x1;
			} else if(read == 'b') {
				charInter = 0x1;
			} else if(read == 'l') {
				charInter = 0x1;
			} else if(read == 'r') {
				charInter = 0x1;
			} else if(read == 's') {
				charInter = 0x0;
			} else {
				charInter = 0;
			}
			IOWR_8DIRECT(PIO_LEDS_BASE, 0, charInter | charRecv | curCommand | (sendCount << 4));
		}

		OSQPost(byteQueue, (void*) read);
	}
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

//#undef printf

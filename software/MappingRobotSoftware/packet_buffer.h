/*
 * packet_buffer.h
 *
 *  Created on: Mar 4, 2016
 *      Author: cjamison
 */

#ifndef PACKET_BUFFER_H_
#define PACKET_BUFFER_H_

#define BUF_SIZE 256
#define TRUE 1
#define FALSE 0


typedef struct {
	char myBuf[BUF_SIZE];
	int myStart;
	int myEnd;
	int myLen;
} PacketBuffer;

void init(PacketBuffer* pb);
int push(PacketBuffer* pb, char* inBuf, int len);
int pushChar(PacketBuffer* pb, char in);
int mRead(PacketBuffer* pb, char* outBuf);
void clear(PacketBuffer* pb);


#endif /* PACKET_BUFFER_H_ */

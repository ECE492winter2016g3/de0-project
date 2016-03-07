/*
 * packet_buffer.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: cjamison
 */

#include "packet_buffer.h"
#include <string.h>

void init(PacketBuffer* pb) {
	memset(pb->myBuf, 0, BUF_SIZE);
	pb->myStart = 0;
	pb->myEnd = 0;
	pb->myLen = 0;
}

int push(PacketBuffer* pb, char* inBuf, int len) {
	int i;
	if(pb->myLen + len > BUF_SIZE) {
		return FALSE;
	}

	int index;
	for(i = 0; i < len; ++i) {
		index = (pb->myEnd + i) % BUF_SIZE;
		pb->myBuf[index] = inBuf[i];
	}
	pb->myEnd = index;
	pb->myLen += len;

	return TRUE;
}

int pushChar(PacketBuffer* pb, char in) {
	if(pb->myStart == pb->myEnd && pb->myLen > 0) {
		return FALSE;
	}

	pb->myBuf[pb->myEnd++] = in;
	pb->myLen++;
	return TRUE;
}

int mRead(PacketBuffer* pb, char* outBuf) {
	int i;
	int index;
	for(i = 0; i < pb->myLen; ++i) {
		index = (pb->myStart + i) % BUF_SIZE;
		outBuf[i] = pb->myBuf[index];
	}
	return pb->myLen;
}

void clear(PacketBuffer* pb) {
	pb->myLen = 0;
	pb->myStart = pb->myEnd;
}

/*************************************************************************
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
* All rights reserved. All use of this software and documentation is     *
* subject to the License Agreement located at the end of this file below.*
**************************************************************************
* Description:                                                           *
* The following is a simple hello world program running MicroC/OS-II.The * 
* purpose of the design is to be a very simple application that just     *
* demonstrates MicroC/OS-II running on NIOS II.The design doesn't account*
* for issues such as checking system call return codes. etc.             *
*                                                                        *
* Requirements:                                                          *
*   -Supported Example Hardware Platforms                                *
*     Standard                                                           *
*     Full Featured                                                      *
*     Low Cost                                                           *
*   -Supported Development Boards                                        *
*     Nios II Development Board, Stratix II Edition                      *
*     Nios Development Board, Stratix Professional Edition               *
*     Nios Development Board, Stratix Edition                            *
*     Nios Development Board, Cyclone Edition                            *
*   -System Library Settings                                             *
*     RTOS Type - MicroC/OS-II                                           *
*     Periodic System Timer                                              *
*   -Know Issues                                                         *
*     If this design is run on the ISS, terminal output will take several*
*     minutes per iteration.                                             *
**************************************************************************/
//#define I2C_DEBUG

#include <stdio.h>
#include "includes.h"
#include <system.h>
#include <altera_avalon_pio_regs.h>
#include <altera_avalon_uart_regs.h>
#include <fcntl.h>
#include "packet_buffer.h"
#include "bluetooth_helpers.h"
#include "LIDAR.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK    task3_stk[TASK_STACKSIZE];
OS_STK    task4_stk[TASK_STACKSIZE];
OS_STK    task5_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      4
#define TASK2_PRIORITY      3
#define TASK3_PRIORITY      2
#define TASK4_PRIORITY      1
#define TASK5_PRIORITY      5

/* DC Driver API */
// EG: dc_driver_write(MOTORA_CW | MOTORB_CCW)
//     dc_driver_write(MOTORB_CW)
// (Note: All unreferenced motors are set to brake)
#define MOTORA_CW  0x2 // 00000010
#define MOTORA_CCW 0x3 // 00000011
#define MOTORB_CW  0x8 // 00001000
#define MOTORB_CCW 0xC // 00001100
#define MOTORA_STOP 0
#define MOTORB_STOP 0
void dc_driver_write(state) {
	IOWR_32DIRECT(DC_DRIVER_0_BASE, 0, state);
}

/* DC MOTOR BT COMMANDS*/
#define BT_CMD_FORWARD 'f'
#define BT_CMD_BACKWARD 'b'
#define BT_CMD_RIGHT 'r'
#define BT_CMD_LEFT 'l'
#define BT_CMD_STOP 's'
#define BT_CMD_TIMEOUT 1000

//#define ALTERA_AVALON_UART_TERMIOS

/* Prints "Hello World" and sleeps for three seconds */
int receiveCount = 0x01;
int currentCommand = 0x00;
int currentLights = 0x00;


char charInter = 0;
char charRecv = 0;
char curCommand = 0;

void task1(void* pdata)
{
	INT16U range = 0;
//	ALTERA_AVALON_UART_STATE_INSTANCE(UART_BLUETOOTH, bluetooth_state);
//	ALTERA_AVALON_UART_STATE_INIT(UART_BLUETOOTH, bluetooth_state);
//	ALTERA_AVALON_UART_DEV_INSTANCE(UART_BLUETOOTH, bluetooth_dev);
//	ALTERA_AVALON_UART_DEV_INIT(UART_BLUETOOTH, bluetooth_dev);
////	alt_fd* fd = bluetooth_dev->dev->open();
//
//	int fd = open(UART_BLUETOOTH_NAME, O_RDWR | O_NONBLOCK);
//	altera_avalon_uart_write_fd(fd, "TEST", 4);
//
//
//
//	IOWR(PIO_LEDS_BASE, 0, 0xFF);
//	printf("LEDs are changing\n");

//	int i = 0;
//	for (; ++i;) {
//		int j = 0;
//		OSTimeDlyHMSM(0, 0, 1, 0);
//		for (;j < 25; ++j) {
//			int t = 2;
//			IOWR_32DIRECT(STEPPER_DRIVER_BASE, 0, 0x00000008); // 1000
//			OSTimeDlyHMSM(0, 0, 0, t);
//			IOWR_32DIRECT(STEPPER_DRIVER_BASE, 0, 0x0000000A); // 1010
//			OSTimeDlyHMSM(0, 0, 0, t);
//			IOWR_32DIRECT(STEPPER_DRIVER_BASE, 0, 0x00000002); // 0010
//			OSTimeDlyHMSM(0, 0, 0, t);
//			IOWR_32DIRECT(STEPPER_DRIVER_BASE, 0, 0x0000000E); // 1110
//			OSTimeDlyHMSM(0, 0, 0, t);
//			IOWR_32DIRECT(STEPPER_DRIVER_BASE, 0, 0x0000000C); // 1100
//			OSTimeDlyHMSM(0, 0, 0, t);
//			IOWR_32DIRECT(STEPPER_DRIVER_BASE, 0, 0x0000000F); // 1111
//			OSTimeDlyHMSM(0, 0, 0, t);
//			IOWR_32DIRECT(STEPPER_DRIVER_BASE, 0, 0x00000003); // 0011
//			OSTimeDlyHMSM(0, 0, 0, t);
//			IOWR_32DIRECT(STEPPER_DRIVER_BASE, 0, 0x0000000B); // 1011
//			OSTimeDlyHMSM(0, 0, 0, t);
//		}
//	}
	dc_driver_write(0);
//	int testDist = 0;
	for (;;) {
//		range = lidar_read();
////		range = testDist++;
//		printf("LIDAR range: %i\n", range);
//		int led = 8 * ((range < 100) ? range : 100) / 100;
//		int i;
//		int ledRes = 0;
//		for (i = 0; i < led; ++i) {
//			ledRes = (ledRes << 1) | 1;
//		}
//		//IOWR(PIO_LEDS_BASE, 0, ledRes);
//
//		LenBuffer *buff = malloc(sizeof(LenBuffer));
//		char* buffData = malloc(10);
//		buffData[0] = range & 0x00FF;
//		buffData[1] = (range & 0xFF00) >> 8;
//		buff->len = 2;
//		buff->buf = buffData;
//		OSQPost(sendQueue, buff);
//		//IOWR_32DIRECT(STEPPER_DRIVER_0_BASE, 0, 200);
////		IOWR(PIO_LEDS_BASE, 0, 0xcc);
		OSTimeDly(100);
//		IOWR(PIO_LEDS_BASE, 0, receiveCount | currentCommand | currentLights);
//		if(currentLights == 0x80) {
//			currentLights = 0x00;
//		} else {
//			currentLights = 0x80;
//		}
	}
}

int motora = 0;
int motorb = 0;
void handle_bluetooth_input(char c) {
	c |= 0x20;
	//printf("Cmd: %c\n", c);
	if (c == BT_CMD_FORWARD) {
		motora = MOTORA_CW;
		motorb = MOTORB_CW;
		dc_driver_write(motora | motorb);
	} else if (c == BT_CMD_BACKWARD) {
		motora = MOTORA_CCW;
		motorb = MOTORB_CCW;
		dc_driver_write(motora | motorb);
	} else 	if (c == BT_CMD_LEFT) {
		motora = MOTORA_CW;
		motorb = MOTORB_CCW;
		dc_driver_write(motora | motorb);
	} else if (c == BT_CMD_RIGHT) {
		motora = MOTORA_CCW;
		motorb = MOTORB_CW;
		dc_driver_write(motora | motorb);
	}
	else if (c == BT_CMD_STOP) {
		motora = MOTORA_STOP;
		motorb = MOTORB_STOP;
		dc_driver_write(motora | motorb);
	}
}

void task2(void* pdata)
{
//	printf("Started task\n");
	// Receive input from phone
	INT8U err;
	while (1) {
//		IOWR(PIO_LEDS_BASE, 0, receiveCount | currentCommand | currentLights);
		LenBuffer *slice = OSQPend(receiveQueue, BT_CMD_TIMEOUT, &err);
//		printf("ReceiveCount: %i\n", receiveCount);
		if(err == OS_ERR_TIMEOUT) {
			//printf("TIMEOUT\n");
			handle_bluetooth_input(BT_CMD_STOP);
//			receiveCount = receiveCount << 1;
//			receiveCount &= 0xF;
//			if(receiveCount == 0) {
//				receiveCount = 1;
//			}
			curCommand = 0xE0;
			IOWR_8DIRECT(PIO_LEDS_BASE, 0, charInter | charRecv | curCommand);
		}
		else {
			receiveCount = receiveCount << 1;
			receiveCount &= 0xF;
			if(receiveCount == 0) {
				receiveCount = 1;
			}
			if(slice->len > 0) {
				if(slice->buf[0] == BT_CMD_FORWARD) {
					currentCommand = 0x10;
				} else if(slice->buf[0] == BT_CMD_BACKWARD) {
					currentCommand = 0x20;
				} else if(slice->buf[0] == BT_CMD_LEFT) {
					currentCommand = 0x40;
				} else if(slice->buf[0] == BT_CMD_RIGHT) {
					currentCommand = 0x80;
				} else if(slice->buf[0] == BT_CMD_STOP) {
					currentCommand = 0x70;
				} else {
					printf("Command: |%c|\n", slice->buf[0]);
					currentCommand = 0xF0;
				}
				curCommand = currentCommand;
				IOWR_8DIRECT(PIO_LEDS_BASE, 0, charInter | charRecv | curCommand);
			}
//			printf("Got: %s\n", slice->buf);
			size_t i;
			for (i = 0; i < slice->len; ++i) {
				handle_bluetooth_input(slice->buf[i]);
			}
			free(slice->buf);
			free(slice);
		}
	}
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{
	printf("Starting?\n");

    INT8U err;
    if(initBluetooth()) {
    	  err = OSTaskCreateExt(sendTask,
    	                  NULL,
    	                  (void *)&task3_stk[TASK_STACKSIZE-1],
    	                  TASK3_PRIORITY,
    	                  TASK3_PRIORITY,
    	                  task3_stk,
    	                  TASK_STACKSIZE,
    	                  NULL,
    	                  0);

    	  OSTaskCreateExt(receiveTask,
    	                  NULL,
    	                  (void *)&task4_stk[TASK_STACKSIZE-1],
    	                  TASK4_PRIORITY,
    	                  TASK4_PRIORITY,
    	                  task4_stk,
    	                  TASK_STACKSIZE,
    	                  NULL,
    	                  0);
    } else {
    	IOWR(PIO_LEDS_BASE, 0, 0xEE);
    }

  OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
//
  OSTaskCreateExt(task2,
                  NULL,
                  (void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);


//  OSTaskCreateExt(echoTask,
//                  NULL,
//                  (void *)&task5_stk[TASK_STACKSIZE-1],
//                  TASK5_PRIORITY,
//                  TASK5_PRIORITY,
//                  task5_stk,
//                  TASK_STACKSIZE,
//                  NULL,
//                  0);

  if(alt_ic_isr_register(
				  UART_BLUETOOTH_IRQ_INTERRUPT_CONTROLLER_ID,
				  UART_BLUETOOTH_IRQ,
				  &bluetoothIRQ,
				  NULL,
				  NULL
  )) {
      printf("Register failed\n");
  }

  OSStart();
  return 0;
}

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/

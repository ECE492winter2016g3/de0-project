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
#include "i2c_opencores.h"
#include "terasic_lib/terasic_includes.h"
#include <fcntl.h>
#include "I2C.h"
#include "packet_buffer.h"
#include "bluetooth_helpers.h"


/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK    task3_stk[TASK_STACKSIZE];
OS_STK    task4_stk[TASK_STACKSIZE];
OS_STK    task5_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2
#define TASK3_PRIORITY      3
#define TASK4_PRIORITY      4
#define TASK5_PRIORITY      5

/* DC Driver API */
// EG: dc_driver_write(MOTORA_CW | MOTORB_CCW)
//     dc_driver_write(MOTORB_CW)
// (Note: All unreferenced motors are set to brake)
#define MOTORA_CW  0x3 // 00000011
#define MOTORA_CCW 0x2 // 00000010
#define MOTORB_CW  0xC // 00001100
#define MOTORB_CCW 0x8 // 00001000
#define LIDAR_I2C_ADDRESS   0xC4
void dc_driver_write(state) {
	IOWR_32DIRECT(DC_DRIVER_0_BASE, 0, state);
}

//#define ALTERA_AVALON_UART_TERMIOS

/* Prints "Hello World" and sleeps for three seconds */
void task1(void* pdata)
{
	int status = 0;
	//Terasic I2C
	//Test Addressing
	I2C_Start(I2C_SCL_BASE, I2C_SDA_BASE);
	status = i2c_selectAddress(I2C_SCL_BASE, I2C_SDA_BASE, LIDAR_I2C_ADDRESS);
	printf("I2C status: %i\n", status);
	I2C_Stop(I2C_SCL_BASE, I2C_SDA_BASE);
	bool i2c_disable = TRUE;
	//laser loop
	    while(1) {
	    	if(i2c_disable)
	    		break;
	    	bool ack = FALSE;
	    	alt_8 poll_value = 0x04;
	    	alt_8 upper = 0;
	    	alt_8 lower = 0;
	    	int range = 0;
	    	//Poll laser
	    	while(ack == FALSE) {
	    	    I2C_Start(I2C_SCL_BASE, I2C_SDA_BASE);
	    		ack = I2C_WriteToDeviceRegister(I2C_SCL_BASE, I2C_SDA_BASE, LIDAR_I2C_ADDRESS, 0x00, &poll_value, 1);
	    		I2C_Stop(I2C_SCL_BASE, I2C_SDA_BASE);
	    		OSTimeDlyHMSM(0, 0, 0, 100);
	    	}
	        I2C_Start(I2C_SCL_BASE, I2C_SDA_BASE);
	    	if(FALSE == I2C_ReadFromDeviceRegister(I2C_SCL_BASE, I2C_SDA_BASE, LIDAR_I2C_ADDRESS, 0x0f, &upper, 1, TRUE)) {
	    		printf("I2C READ1 FAILED\n");
	    	}
	    	else {
	        	I2C_Stop(I2C_SCL_BASE, I2C_SDA_BASE);
	    		I2C_Start(I2C_SCL_BASE, I2C_SDA_BASE);
	    		if(FALSE == I2C_ReadFromDeviceRegister(I2C_SCL_BASE, I2C_SDA_BASE, LIDAR_I2C_ADDRESS, 0x10, &lower, 1, TRUE)) {
	    			printf("I2C READ2 FAILED\n");
	    		}
	    		else {
	    			printf("\nresults: %i, %i\n", upper, lower);
	    			range = (upper << 8) + lower;
	    			printf("RANGE: %i\n", range);
	    		}
	    	}
	    	I2C_Stop(I2C_SCL_BASE, I2C_SDA_BASE);
	    }
	/*
	alt_u32 upper = 0;
	alt_u32 lower = 0;
	alt_u32 result = 0;
	int status = 0;
	I2C_init(I2C_OPENCORES_LIDAR_BASE, ALT_CPU_CPU_FREQ, 100000);
	status = I2C_start(I2C_OPENCORES_LIDAR_BASE, 0x62, 0);
	I2C_write(I2C_OPENCORES_LIDAR_BASE, 0x00, 0);
	I2C_write(I2C_OPENCORES_LIDAR_BASE, 0x04, 1);
	OSTimeDly(1000);
	I2C_start(I2C_OPENCORES_LIDAR_BASE, 0x62, 0);
	I2C_write(I2C_OPENCORES_LIDAR_BASE, 0x0f, 1);
	I2C_start(I2C_OPENCORES_LIDAR_BASE, 0x62, 1);
	upper = I2C_read(I2C_OPENCORES_LIDAR_BASE, 1);
	I2C_start(I2C_OPENCORES_LIDAR_BASE, 0x62, 0);
	I2C_write(I2C_OPENCORES_LIDAR_BASE, 0x10, 1);
	I2C_start(I2C_OPENCORES_LIDAR_BASE, 0x62, 1);
	lower = I2C_read(I2C_OPENCORES_LIDAR_BASE, 1);

	result = (upper << 8) + lower;
	printf("RANGE: %i", result);
	*/
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

	for (;;)
		OSTimeDly(1000);
}

void task2(void* pdata)
{
	printf("Starting...\n");
	IOWR_32DIRECT(STEPPER_DRIVER_0_BASE, 1, 0x00010000);
	dc_driver_write(MOTORA_CCW | MOTORB_CCW);
	printf("Done.\n");
	for(;;)
		OSTimeDly(10000);
	//for (;;)
	//	printf("Blah\n");

	unsigned char dir = 0;
	while (1)
	{
		OSTimeDlyHMSM(0, 0, 1, 0);

		// Reverse and drive the motor
		dir = ~dir;
		if (dir) {
			dc_driver_write(MOTORA_CCW | MOTORB_CCW);
		} else {
			dc_driver_write(MOTORA_CW | MOTORB_CCW);
		}
	}
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{
	printf("Starting?\n");

    INT8U err;
    initBluetooth();
  OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  OSTaskCreateExt(task2,
                  NULL,
                  (void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

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

  OSTaskCreateExt(echoTask,
                  NULL,
                  (void *)&task5_stk[TASK_STACKSIZE-1],
                  TASK5_PRIORITY,
                  TASK5_PRIORITY,
                  task5_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

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

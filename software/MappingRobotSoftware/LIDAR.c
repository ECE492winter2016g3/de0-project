/*
 * LIDAR.c
 *
 * Created on: Mar 11, 2016
 *      Author: jkeeling
 */

#include <stdio.h>
#include <system.h>
#include "terasic_lib/terasic_includes.h"
#include "I2C.h"
#include "LIDAR.h"

#define LIDAR_I2C_ADDRESS   	0xc4
#define LIDAR_REGISTER_COMMAND	0x00
#define LIDAR_REGISTER_UPPER	0x0f
#define LIDAR_REGISTER_LOWER	0x10
#define LIDAR_COMMAND_POLL		0x04
#define	READ_DELAY				20

int read_registers();
int send_command(alt_u8 command);

// return 1 if success, 0 otherwise. Maybe change to bool later if decide on using bools
int lidar_test_address()
{
	int status = 0;
	I2C_Start(I2C_SCL_BASE, I2C_SDA_BASE);
	status = i2c_selectAddress(I2C_SCL_BASE, I2C_SDA_BASE, LIDAR_I2C_ADDRESS);
	I2C_Stop(I2C_SCL_BASE, I2C_SDA_BASE);
	printf("LIDAR address status : %i\n", status);
	return status;
}

// poll and read 20ms later
int lidar_read() {
	if(send_command(LIDAR_COMMAND_POLL)) {
		OSTimeDlyHMSM(0, 0, 0, READ_DELAY);
		return read_registers();
	}
	return 0;
}

// poll and read as soon as possible
int lidar_read_fast(){
	int result = 0;
	int count = 0;
    while(!result && count++ < READ_DELAY) {
    	result = read_registers();
		OSTimeDlyHMSM(0, 0, 0, 1);
    }
    return result;
}

int send_command(alt_u8 command) {
	bool status = FALSE;
	I2C_Start(I2C_SCL_BASE, I2C_SDA_BASE);
	status = I2C_WriteToDeviceRegister(I2C_SCL_BASE, I2C_SDA_BASE, LIDAR_I2C_ADDRESS, LIDAR_REGISTER_COMMAND, &command, 1);
	I2C_Stop(I2C_SCL_BASE, I2C_SDA_BASE);
	return status;
}

int read_registers() {
	alt_u8 upper = 0;
	alt_u8 lower = 0;
	int range = 0;
	I2C_Start(I2C_SCL_BASE, I2C_SDA_BASE);
	if(I2C_ReadFromDeviceRegister(I2C_SCL_BASE, I2C_SDA_BASE, LIDAR_I2C_ADDRESS, 0x0f, &upper, 1, TRUE)) {
		I2C_Stop(I2C_SCL_BASE, I2C_SDA_BASE);
		I2C_Start(I2C_SCL_BASE, I2C_SDA_BASE);
		if(!I2C_ReadFromDeviceRegister(I2C_SCL_BASE, I2C_SDA_BASE, LIDAR_I2C_ADDRESS, LIDAR_REGISTER_LOWER, &lower, 1, TRUE)) {
			printf("LOWER READ FAILED\n");
			return FALSE;
		}
		I2C_Stop(I2C_SCL_BASE, I2C_SDA_BASE);
	}
	else {
		printf("UPPER READ FAILED\n");
		return FALSE;
	}
	//printf("UPPER, LOWER: %i, %i\n", upper, lower);
	range = (upper << 8) + lower;
	printf("LIDAR range value: %i\n", range);
	return range;
}

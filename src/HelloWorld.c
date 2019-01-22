/*
 ============================================================================
 Name        : HelloWorld.c
 Author      : Kai Chi & Sleyter Angulo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int file_gyro;
int file_rgb;
int previous_x;
int previous_y;
int previous_z;
int angular_disp_x;
int angular_disp_y;
int angular_disp_z;
int xGyro = 0;
int yGyro = 0;
int zGyro = 0;


int main() {

	char *bus_rgb = "/dev/i2c-1";
	char *bus_gyro = "/dev/i2c-2";

	if ((file_rgb = open(bus_rgb, O_RDWR)) < 0) {
		printf("Failed to open the rgb bus. \n");
		exit(1);
	}

	if ((file_gyro = open(bus_gyro, O_RDWR)) < 0) {
		printf("Failed to open the gyro bus. \n");
		exit(1);
	}

	printf("Rgb bus opened. Return value = %d \n",
			ioctl(file_rgb, I2C_SLAVE, 0x29));

	printf("Gyro bus opened. Return value = %d \n",
			ioctl(file_gyro, I2C_SLAVE, 0x6B));

	config_rgb();
	config_gyro();

	while (1) {
		print_rgb_values();
		print_gyro_values();
		printf("*******************************************************\n");
		sleep(1);
	}

	exit(0);
}

void config_rgb() {
	// Select enable register(0x80)
	// Power ON, RGBC enable, wait time disable(0x03)
	char config[2] = { 0 };
	config[0] = 0x80;
	config[1] = 0x03;
	write(file_rgb, config, 2);

	// Select ALS time register(0x81)
	// Atime = 700 ms(0x00)
	config[0] = 0x81;
	config[1] = 0x00;
	write(file_rgb, config, 2);
	// Select Wait Time register(0x83)
	// WTIME : 2.4ms(0xFF)
	config[0] = 0x83;
	config[1] = 0xFF;
	write(file_rgb, config, 2);
	// Select control register(0x8F)k
	// AGAIN = 1x(0x00)
	config[0] = 0x8F;
	config[1] = 0x00;
	write(file_rgb, config, 2);
	sleep(1);
}

void config_gyro() {
	// Enable X, Y, Z-Axis and disable Power down mode(0x0F)
	char config[2] = { 0 };
	config[0] = 0x20;
	config[1] = 0x0F;
	write(file_gyro, config, 2);
	// Full scale range, 2000 dps(0x30)
	config[0] = 0x23;
	config[1] = 0x30;
	write(file_gyro, config, 2);
	sleep(1);
}

void print_rgb_values() {
	// Read 8 bytes of data from register(0x94)
	// cData lsb, cData msb, red lsb, red msb, green lsb, green msb, blue lsb, blue msb
	char reg[1] = { };
	write(file_rgb, reg, 1);
	char data[8] = { 0 };
	if (read(file_rgb, data, 8) != 8) {
		printf("Erorr : Input/output Erorr \n");
	} else {
		// Convert the data
		int cData = (data[1] << 8) + data[0];
		int red = (data[3] << 8) + data[2];
		int green = (data[5] << 8) + data[4];
		int blue = (data[7] << 8) + data[6];

		// Calculate luminance
		float luminance = (-0.32466) * (red) + (1.57837) * (green)
				+ (-0.73191) * (blue);
		if (luminance < 0) {
			luminance = 0;
		}

		// Output data to screen
		printf("Color R: %d,", red);
		printf("G : %d,", green);
		printf("B : %d [lux] \n", blue);
		printf("IR : %d [lux] \n", cData);
		printf("Ambient Light : %.2f [lux] \n", luminance);
	}
}

void print_gyro_values() {
	char reg[1] = { 0x28 };
	write(file_gyro, reg, 1);
	char datai[1] = { 0 };
	
	if (read(file_gyro, datai, 1) != 1) {
		printf("Error : Input/Output Error \n");
		exit(1);
	}
	char data_0 = datai[0];
	
	//Assigning previous values
	previous_x = xGyro;
	previous_y = yGyro;
	previous_z= zGyro;


	//xGyro msb
	reg[0] = 0x29;
	write(file_gyro, reg, 1);
	read(file_gyro, datai, 1);
	char data_1 = datai[0];
	

	//yGyro lsb
	reg[0] = 0x2A;
	write(file_gyro, reg, 1);
	read(file_gyro, datai, 1);
	char data_2 = datai[0];

	//yGyro msb
	reg[0] = 0x2B;
	write(file_gyro, reg, 1);
	read(file_gyro, datai, 1);
	// same operation
	char data_3 = datai[0];

	//zGyro lsb
	reg[0] = 0x2C;
	write(file_gyro, reg, 1);
	read(file_gyro, datai, 1);
	char data_4 = datai[0];

	//zGyro msb
	reg[0] = 0x2D;
	write(file_gyro, reg, 1);
	read(file_gyro, datai, 1);
	char data_5 = datai[0];

	//Convert data
	xGyro = (data_1 << 8) + data_0;
	if (xGyro > 32767) {
		xGyro -= 65536;
	}

	yGyro = (data_3 << 8) + data_2;
	if (yGyro > 32767) {
		yGyro -= 65536;
	}

	zGyro = (data_5 << 8) + data_4;
	if (zGyro > 32767) {
		zGyro -= 65536;
	}
	// Output data to screen
	printf("Rotation : %d, ", xGyro);
	printf("%d, ", yGyro);
	printf("%d\n", zGyro);
	
	//Call to calculate angular displacement
	angular_disp_x += calculate_angular(xGyro, previous_x);
	angular_disp_y += calculate_angular(yGyro, previous_y);
	angular_disp_z += calculate_angular(zGyro, previous_z);

	//Print angular displacement
	printf("Angular Displacement: x:%d", angular_disp_x);
	printf("y: %d, ", angular_disp_y);
	printf("z: %d \n", angular_disp_z);
	
}

int calculate_angular (int current, int previous){
	int sensor_velocity = 100;
	int prev_angular = (current - previous) * sensor_velocity;
	return prev_angular;
	
	}

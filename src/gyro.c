/*
 ============================================================================
 Name        : Gyro.c

 Author      : Sleyter and Kajetan
 ============================================================================
 */

#include "gyro.h"

void run_gyro() {
	char *bus_gyro = "/dev/i2c-2";

	if ((file_gyro = open(bus_gyro, O_RDWR)) < 0) {
		printf("Failed to open the gyro bus. \n");
		exit(1);
	}

	printf("Gyro bus opened. Return value = %d \n",
			ioctl(file_gyro, I2C_SLAVE, 0x6B));

	config_gyro();

	float xangle_accum = 0.0f;
	float yangle_accum = 0.0f;
	float zangle_accum = 0.0f;

	while (1) {
		/*
		 * 1. Read status register 0x27
		 * 2. Check three last status bits for new gyro data
		 */
		char reg[1] = {0x27};
		char data[1]={0};
		write(file_gyro, reg, 1);
		read(file_gyro, data, 1);
		if (data[0] & 0x7) {
			 get_gyro_data(data[0], &xangle_accum, &yangle_accum, &zangle_accum);
		}
	}
}

void get_gyro_data(char reg_status, float *x_angle_accum, float *y_angle_accum, float *z_angle_accum ) {

	// Instantaneous local velocities (x,y and z)
	float x_rotational_speed = 0.0f;
	float y_rotational_speed = 0.0f;
	float z_rotational_speed = 0.0f;

	char new_data_flags = 0x0;

	//check the X-axis
	if (reg_status & 0x1) {
		new_data_flags |= 0x1;
		calculate(X, x_angle_accum, &x_rotational_speed);
	}
	// check the Y-axis
	if (reg_status & 0x2) {
		new_data_flags |= 0x2;
		calculate(Y, y_angle_accum, &y_rotational_speed);
	}
	// check the Z-axis
	if (reg_status & 0x4) {
		new_data_flags |= 0x4;
		calculate(Z, z_angle_accum, &z_rotational_speed);
	}

	if (new_data_flags){
		printf("\n******************************************************\n");
		printf("Rotational speed [x,y,z]: %.2f, %.2f, %.2f\n", x_rotational_speed,y_rotational_speed,z_rotational_speed);
		printf("Angles [x,y,z]          : %.2f, %.2f, %.2f\n",*x_angle_accum, *y_angle_accum, *z_angle_accum);
	}
}

void calculate(char axis, float *angle_accum, float *rotational_speed) {
	int new_value = get_filtered_rotational_speed_value(axis);
	*rotational_speed = normalize_rotational_speed_value(new_value);
	calculate_angle(angle_accum, rotational_speed);
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

float normalize_rotational_speed_value(int value) {
	float normalized_value = (float) value * g_sens;

	if (normalized_value>-4 && normalized_value< 4) {
		normalized_value= 0;
	}
	return normalized_value;
}

int get_filtered_rotational_speed_value(char a) {
	char reg_lsb[1]={0};
	char reg_msb[1] ={0};
	char datai[1] = { 0 };

	switch (a) {
	case 'X':
		reg_lsb[0] = 0x28;
		reg_msb[0] = 0x29;
		break;
	case 'Y':
		reg_lsb[0] = 0x2A;
		reg_msb[0] = 0x2B;
		break;
	case 'Z':
		reg_lsb[0] = 0x2C;
		reg_msb[0] = 0x2D;
		break;
	default:
		printf("Unknown axis: %c\n", a);
		exit(1);
	}

	write(file_gyro, reg_lsb, 1);
	read(file_gyro, datai, 1);
	char data_0 = datai[0];

	write(file_gyro, reg_msb, 1);
	read(file_gyro, datai, 1);
	char data_1 = datai[0];

	int new_value = (data_1 << 8) + data_0;
	if (new_value > 32767) {
		new_value -= 65536;
	}

	return new_value;
}

void calculate_angle(float *accum, float *gyro_value) {
	*accum += (*gyro_value * time);

	//reduce the value to scope [-180,180]
	if (*accum < -180 ){
		*accum += 360;
	}

	if (*accum > 180 ) {
		*accum -= 360;
	}
}

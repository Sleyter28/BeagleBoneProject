#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int file_gyro;

#define time 0.01//period 10ms (100Hz)
#define g_sens 0.07 //Sensitivity gyroscope (2000dps)
#define X 'X'
#define Y 'Y'
#define Z 'Z'

void run_gyro();
void config_gyro();
void get_gyro_data(char reg_status, float *x_angle_accum, float *y_angle_accum, float *z_angle_accum);
void calculate(char axis, float *angle_accum, float *rotational_speed);
float normalize_rotational_speed_value(int value);
int get_filtered_rotational_speed_value(char a);
void calculate_angle(float *accum, float *gyro_value);

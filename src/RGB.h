/*
 * RGB.h
 *
 *  Created on: Jan 25, 2019
 *      Author: dte
 */

#ifndef RGB_H_
#define RGB_H_


#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>

int file_rgb;

void run_rgb();
void config_rgb();
void print_rgb_values();
void closestColor(int r,int g,int b);

#endif /* RGB_H_ */

#include "gyro.h"
#include "RGB.h"

#define RUN_GYRO 1

int main() {
	if(RUN_GYRO) {
		run_gyro();
	}
	else {
		run_rgb();
	}
	return 0;
}

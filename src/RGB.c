#include "RGB.h"


const int distinctRGB[22][3] = {{255, 255, 255},{0,0,0},{128,0,0},{255,0,0},{255, 200, 220},{170, 110, 40},{255, 150, 0},{255, 215, 180},{128, 128, 0},{255, 235, 0},{255, 250, 200},{190, 255, 0},{0, 190, 0},{170, 255, 195},{0, 0, 128},{100, 255, 255},{0, 0, 128},{67, 133, 255},{130, 0, 150},{230, 190, 255},{255, 0, 255},{128, 128, 128}};
const char *distinctColors[22] = {"white","black","maroon","red","pink","brown","orange","coral","olive","yellow","beige","lime","green","mint","teal","cyan","navy","blue","purple","lavender","magenta","grey"};

void closestColor(int r,int g,int b) {
	int i = 0;
	char colorReturn[7] = "NA";
	int biggestDifference = 1000;
	for (i = 0; i < 22; i++) {
		if (sqrt(pow(r - distinctRGB[i][0],2) + pow(g - distinctRGB[i][1],2) + pow(b - distinctRGB[i][2],2)) < biggestDifference) {
		  strcpy(colorReturn, distinctColors[i]);
		  biggestDifference = sqrt(pow(r - distinctRGB[i][0],2) + pow(g - distinctRGB[i][1],2) + pow(b - distinctRGB[i][2],2));
		}
	}
	printf("Closest color: %s\n", colorReturn);
}


void run_rgb() {

       char *bus_rgb = "/dev/i2c-1";

       if ((file_rgb = open(bus_rgb, O_RDWR)) < 0) {
               printf("Failed to open the rgb bus. \n");
               exit(1);
       }


       printf("Rgb bus opened. Return value = %d \n",
                       ioctl(file_rgb, I2C_SLAVE, 0x29));


       config_rgb();

       while (1) {
		   print_rgb_values();
//    	   get_rgb_values();
		   printf("*******************************************************\n");
		   sleep(1);
       }
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


void print_rgb_values() {
       // Read 8 bytes of data from register(0x94)
       // cData lsb, cData msb, red lsb, red msb, green lsb, green msb, blue ls
       char reg[1] = { 0x94};
       write(file_rgb, reg, 1);
       char data[8] = { 0 };
       if (read(file_rgb, data, 8) != 8) {
               printf("Error : Input/output Erorr \n");
       } else {
               // Convert the data
			   int cData = (data[1] * 256 + data[0]);
			   char red = data[3];
			   char green = data[5];
			   char blue = data[7];
//			   int red = (data[3] * 256 + data[2]);
//			   int green = (data[5] * 256 + data[4]);
//			   int blue = (data[7] * 256 + data[6]);
//               char newR = red/256;
//               char newG = green / 256;
//               char newB = blue / 256;

               // Calculate luminance
               float luminance = (-0.32466) * (red) + (1.57837) * (green)
                               + (-0.73191) * (blue);
               if (luminance < 0) {
                       luminance = 0;
               }

               // Output data to screen
               printf("Color R: %d,", red);
               printf("G : %d,", green);
               printf("B : %d \n", blue);
               printf("IR : %d [lux] \n", cData);
               printf("Ambient Light : %.2f [lux] \n", luminance);
               closestColor(red, green, blue);
       }
}

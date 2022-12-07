#include<stdio.h>
#include<stdlib.h>
#include<linux/i2c-dev.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>

int main()
{
	int file;
	char *bus = "/dev/i2c-1";

	if((file = open(bus, O_RDWR)) < 0)
	{
		printf("error to read bus\n");
		exit(1);
	}
	
	
	ioctl(file, I2C_SLAVE, 0x48);


	char config[1] = {0};
	config[0] = 0x00;
	write(file, config, 1);
	int temp, final_temp;
	unsigned char read_data[2] = {0};
	if(read(file, read_data, 2)!=2){
		printf("Error in dataread\n");
	}
	else{
		temp = ((read_data[0] << 4 ) | ( read_data[1] >> 4)); 
	}
	final_temp = temp * 0.0625; 
	
	printf("temperature in celsius %dC", final_temp ); 
	
	return 0;

}

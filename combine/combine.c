#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <signal.h>
#include <linux/i2c-dev.h>
int enable_i2c_bus_for_both_devices(){
	int file;
	char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0)
	{
		printf("error to read bus\n");
		exit(1);
	}
	ioctl(file, I2C_SLAVE, 0x48);
	ioctl(file, I2C_SLAVE, 0x76);
	return file;
}
void read_tmp(struct mq_attr attr, mqd_t mqd, int file)
{
	char tmp_buf[200];
	char config[1] = {0};
	config[0] = 0x00;
	write(file, config, 1);
	sleep(1);
	int temp, final_temp;

	unsigned char read_data[2] = {0};
	
	if(read(file, read_data, 2)!=2)
	{
		printf("Unable to read\n");
	}
	else
	{
		temp = ((read_data[0] << 4 ) | ( read_data[1] >> 4)); 
	}	
	final_temp = temp * 0.0625; 

	printf("temperature in celsius %dC\n", final_temp ); 

	memcpy(tmp_buf, &final_temp, sizeof(int));

	if(mq_send(mqd,tmp_buf,sizeof(int),1) == -1)
	{
		printf("\n\rError in sending data via message 	queue. Error: %s", strerror(errno));
	}	
}
void read_bme(struct mq_attr attr, mqd_t mqd, int file)
{
	char bme_buf[200];
	char reg[1] = {0x88};
	write(file, reg, 1);
	char b1[24] = {0};
	if(read(file, b1, 24) != 24)
	{
		printf("Error : Input/Output error \n");
		exit(1);
	}
	// Convert the data
	// temp coefficents
	int dig_T1 = (b1[0] + b1[1] * 256);
	int dig_T2 = (b1[2] + b1[3] * 256);
	dig_T2 = dig_T2 > 32767?dig_T2 - 65536:dig_T2;
	int dig_T3 = (b1[4] + b1[5] * 256);

	// Read 1 byte of data from register(0xA1)
	reg[0] = 0xA1;
	write(file, reg, 1);
	char data[8] = {0};
	read(file, data, 1);
	int dig_H1 = data[0];

	// Read 7 bytes of data from register(0xE1)
	reg[0] = 0xE1;
	write(file, reg, 1);
	read(file, b1, 7);

	// Convert the data
	// humidity coefficents
	int dig_H2 = (b1[0] + b1[1] * 256);
	int dig_H3 = b1[2] & 0xFF ;
	dig_H2 = dig_H2 > 32767?dig_H2 - 65536:dig_H2;
	int dig_H4 = (b1[3] * 16 + (b1[4] & 0xF));
	dig_H4 = dig_H4 > 32767?dig_H4 - 65536:dig_H4;
	int dig_H5 = (b1[4] / 16) + (b1[5] * 16);
	dig_H5 = dig_H5 > 32767?dig_H5 - 65536:dig_H5;
	int dig_H6 = b1[6];
	dig_H6 = dig_H6 > 127?dig_H6 - 256:dig_H6;
	// Select control humidity register(0xF2)
	// Humidity over sampling rate = 1(0x01)
	char config[2] = {0};
	config[0] = 0xF2;
	config[1] = 0x01;
	write(file, config, 2);
	// Select control measurement register(0xF4)
	// Normal mode, temp and pressure over sampling rate = 1(0x27)
	config[0] = 0xF4;
	config[1] = 0x27;
	write(file, config, 2);
	// Select config register(0xF5)
	// Stand_by time = 1000 ms(0xA0)
	config[0] = 0xF5;
	config[1] = 0xA0;
	write(file, config, 2);

	// Read 8 bytes of data from register(0xF7)
	// pressure msb1, pressure msb, pressure lsb, temp msb1, temp msb, temp lsb, humidity lsb, humidity msb
	reg[0] = 0xF7;
	write(file, reg, 1);
	read(file, data, 8);

	// Convert temperature data to 19-bits
	long adc_t = ((long)(data[3] * 65536 + ((long)(data[4] * 256) + (long)(data[5] & 0xF0)))) / 16;
	// Convert the humidity data
	long adc_h = (data[6] * 256 + data[7]);

	// Temperature offset calculations
	float var1 = (((float)adc_t) / 16384.0 - ((float)dig_T1) / 1024.0) * ((float)dig_T2);
	float var2 = ((((float)adc_t) / 131072.0 - ((float)dig_T1) / 8192.0) *
					(((float)adc_t)/131072.0 - ((float)dig_T1)/8192.0)) * ((float)dig_T3);
	float t_fine = (long)(var1 + var2);
	
	// Humidity offset calculations
	float var_H = (((float)t_fine) - 76800.0);
	var_H = (adc_h - (dig_H4 * 64.0 + dig_H5 / 16384.0 * var_H)) * (dig_H2 / 65536.0 * (1.0 + dig_H6 / 67108864.0 * var_H * (1.0 + dig_H3 / 67108864.0 * var_H)));
	float humidity = var_H * (1.0 -  dig_H1 * var_H / 524288.0);
	humidity = humidity > 100.0?100.0:humidity;
	
	humidity = humidity < 0.0?0.0:humidity;
	// Output data to screen
	printf("Relative Humidity : %.2f RH \n", humidity);
	
	memcpy(bme_buf, &humidity, sizeof(float));
	if(mq_send(mqd,bme_buf,sizeof(float),1)== -1)
	{
		printf("\n\rError in sending data via message queue. Error: %s", strerror(errno));
    	}
}

int main()
{
	struct mq_attr attr;
	mqd_t mqd;
	attr.mq_maxmsg = 10;
    	attr.mq_msgsize = sizeof(double) + sizeof(double);
	mqd = mq_open("/sendmq", O_CREAT | O_RDWR, S_IRWXU, &attr);
    	if(mqd == (mqd_t)-1)
    	{
        	printf("\n\rError in creating a message queue. Error: %s", strerror(errno));
    	}
    	int file = enable_i2c_bus_for_both_devices();
    	int i = 20;
	while(i--)
	{
		read_tmp(attr,mqd,file);
		//read_bme(attr,mqd,file);
	}
	
}

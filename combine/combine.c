#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <mqueue.h>

struct mq_attr attr;

typedef union i2c_smbus_data i2c_data;
/*void bme(){
	// Read 24 bytes of data from register(0x88)
	char reg[1] = {0x88};
	write(file, reg, 1);
	char b1[24] = {0};
	if(read(file, b1, 24) != 24)
	{
		printf("Error : Input/Output error \n");
		exit(1);
	}
	ioctl(file, I2C_SLAVE, 0x76);
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
	double var1 = (((double)adc_t) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
	double var2 = ((((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0) *
					(((double)adc_t)/131072.0 - ((double)dig_T1)/8192.0)) * ((double)dig_T3);
	double t_fine = (long)(var1 + var2);
	
	// Humidity offset calculations
	double var_H = (((double)t_fine) - 76800.0);
	var_H = (adc_h - (dig_H4 * 64.0 + dig_H5 / 16384.0 * var_H)) * (dig_H2 / 65536.0 * (1.0 + dig_H6 / 67108864.0 * var_H * (1.0 + dig_H3 / 67108864.0 * var_H)));
	double humidity = var_H * (1.0 -  dig_H1 * var_H / 524288.0);
	humidity = humidity > 100.0?100.0:humidity;
	humidity = humidity < 0.0?0.0:humidity;
	// Output data to screen
	memcpy(sensor_buffer, &humidity, sizeof(double));
}*/
int main()
{
	int file;
	char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		printf("value of file %d",file);
		exit(1);
	}
	mqd_t mqd;
    	char sensor_buffer[sizeof(int)+sizeof(int)+20] = {0};
    	attr.mq_maxmsg = 10;
    	attr.mq_msgsize = (sizeof(int)+sizeof(int)+40);
    	mqd = mq_open("/sendmq", O_CREAT | O_RDWR, S_IRWXU, &attr);
    	if(mqd == (mqd_t)-1)
    	{
        printf("\n\rError in creating a message queue. Error: %s", strerror(errno));
    	}
while(1)
{
	/*tmp102*/
	ioctl(file, I2C_SLAVE, 0x48);

	char configT[1] = {0};
	configT[0] = 0x00;
	write(file, configT, 1);
	sleep(1);

	double temp, final_temp;
	unsigned char read_data[2] = {0};
	
	if(read(file, read_data, 2)!=2)
	{
		printf("Error in dataread\n");
	}
	else
	{
		temp = ((read_data[0] << 4 ) | ( read_data[1] >> 4)); 
	}

	final_temp = temp * 0.0625;
	int temp_int = (int)final_temp;
	//printf("temperature in celsius %f C\n", final_temp ); 
	memcpy(sensor_buffer, &temp_int, sizeof(int));
	printf("sensor_buffer value = %s",sensor_buffer);
	if(mq_send(mqd, sensor_buffer, sizeof(int), 1) == -1)
    	{
    	    printf("\n\rError in sending data via message queue. Error: %s", strerror(errno));
    	}
    	memset(sensor_buffer,0,sizeof(sensor_buffer));
    	sleep(100);
}


}

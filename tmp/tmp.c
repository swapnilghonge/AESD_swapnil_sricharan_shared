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

int main()
{
	
	int file;
	char *bus = "/dev/i2c-1";

	if((file = open(bus, O_RDWR)) < 0)
	{
		printf("error to read bus\n");
		exit(1);
	}
	mqd_t mqd;
    	char sensor_buffer[sizeof(float)];
    	attr.mq_maxmsg = 10;
    	attr.mq_msgsize = sizeof(float);
    	mqd = mq_open("/sendmq", O_CREAT | O_RDWR, S_IRWXU, &attr);
    	if(mqd == (mqd_t)-1)
    	{
        printf("\n\rError in creating a message queue. Error: %s", strerror(errno));
    	}
    	
	while(1)
	{
	ioctl(file, I2C_SLAVE, 0x48);


	char config[1] = {0};
	config[0] = 0x00;
	write(file, config, 1);
	sleep(1);
	
	

	int temp, final_temp;
	
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
	
	//printf("temperature in celsius %dC", final_temp ); 
	}
	mqd_t mqd;
    	char sensor_buffer[sizeof(float)];
    	attr.mq_maxmsg = 10;
    	attr.mq_msgsize = sizeof(float);
    	mqd = mq_open("/sendmq", O_CREAT | O_RDWR, S_IRWXU, &attr);
    	if(mqd == (mqd_t)-1)
    	{
        printf("\n\rError in creating a message queue. Error: %s", strerror(errno));
    	}
    	
    	memcpy(sensor_buffer, &final_temp, sizeof(float));
	if(mq_send(mqd, sensor_buffer, sizeof(float), 1) == -1)
    	{
    	    printf("\n\rError in sending data via message queue. Error: %s", strerror(errno));
    	}
    	sleep(1);
	
}

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


struct mq_attr attr;

typedef union i2c_smbus_data i2c_data;

int main()
{
	mqd_t mqd;
	char sensor_buffer[sizeof(double) + sizeof(double)];
    	attr.mq_maxmsg = 10;
    	attr.mq_msgsize = sizeof(double) + sizeof(double);
    	mqd = mq_open("/sendmq", O_CREAT | O_RDWR, S_IRWXU, &attr);
    	if(mqd == (mqd_t)-1)
    	{
        printf("\n\rError in creating a message queue. Error: %s", 				strerror(errno));
    	}
    	
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
	memcpy(sensor_buffer, &final_temp, sizeof(double));
	
	printf("temperature in celsius %dC", final_temp ); 
	if(mq_send(mqd, sensor_buffer, sizeof(double) + sizeof(double), 1) == -1)
    	{
    	    printf("\n\rError in sending data via message queue. Error: %s", strerror(errno));
    	}
	return 0;

}

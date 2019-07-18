#include <stdint.h>  
#include <unistd.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <getopt.h>  
#include <fcntl.h>  
#include <sys/ioctl.h>  
#include <linux/types.h>  
#include <linux/spi/spidev.h>  
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
int set_opt(int,int,int,char,int);
int ctrl_creat(int fd,int velocity,int radius);
int open_usb_serial(int fd);
int open_creat_interface(int fd);
int goto_safe_mode(int fd);	
int close_creat_interface(int fd);
int seek_dock(int fd);	
int default_clean(int fd);	
int power_down(int fd);
int show_battery(int fd);
int get_distance_and_angle(int fd);

static const char *usb_name = "/dev/ttyUSB0"; 
int distance=0;
int angle=0;
int main(int argc, char *argv[])  
{  
	int fd = 0;
	int i = 0;
	fd = open_usb_serial(fd);	//	打开，连接usb
	open_creat_interface(fd);	//	打开接口
	show_battery(fd);			//	查电量
	//seek_dock(fd);
	//default_clean(fd);
	goto_safe_mode(fd);			//	进入安全模式
        ctrl_creat(fd,100,0);		//	100mm/s直线行走
	sleep(3);
	ctrl_creat(fd,100,200);		//	速度100mm/s,半径200mm行走
	sleep(3);
	ctrl_creat(fd,0,0);			//	停止
	sleep(1);
	for(i = 0;i<10;++i)
	{
		ctrl_creat(fd,100,0);	
		get_distance_and_angle(fd);
		usleep(500000);
	}
	for(i = 0;i<10;++i)
	{
		ctrl_creat(fd,100,200);	
		get_distance_and_angle(fd);
		usleep(500000);
	}
	//power_down(fd);
	close_creat_interface(fd);
	printf("close\n");
	return 0;
}
int open_creat_interface(int fd)	//打开creater接口
{
	char write_buf = 0x80;			//第7页		Start Interface	Opcode:128
	write(fd, &write_buf, 1);
	return 0;
}
int goto_safe_mode(int fd)			//进入安全模式（安全模式下机器不会睡眠，充电也冲不进去）
{
	char write_buf = 0x82;			//第9页		Safe  Mode		Opcode:130 OR 131
	write(fd, &write_buf, 1);
	usleep(20000);					//发现进入安全模式需要20ms的启动时间，原因我就不知道了。
	return 0;
}
int close_creat_interface(int fd)	//关闭creater接口保护电池
{
	char write_buf = 0xAD;			//第7页		Stop Interface	Opcode:173
	write(fd, &write_buf, 1);
	return 0;	
	//省电 在被动模式下，Roomba将在5分钟不活动后睡眠，以保持电池电量。 在安全和 全模式，Roomba将永远不会睡觉，如果长时间保持在这个状态，会深深的 放电，即使插入充电器。 充电器将在所有模式下为Roomba供电，但是 它不会在安全或完全模式下为电池充电。 当你完成工作时很重要 Roomba，您发出被动或停止命令来保护电池。	
	//Power Saving
	/*
			In Passive mode, Roomba will sleep after 5 minutes of inactivity to preserve battery power. In Safe and
		Full modes, Roomba will never sleep, and if left in this state for an extended period of time, will deeply
		discharge its battery, even if plugged into the charger. The charger will power Roomba in all modes, but
		it will not charge the battery in Safe or Full mode. It is important that when you are finished working with
		Roomba that you issue either the Passive or Stop command to protect the battery.
	*/
}
int show_battery(int fd)			//查询此刻电量
{
	char write_buf[4] = {0};
	char read_buf[4] = {0};
	write_buf[0]=0x95;				//第20页	Query List		Opcode:149
	write_buf[1]=0x02;				//查询2个参数
	write_buf[2]=0x19;				//第38页	packet 25		当前可用电量 mAh
	write_buf[3]=0x1A;				//第38页	packet 26		电池总电量 mAh
	write(fd, write_buf, 4);
	read(fd,read_buf,4);
	short s_battery_charge = 0;		//0x19
	short s_battery_capacity = 0;	//0x2A
	char  c_battery_charge[2]={0};
	char  c_battery_capacity[2]={0};
	c_battery_charge[0]=read_buf[1];
	c_battery_charge[1]=read_buf[0];
	c_battery_capacity[0]=read_buf[3];  
	c_battery_capacity[1]=read_buf[2];
	memcpy(&s_battery_charge,&c_battery_charge[0], 2);
	memcpy(&s_battery_capacity,&c_battery_capacity[0], 2);	
	printf("battery_charge=%d\nbattery_capacity=%d\n",(int)s_battery_charge,(int)s_battery_capacity);
	return 0;	
}
int seek_dock(int fd)				//寻找充电桩
{
	char write_buf=0x8F;			//第10页	Seek Dock		Opcode:143
	write(fd, &write_buf, 1);	
	return 0;	
}
int default_clean(int fd)			//默认清扫模式
{
	char write_buf=0x87;			//第10页	Clean			Opcode:135
	write(fd, &write_buf, 1);
	return 0;	
}
int power_down(int fd)				//关机
{
	char write_buf=0x85;			//第10页	Clean			Opcode:133
	write(fd, &write_buf, 1);	
	return 0;	
}
									//驱动creater行走	velocity驱动轮的平均速度mm/s  radius转动的半径mm	 
int ctrl_creat(int fd,int velocity,int radius)
{
	char write_buffer[5]={0};
	write_buffer[0] = (unsigned char)0X89;		//第12页	Drive	Opcode:137
	char buf[4];
	short s_velocity = 0;
	short s_radius = 0;
	s_velocity = (short)velocity;
	s_radius = (short)radius;
	memcpy(&buf[0], &s_velocity, 2);
	memcpy(&buf[2], &s_radius, 2);
	write_buffer[1] = buf[1];
	write_buffer[2] = buf[0];
	write_buffer[3] = buf[3];    
	write_buffer[4] = buf[2]; 
	write(fd, write_buffer, 5);	
	return 0;	
}
									//获取位移变化量和角度变化量
int get_distance_and_angle(int fd)
{
	char write_buf[4] = {0};
	char read_buf[4] = {0};
	write_buf[0]=0x95;					//第20页	Query List		Opcode:149 
	write_buf[1]=0x02;					//查询2个参数      
	write_buf[2]=0x13;					//第38页	packet 19		Distance   
	write_buf[3]=0x14;					//第38页	packet 20		Angle  
	write(fd, write_buf, 4);	
	read(fd,read_buf,4);
	
	short s_distance = 0;
	short s_angle = 0;
	char  c_distance[2]={0};
	char  c_angle[2]={0};
	c_distance[0]=read_buf[1];
	c_distance[1]=read_buf[0];
	c_angle[0]=read_buf[3];  
	c_angle[1]=read_buf[2];
	memcpy(&s_distance,&c_distance[0], 2);
	memcpy(&s_angle,&c_angle[0], 2);
	if((int)s_distance<80&&(int)s_distance>-80)
	{
		distance+=(int)s_distance;
	}
	else
	{
		s_distance=0;
	}
	if((int)s_angle<80&&(int)s_angle>-80)
	{
		angle+=(int)s_angle;
	}
	else
	{
		s_angle=0;
	}
	printf("distance:%d	angle:%d\nsum_dis=%d	sum_ang=%d\n",(int)s_distance,(int)s_angle,distance,angle);
	return 0;	
}

//打开creater usb串口
int open_usb_serial(int fd)
{
	int ret_creat = 0; 
	fd = open(usb_name,O_RDWR|O_NOCTTY); 
	if(fd == -1)  
	{  
		printf("%s open failed\n",usb_name);  
	}  
	else  
	{  
		usleep(300000);
		printf("%s open success\n",usb_name);  
		ret_creat = set_opt(fd,115200,8,'N',1);  
		if(ret_creat == -1)  
		{ 
			printf("set_opt err\n"); 
			exit(-1);  
		} 
	} 	
	return fd;
}
//设置usb串口
int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop)  
{  
	struct termios newtio,oldtio;  
	if(tcgetattr(fd,&oldtio)!=0)  
	{  
		perror("error:SetupSerial 3\n");  
		return -1;  
	}  
	bzero(&newtio,sizeof(newtio));  
	//使能串口接收  
	newtio.c_cflag |= CLOCAL | CREAD;  
	newtio.c_cflag &= ~CSIZE;  

	newtio.c_lflag &=~ICANON;//原始模式    

	//newtio.c_lflag |=ICANON; //标准模式   

	//设置串口数据位  
	switch(nBits)  
	{  
		case 7:  
			newtio.c_cflag |= CS7;  
			break;  
		case 8:  
			newtio.c_cflag |=CS8;  
			break;  
	}  
	//设置奇偶校验位  
	switch(nEvent)  

	{  
		case 'O':  
			newtio.c_cflag |= PARENB;  
			newtio.c_cflag |= PARODD;  
			newtio.c_iflag |= (INPCK | ISTRIP);  
			break;  
		case 'E':  
			newtio.c_iflag |= (INPCK | ISTRIP);  
			newtio.c_cflag |= PARENB;  
			newtio.c_cflag &= ~PARODD;  
			break;  
		case 'N':  
			newtio.c_cflag &=~PARENB;  
			break;  
	}  
	//设置串口波特率  
	switch(nSpeed)  
	{  
		case 2400:  
			cfsetispeed(&newtio,B2400);  
			cfsetospeed(&newtio,B2400);  
			break;  
		case 4800:  
			cfsetispeed(&newtio,B4800);  
			cfsetospeed(&newtio,B4800);  
			break;  
		case 9600:  
			cfsetispeed(&newtio,B9600);  
			cfsetospeed(&newtio,B9600);  
			break;  
		case 115200:  
			cfsetispeed(&newtio,B115200);  
			cfsetospeed(&newtio,B115200);  
			break;  
		case 460800:  
			cfsetispeed(&newtio,B460800);  
			cfsetospeed(&newtio,B460800);  
			break;  
		default:  
			cfsetispeed(&newtio,B9600);  
			cfsetospeed(&newtio,B9600);  
			break;  
	}  
	//设置停止位  
	if(nStop == 1)  
		newtio.c_cflag &= ~CSTOPB;  
	else if(nStop == 2)  
		newtio.c_cflag |= CSTOPB;  
	newtio.c_cc[VTIME] = 5;  
	newtio.c_cc[VMIN] = 0;  
	tcflush(fd,TCIFLUSH);  

	if(tcsetattr(fd,TCSANOW,&newtio)!=0)  
	{  
		perror("com set error\n");  
		return -1;  
	}  
	return 0;  
} 

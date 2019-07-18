#include "Create2.h"


int distance=0;
int angle=0;

int32_t open_usb_serial(int32_t fd)
{
        int32_t ret_creat = 0;
        const char *usb_name = "/dev/ttyUSB0";
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

int32_t open_creat_interface(int32_t fd)
{
        char write_buf = 0x80;			//第7页		Start Interface	Opcode:128
        write(fd, &write_buf, 1);
        return 0;
}

int32_t close_creat_interface(int32_t fd)	//关闭creater接口保护电池
{
        char write_buf = 0xAD;			//第7页		Stop Interface	Opcode:173
        write(fd, &write_buf, 1);
        return 0;
}


int32_t send_cmd(int32_t fd,int32_t velocity,int32_t radius)
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

int32_t set_opt(int32_t fd,int32_t nSpeed,int32_t nBits,char nEvent,int32_t nStop)
{
        struct termios newtio,oldtio;
        if(tcgetattr(fd,&oldtio)!=0)
        {
                perror("error:SetupSerial 3\n");
                return -1;
        }
        bzero(&newtio,sizeof(newtio));
        //enable the serial port
        newtio.c_cflag |= CLOCAL | CREAD;
        newtio.c_cflag &= ~CSIZE;

        newtio.c_lflag &=~ICANON;  //original mode

        //newtio.c_lflag |=ICANON; //standard mode

        //set the data bits of serial port
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
        //set the Baud rate
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

int32_t goto_safe_mode(int32_t fd)	// in safe mode, robot would not sleep, charging function is disabled
{
        char write_buf = 0x82;
        write(fd, &write_buf, 1);
        usleep(20000);	       // wait 20ms for the safe mode to be enabled
        return 0;
}

int32_t get_distance_and_angle(int32_t fd)
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
        if((int32_t)s_distance<80&&(int32_t)s_distance>-80)
        {
                distance+=(int32_t)s_distance;
        }
        else
        {
                s_distance=0;
        }
        if((int32_t)s_angle<80&&(int32_t)s_angle>-80)
        {
                angle+=(int32_t)s_angle;
        }
        else
        {
                s_angle=0;
        }
        printf("distance:%d	angle:%d\nsum_dis=%d	sum_ang=%d\n",(int)s_distance,(int)s_angle,distance,angle);
        return 0;
}

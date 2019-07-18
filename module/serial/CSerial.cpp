/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	File name:	CTcpSocket.h
//	descripton:	modus 协议转换类,根据统一报文格式封装
//	
//	
//	version:
//	create:	Date		by
//		2005/1/19	xxx
//		................	......
//	modify:	data	by
//		2005/1/19	xxx
//		...............		......
//	
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/times.h>
#include <fcntl.h>
#include <string.h>
#include "CSerial.h"


int_32 set_cmd(uint_8 *cmd, int_16  line_vel, int_16  angular_vel)
{

    int_16 Radius;

    if (abs(angular_vel) < 1e-5)
        Radius = 0;
    else
        Radius = (2*line_vel/angular_vel+ WHEEL_BASE)/2;

    if((0 == Radius&&line_val!=0)||(line_val!=0&&0 != Radius)||(0 == Radius&&line_val==0))
    {
            cmd[0] = (unsigned char)0X89;		//第12页	Drive	Opcode:137
            char buf[4];
            memcpy(&buf[0], &line_vel, 2);
            memcpy(&buf[2], &Radius, 2);
            cmd[1] = buf[1];
            cmd[2] = buf[0];
            cmd[3] = buf[3];
            cmd[4] = buf[2];
    }
    else
    {
            cmd[0] = (unsigned char)0X91;		//第12页	Drive	Opcode:145
            char buf[4];
            short left_wheel = -angular_vel*WHEEL_BASE/2;
            short right_wheel = angular_vel*WHEEL_BASE/2;
            memcpy(&buf[0], &left_wheel, 2);
            memcpy(&buf[2], &right_wheel, 2);
            cmd[1] = buf[1];
            cmd[2] = buf[0];
            cmd[3] = buf[3];
            cmd[4] = buf[2];
    }
    return 0;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CSerial::CSerial()
{
	m_nFdcom        = 0 ;
	m_chrPrompt      = '0' ;
	m_nBaudrate      = 115200 ;
	m_chrDatabit      = '8' ;
	m_chrDebug      = '0' ;
	m_chrEcho       = '0' ;
	m_chrFctl        = '2' ;
	m_chrTty  	= '0' ;
	m_chrParity      = '0' ;
	m_chrStopbit     = '1' ;
	m_nReserved     = 0 ;

	m_nRecvLen = 0 ;
	m_nSendLen = 0 ;

	uTtyDelay=10*1000000/m_nBaudrate; //PramTable.Baudrate;

	bzero(m_chrSendBuffer,sizeof(m_chrSendBuffer)) ;
	bzero(m_chrRecvBuffer,sizeof(m_chrRecvBuffer));

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CSerial:: CSerial(int fdcom,char prompt,int baudrate,char databit,char debug,char echo,char fctl,char tty,char parity,char stopbit)
{
	m_nFdcom        = fdcom ;
	m_chrPrompt      = prompt ;
	m_nBaudrate      = baudrate ;
	m_chrDatabit      = databit ;
	m_chrDebug      = debug ;
	m_chrEcho       = echo ;
	m_chrFctl        = fctl ;
	m_chrParity      = parity ;
	m_chrStopbit     = stopbit ;
	m_nReserved     = 0 ;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CSerial::~CSerial()
{
//	printf("CSerial destruct\n ") ;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int CSerial::convBaud(unsigned long int baudrate)
{
	switch(baudrate){
		case 1200:
			return B1200;
		case 2400:
			return B2400;
		case 4800:
			return B4800;
		case 9600:
			return B9600;
		case 19200:
			return B19200;
		case 38400:
			return B38400;
		case 57600:
			return B57600;
		case 115200:
			return B115200;
		default:
			return B9600;
	}
	return 0 ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
char* CSerial::getTty(char tty)
{
	char *ptty;
	ptty = 0 ;
	switch(tty){
		case '0':{
					 ptty = (char*)TTY_DEV"0";
				 }break;
		case '1':{
					 ptty = (char*)TTY_DEV"1";
				 }break;
		case '2':{
					 ptty = (char*)TTY_DEV"2";
				 }break;
	}
	return(ptty);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//pxro_serial->setPort(0, 115200, 8, '0', '0', '0', 0, 1);
int CSerial::setPort(char prompt,int baudrate,char databit,char debug,char echo,char fctl,char parity,char stopbit)
{
	struct	termios termios_old; 
	struct	termios termios_new;
	int	tmp;
	//m_nFdcom = fdcom ;
	bzero(&termios_old, sizeof(termios_old));
	bzero(&termios_new, sizeof(termios_new));
	cfmakeraw(&termios_new);
	tcgetattr(m_nFdcom, &termios_old);			//get the CSerial port attributions
	//baudrates
	m_nBaudrate = convBaud(baudrate);
	cfsetispeed(&termios_new, m_nBaudrate);
	cfsetospeed(&termios_new, m_nBaudrate);
	termios_new.c_cflag |= CLOCAL;
	termios_new.c_cflag |= CREAD;
	m_chrFctl = fctl;
	switch(fctl){
		case '0':{
					 termios_new.c_cflag &= ~CRTSCTS;		//no flow control
				 }break;
		case '1':{
					 termios_new.c_cflag |= CRTSCTS;			//hardware flow control
				 }break;
		case '2':{
					 termios_new.c_iflag |= IXON | IXOFF |IXANY;	//software flow control
				 }break;
	}
	termios_new.c_cflag &= ~CSIZE;
	m_chrDatabit = databit;
	switch(databit){
		case '5':{
					 termios_new.c_cflag |= CS5;
				 }break;
		case '6':{
					 termios_new.c_cflag |= CS6;
				 }break;
		case '7':{
					 termios_new.c_cflag |= CS7;
				 }break;
		case '8':{
					 termios_new.c_cflag |= CS8;
				 }break;
	}
	m_chrParity = parity;
	switch(parity){
		case '0':{
					 termios_new.c_cflag &= ~PARENB;		//no parity check
					 termios_new.c_iflag &= ~INPCK; 
				 }break;
		case '2':{
					 termios_new.c_cflag |= PARENB;		//even check
					 termios_new.c_cflag &= ~PARODD;
					 //	termios_new.c_iflag |= (INPCK|ISTRIP); 
					 termios_new.c_iflag |= INPCK; 
				 }break;

		case '1':{
					 termios_new.c_cflag |= PARENB;		//odd check
					 //	termios_new.c_cflag |=PARODD;
					 //	termios_new.c_iflag |=(INPCK|ISTRIP);  
					 termios_new.c_iflag |= INPCK; 
				 }break;
	}
	m_chrStopbit = stopbit;
	if(stopbit == '2'){
		termios_new.c_cflag |= CSTOPB;	//2 stop bits
	}
	else{
		termios_new.c_cflag &= ~CSTOPB;	//1 stop bits
	}
	//other attributions default
	termios_new.c_oflag &= ~OPOST;
	termios_new.c_cc[VMIN]  = 0;
	termios_new.c_cc[VTIME] = 1;
	tcflush(m_nFdcom, TCIFLUSH);
	tmp = tcsetattr(m_nFdcom, TCSANOW, &termios_new);
	tcgetattr(m_nFdcom, &termios_old);
	return(tmp);
}






int CSerial::set_opt(int fd,int nSpeed,int nBits,unsigned char nEvent,int nStop)  
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

	//newtio.c_cc[VTIME] = 50;  
	//newtio.c_cc[VMIN] = 40;  

	newtio.c_cc[VTIME] = 0;  
	newtio.c_cc[VMIN] = 1;  

	tcflush(fd,TCIFLUSH);  

	if(tcsetattr(fd,TCSANOW,&newtio)!=0)  
	{  
		perror("com set error\n");  
		return -1;  
	}  
	return 0;  
} 




/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CSerial::openPort()
{
	char *ptty;
	ptty = getTty(m_chrTty);
	m_nFdcom = open(ptty, O_RDWR ); //| O_NOCTTY | O_NONBLOCK);
//	printf("m_nFdcom: %d \n",m_nFdcom);
//	if(m_nFdcom<0)
//	{
//		printf("Error: open CSerial [%s] error.\n",ptty);
//		exit(1);
//	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void CSerial::openPort(char tty)
{
	char *ptty;
	m_chrTty = tty ;
	ptty = getTty(m_chrTty);
	//        m_nFdcom = open(ptty, O_RDWR ); //| O_NOCTTY | O_NONBLOCK);
	m_nFdcom = open(ptty, O_RDWR | O_NOCTTY | O_NONBLOCK);
	printf("m_nFdcom: %d \n",m_nFdcom);
	if(m_nFdcom<0)
	{
		printf("Error: open CSerial [%s] error.\n",ptty);
		exit(1);
	}
}

*/
void CSerial::openPort(char tty)
{
	char *ptty;
	switch(tty)
	{
		case '0':{
				ptty = (char *)"/dev/ttyUSB0";
				break;
			}
		case '1':{
				ptty = (char *)"/dev/ttyUSB1";
				break;	
			}
		case '2':{
				ptty = (char *)"/dev/ttyUSB2";
				break;
			}

		default:	break;

	}

	m_nFdcom = open( ptty, O_RDWR | O_NOCTTY | O_NONBLOCK);
	//m_nFdcom = open(ptty, O_RDWR | O_NOCTTY );

//	printf("m_nFdcom: %d [%s]\n",m_nFdcom,ptty);
	if(m_nFdcom<0)
	{
		printf("Error: open CSerial [%s] error.\n",ptty);
		exit(1);
	}


	/*
		if(-1 ==  set_opt(m_nFdcom,115200,8,'N',1) )
		{  
			printf("Error: set_opt error.\n");
			exit(-1);  
		}  

	*/



}






/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CSerial::closePort()
{
	close(m_nFdcom);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
//////////////////////////////////////////////////////////////////////////////////////////d///////////////
int CSerial::sendPort(unsigned char *data,int datalen)
{

	int len = 0;

//	printf("Send to Serial Package:[") ;
//	for(int i = 0 ;i<datalen;i++)
//		printf(" 0x%x ",data[i]) ;
//	printf("]\n") ;


	len = write(m_nFdcom, data, datalen);
	if(len == datalen)
	{
		// printf("len= %d \n",len);
		return (len);
	}
	else{
		tcflush(m_nFdcom, TCOFLUSH);
		return -1;
	}	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int CSerial::recvPort(unsigned char *data,int datalen)
{
	int readlen=0;
	//int inComLen;
	int fs_sel;
	fd_set fs_read;
	struct timeval tv_timeout;
	FD_ZERO(&fs_read);
	FD_SET(m_nFdcom, &fs_read);
	tv_timeout.tv_sec = 0;
        tv_timeout.tv_usec = 32*uTtyDelay;

	readlen = read(m_nFdcom, data, datalen);
	if (readlen==-1)
	{
//		usleep(10000);
		fs_sel = select(m_nFdcom+1, &fs_read, NULL, NULL, &tv_timeout);
		//TODO
		if(fs_sel)
			;
		readlen = read(m_nFdcom, data, datalen);
		
	}

//              printf("readlen= %d \n",readlen);
//   		printf("readdata= %s \n",data);
		return(readlen);

}






/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CSerial::getDataFromSerial()
{
	//局部变量
	int  	RecvLen;		//接收长度
	unsigned char chrportinfo[2];//临时缓冲区
	int 	recvtime ;	//接收次数

	//局部变量初始化	
	recvtime = 0 ;		
	RecvLen = 0 ;

	while(1)
	{
		//		printf("begin read COM\n");
		RecvLen = recvPort(chrportinfo,1);
		if (RecvLen == -1) break ; 

		if (recvtime>500) break;

		m_chrRecvBuffer[recvtime] = chrportinfo[0] ;			  		
		recvtime++;

	}
	m_nRecvLen = recvtime ;
//	if(m_nRecvLen)
//	{
//		printf("Successfully,Read From Serial,All length = [%d]\n",recvtime) ;
//		printf("recv length: %d\n",m_nRecvLen);
//	}
//	else
//		printf("Unsuccessfully,Read From Serial,Failure!\n") ;



}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int CSerial::getSendLen()
{
	return m_nSendLen ;	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int CSerial::getRecvLen()
{
	return m_nRecvLen ;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char* CSerial::getSendBuffer()
{
	return m_chrSendBuffer ;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char* CSerial::getRecvBuffer()
{
	return m_chrRecvBuffer ;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CSerial::setSendLen(int len)
{
	m_nSendLen = len ;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	名称:	
//	功能:	
//	参数:	
//	返回值:	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CSerial::setRecvLen(int len)
{
	m_nRecvLen = len ;
}


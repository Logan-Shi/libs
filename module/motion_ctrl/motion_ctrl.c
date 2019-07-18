#include "xrobot.h"
#include <math.h>
#include <termios.h> // clear buffer
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int_32 set_cmd(uint_8 *cmd, int_32  line_vel, int_32  angular_vel)
{
    int i;
    uint_8 temp_sum[8]={0};
    cmd[0] = (uint_8)0xAA;cmd[1] = (uint_8)0xAA;											//00-01 头

    cmd[2] = (uint_8)0x12;																//02    设备地址
    cmd[3] = (uint_8)0x02;																//03    功能码
    cmd[4] = (uint_8)0x00;cmd[5] = (uint_8)0x00;											//04-05 起始地址
    cmd[6] = (uint_8)0x04;cmd[7] = (uint_8)0x00;cmd[8] = (uint_8)0x00;cmd[9] = (uint_8)0x00;	//06-09 要读写的数据长度
    uint_8 *p = (uint_8*)&line_vel;
    memcpy(&cmd[10], p, sizeof(int));
    //10-13 线速度
    for(i = 10; i < 14; i++)
    {
        if((uint_8)0xA5 == cmd[i])
            cmd[i] -= 1;
    }
    int  check1 = 18+2+0+4+(uint_8)cmd[10]+(uint_8)cmd[11]+(uint_8)cmd[12]+(uint_8)cmd[13];
    p = (uint_8*)&check1;
    memcpy(temp_sum, p, sizeof(int));
    cmd[14] = temp_sum[0];																//14-15 校验码

    cmd[15] = (uint_8)0x55;cmd[16] = (uint_8)0x55;											//16	尾
    //=================================================================================================================
    cmd[17] = (uint_8)0xAA;cmd[18] = (uint_8)0xAA;

    cmd[19] = (uint_8)0x12;
    cmd[20] = (uint_8)0x02;
    cmd[21] = (uint_8)0x04;cmd[22] = (uint_8)0x00;
    cmd[23] = (uint_8)0x04;cmd[24] = (uint_8)0x00;cmd[25] = (uint_8)0x00;cmd[26] = (uint_8)0x00;
    //printf("string cmd input: %d\n", angular_vel);
    p = (uint_8*)&angular_vel;
    memcpy(&cmd[27], p, sizeof(int));
    for(i = 27; i < 31; i++)
    {
        if((uint_8)0xA5 == cmd[i])
            cmd[i] -= 1;
    }
    //printf("string cmd set: %d，", cmd[27]);
    //printf("%d, %d, %d\n", cmd[28], cmd[29], cmd[30]);

    int  check2 = 18+2+4+4+(uint_8)cmd[27]+(uint_8)cmd[28]+(uint_8)cmd[29]+(uint_8)cmd[30];
    p = (uint_8*)&check2;
    memcpy(temp_sum, p, sizeof(int));
    cmd[31] = temp_sum[0];

    cmd[32] = (uint_8)0x55;cmd[33] = (uint_8)0x55;
    return 0;
}

int set_opt(int fd,int nSpeed,int nBits,uint_8 nEvent,int nStop)
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
    newtio.c_cc[VTIME] = 50;
    newtio.c_cc[VMIN] = 40;
    tcflush(fd,TCIFLUSH);

    if(tcsetattr(fd,TCSANOW,&newtio)!=0)
    {
        perror("com set error\n");
        return -1;
    }
    return 0;
}


int buff_combination(uint_8 * pre_buf, int pre_len, uint_8* cur_buf, int cur_len, uint_8 *msg)
{
    int i, j;
    uint_8 buf[256]; //用于存储连接两个字符数组的临时数组

    if(pre_len < 1 || cur_len < 1)
    {
        printf("Error length from parameters,pre_len = %d, cur_len = %d\n ", pre_len, cur_len);
    }
    for(i = 0; i < pre_len; i++)
    {
        buf[i] = pre_buf[i];
    }
    for(j = 0; j < cur_len; j++)
    {

        buf[i++] = cur_buf[j];
    }
    buf[i] = '\0';
    /*
           printf("\ncomb: ");
           for(j = 0; j < i; j++)
           printf("%d ", buf[j]);
           printf("\n");
         */
    //找到字符串的起始位
    //    printf("Combination: 002, i = %d\n", i);
    for(i = 0;;)
    {
        if((buf[i] == (uint_8)0xAA) && (buf[i + 1] == (uint_8)0xAA) && (buf[i + 2] == (uint_8)0x12))
            break;
        else
            i++;
    }
    //    printf("Combination: 003, i = %d\n", i);
    //	printf("pos = %d\n", i);

    for( j = 0; j < XROMSGLENTH + 1;)
    {
        if(buf[i]==(uint_8)0xA5 && (buf[i+1]==(uint_8)0xA5 ||buf[i+1] == (uint_8)0xAA || (buf[i+1]== (uint_8)0x55 &&buf[i+2]!= (uint_8)0x55)))
            i++;
        else
        {
            if(buf[i] == (uint_8)0x55 && buf[i + 1] == (uint_8)0x55)
            {
                msg[j++] = (uint_8)0x55;
                msg[j++] = (uint_8)0x55;
                return j;//break;
            }
            msg[j++] = buf[i++];
        }

        if(i >= (pre_len + cur_len))
            return -2;
    }
    printf("Combination: 004, j = %d\n",j);
    msg[j] = '\0';

    return j;

}


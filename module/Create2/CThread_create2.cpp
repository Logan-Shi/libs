/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File name: 
//	descripton:
//	version:
//	create:	Date		by
//			2017/4/12	Darren
//		................	......
//	modify:	date		by
//			2017/4/12	Darren
//		...............		......
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////

//#pragma once

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include "CSerial.h"
#include "types.h"
#include "create2.h"
#include "serial.h"
#include "CThread_create2.h"

#include <iomanip>
#include <iostream>

using namespace std;

CThread_create2::CThread_create2()
{
    create2_read_len = 0;
    create2_write_len = 0;
}

CThread_create2::~CThread_create2()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void* CThread_create2::process_create2(void *)
{

    fd_set fs_read;
    int fs_sel;
    int i;
    //struct timeval tv_timeout;

    CSerial *pcreate2_serial;

    pcreate2_serial = new CSerial();
    if(NULL == pcreate2_serial)
    {
        return (void *)-1;
    }
    pcreate2_serial->openPort('1');
    pcreate2_serial->setPort(0, 115200, '8', '0', '0', '0', '0', '1');
    pcreate2_serial->uTtyDelay = 10*1000000/115200;

    //线程处理
    while(1)
    {
        // TODO
        //tv_timeout.tv_sec = 0;
        // tv_timeout.tv_usec = 50000;
        //
        FD_ZERO(&fs_read);
        FD_SET(pcreate2_serial->m_nFdcom, &fs_read);
        FD_SET(0, &fs_read);
        //cout << "Process Xro 001" << endl;
        fs_sel = select(pcreate2_serial->m_nFdcom + 1, &fs_read, NULL, NULL, NULL);
        if(-1 == fs_sel)
        {
            ; //TODO
        }
//cout << "Process Xro 001" << endl;
        if(FD_ISSET(pcreate2_serial->m_nFdcom, &fs_read))
        {
            pcreate2_serial->getDataFromSerial();

            int decode_ret = decode_create2(pcreate2_serial->m_chrRecvBuffer, pcreate2_serial->m_nRecvLen, px_msg_struct);
            //#ifdef PRINT_XRO_DEBUG_INFO
            if(decode_ret < 0)

            {
                printf("Recvd form Xrobot:");
                printf("len=%d, Decode return value is %d, update = %d\n", pcreate2_serial->m_nRecvLen, decode_ret, px_msg_struct->update);
                for(i = 0; i < pcreate2_serial->m_nRecvLen; i++)
                    printf("%2x  ", pcreate2_serial->m_chrRecvBuffer[i]);
                std::cout << std::endl;
                //		print_xrobot(px_msg_struct);
            }
            //#endif
        }
        if(create2_cmd_update)
        {
            int ret_val =  pcreate2_serial->sendPort(create2_cmd_string, 34);
            if(ret_val == 34)
            {
                printf("send cmd to xrobot OK\n");
            }
            else
            {
                printf("here005: send cmd to xrobot error\n");
            }
            create2_cmd_update = 0;
        }
        else
        {
            ;
        };
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CThread_create2::create(pthread_t &tid,pthread_attr_t *attr,int sock)
{
    if(pthread_create(&tid,attr,process_create2,&sock))
    {
        printf("CThread Created Error \n") ;
        exit(1) ;
    }
    else
    {
        printf("Xrobot thread create successfully.\n");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CThread_create2::exit(int)
{
    pthread_exit(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
int_32 CThread_create2::decode_create2(uint_8 *buffer, int_8 rcv_len, pcreate2_msg msg)
{
    int i = 0;
    int j = 0;
    uint_8 buf[64];
    uint_8 checksum;

    //printf("Decode starting**************************\n");

    for(i = 0; i < rcv_len; )
    {
        //printf("%2X ", (uint_8)buffer[i]);

        if(i < 10)
            buf[j++] = buffer[i++];
        else
        {
            if((uint_8)buffer[i] == (uint_8)0xA5)
            {

                if((uint_8)buffer[i+1] == (uint_8)0xA5 || (uint_8)buffer[i+1] == (uint_8)0xAA || buffer[i+1] == (uint_8)0x55)
                {
                    buf[j++] = buffer[i+1];
                    i = i + 2;
                }
                else{

                    buf[j++] = buffer[i++];

                }
            }
            else
            {
                buf[j++] = buffer[i++];
            }
        }
    }

#ifdef PRINT_XRO_DEBUG_INFO
    printf("\ndel ctrl : \n");
    for(i = 0; i < j; i++)
        printf("%2d ", i);
    printf("\n");

    for(i = 0; i < j; i++)
    {
        printf("%2x ", (uint_8)buf[i]);
    }
    printf("\nlen in = %d, len del = %d\n", rcv_len, j);
#endif

    if(j < 41)
    {
        printf("decode length = %d, Decode error******************************\n", j);
        return -1;
    }
    else
    {
        i = 0;

        //frame frame header
        if(buf[i++] != (uint_8)0xaa || buf[i++] != (uint_8)0xaa)
            return -2;
        //framme device address &&  function code
        if(buf[i++] != (uint_8)0x12 || buf[i++] != (uint_8)0x83)
            return -3;
        //frame data start address
        if(buf[i++] != (uint_8)0x0c || buf[i++] != (uint_8)0x00)
            return -4;
        //frame data length
        if(buf[i++] != (uint_8)0x1c || buf[i++] != (uint_8)0x00 || buf[i++] != (uint_8)0x00 || buf[i++] != (uint_8)0x00)
            return -5;
        //frame tail
        if(buf[40] != (uint_8)0x55 || buf[41] != (uint_8)0x55)
            return -6;

        //checksum
        for(i = 2; i < 38; i++)
            checksum += buf[i];
        /*
                   if(checksum != (uint_8)buf[39])
                   {
                   printf("^^^^^^^^^^^^^^^^^^^^^^^^^checksum error!&&&&&&&&&&&&&&&&&&&&&&&&\n");
                   return -7;
                   }
                 */		//时间标签
        msg->time = getTStamp();//(uint_32)(((uint_8)buf[10]) + ((uint_8)buf[11] << 8) + ((uint_8)buf[12] << 16) + ((uint_8)buf[13] << 24));
        msg->drop_sensor.right = (((uint_8)buf[14] & 0x01)!=0);
        msg->drop_sensor.left = (((uint_8)buf[14] & 0x02) != 0);
        msg->drop_sensor.rmid = (((uint_8)buf[14] & 0x08) != 0);
        msg->drop_sensor.lmid = (((uint_8)buf[14] & 0x04) != 0);

        msg->ir_sensor.right = (((uint_8)buf[15] & 0x01) != 0);
        msg->ir_sensor.rmid = (((uint_8)buf[15] & 0x02) != 0);
        msg->ir_sensor.mid = (((uint_8)buf[15] & 0x04) != 0);
        msg->ir_sensor.lmid = (((uint_8)buf[15] & 0x08)!= 0);
        msg->ir_sensor.left =(((uint_8)buf[15] & 0x10) != 0);


        msg->bumper_sensor.right = (((uint_8)((uint_8)(buf[17]) & 0x01)) != 0);
        msg->bumper_sensor.left = (((uint_8)((uint_8)(buf[17]) & 0x02)) != 0);
        msg->angular_pos = (int_16)(buf[18] + (buf[19] << 8));
        msg->left_pos = (int_32)(((uint_8)buf[20]&0xFFFFFFFF) + (((uint_8)buf[21]&0xFFFFFFFF)<<8) + (((uint_8)buf[22]&0xFFFFFFFF)<<16) + (((uint_8)buf[23]&0xFFFFFFFF)<<24));
        msg->right_pos = (int_32)(((uint_8)buf[24]&0xFFFFFFFF) + (((uint_8)buf[25]&0xFFFFFFFF)<<8) + (((uint_8)buf[26]&0xFFFFFFFF)<<16) + (((uint_8)buf[27]&0xFFFFFFFF)<<24));
        msg->line_vel = (int_32)(buf[28] + (buf[29]<<8) + (buf[30]<<16) + (buf[31]<<24));
        msg->angular_vel = (int_32)(buf[32] + (buf[33]<<8) + (buf[34]<<16) + (buf[35]<<24));
        msg->battery = (uint_8)buf[37];

    }
    msg->update = 1;

    //	printf("Decode finished!\n");

    return 0;
}




int_32 CThread_create2::print_create2bot(pcreate2_msg msg)
{
    if(msg)
    {
        /*
                   std::cout << " time:" << std::dec  << msg->time << std::endl;
                   std::cout << " right drop sensor: " << msg->drop_sensor.right << std::endl;
                   std::cout << " left  drop sensor:" <<  msg->drop_sensor.left << std::endl;
                   std::cout << " mid left drop sensor:" <<  msg->drop_sensor.lmid << std::endl;
                   std::cout << " mid right drop sensor:" <<  msg->drop_sensor.rmid << std::endl;
                   std::cout << " right ir sensor:" << msg->ir_sensor.right << std::endl;
                   std::cout << " right mid ir sensor:" << msg->ir_sensor.rmid << std::endl;
                   std::cout << " middle ir sensor:" << msg->ir_sensor.mid << std::endl;
                   std::cout << " left mid ir sensor:" << msg->ir_sensor.lmid << std::endl;
                   std::cout << " left ir sensor:"<< msg->ir_sensor.left << std::endl;
                   std::cout << " right bumper sensor:" << msg->bumper_sensor.right << std::endl;
                   std::cout << " left bumper sensor:" << msg->bumper_sensor.left << std::endl;
                   std::cout << " angular pos:" << msg->angular_pos << std::endl;
                   std::cout << " left pos:" << msg->left_pos << std::endl;
                   std::cout << " right pos:" << msg->right_pos << std::endl;
                   std::cout << " line velocity:" << msg->line_vel << std::endl;
                   std::cout << " angular velocity:" << msg->angular_vel << std::endl;
                   std::cout << " battery: " << msg->battery << std::endl;
                 */

        printf("*************************************\n\
               time:				%lld\n \
               right drop sensor:		%d\n \
               left  drop sensor:		%d\n \
               mid-left drop sensor:		%d\n \
               mid-right drop sensor:		%d\n \
               right ir sensor:                 %d\n \
               right-mid ir sensor:		%d\n \
               middle ir sensor:		%d\n \
               left-mid ir sensor:		%d\n \
               left ir sensor:			%d\n \
               right bumper sensor:		%d\n \
               left bumper sensor:		%d\n \
               angular pos:			%d\n \
               left pos:			%d\n \
               right pos:			%d\n \
               line velocity:			%d\n \
               angular velocity:		%d\n \
               battery:                         %d\n \
               **************************************", \
               msg->time, \
               msg->drop_sensor.right, \
               msg->drop_sensor.left, \
               msg->drop_sensor.lmid,\
               msg->drop_sensor.rmid,\
               msg->ir_sensor.right,\
               msg->ir_sensor.rmid,\
               msg->ir_sensor.mid,\
               msg->ir_sensor.lmid,\
               msg->ir_sensor.left,\
               msg->bumper_sensor.right,\
               msg->bumper_sensor.left,\
               msg->angular_pos,\
               msg->left_pos,\
               msg->right_pos,\
               msg->line_vel,\
               msg->angular_vel,\
               msg->battery);
    }
    else
    {
        printf(" xrobot msg is NULL\n");
        return -1;
    }

    return 0;
}

int64_t CThread_create2::getTStamp()
{
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1e6+tv.tv_usec;
}

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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <iostream>

#include "CThread_imu.h"
#include "CThread_base.h"

CThread_imu::CThread_imu()
{

	imu_read_len = 0;
	imu_read_times = 0;
	imu_read_err_times = 0;
	imu_read_update = 0;

	bzero(imu_cominbuf, sizeof(imu_cominbuf));
}

CThread_imu::~CThread_imu()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void* CThread_imu::process_imu(void *)
{

	fd_set fs_read;
	int fs_sel;
	int decode_ret;

	CSerial *pimu_serial;

	pimu_serial = new CSerial();
	if(NULL == pimu_serial)
	{
		return (void *)-1;
	}
	pimu_serial->openPort('0');
	pimu_serial->setPort(0, 115200, '8', '0', '0', '0', '0', '1');
	pimu_serial->uTtyDelay = 10*1000000/115200;

	//线程处理
	while(1)
	{
		/* TODO
		   tv_timeout.tv_sec = 0;
		   tv_timeout.tv_usec = 50000;
		 */
		FD_ZERO(&fs_read);
		FD_SET(pimu_serial->m_nFdcom, &fs_read);
		FD_SET(0, &fs_read);
		fs_sel = select(pimu_serial->m_nFdcom + 1, &fs_read, NULL, NULL, NULL);
		if(-1 == fs_sel)
		{
			; //TODO
		}

		if(FD_ISSET(pimu_serial->m_nFdcom, &fs_read))
		{
			pimu_serial->getDataFromSerial();
                        decode_ret = decode_imu(pimu_serial->m_chrRecvBuffer, pimu_serial->m_nRecvLen, 2, pimu_msg_struct);
//#ifdef PRINT_IMU_DEBUG_INFO
//			printf("process_imu decode ret = %d\n", decode_ret);
//			print_imu(pimu_msg_struct);
//#endif

		}
		usleep(200);
	}			

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CThread_imu::create(pthread_t &tid,pthread_attr_t *attr, int sock)
{
	if(pthread_create(&tid,attr,process_imu,&sock))
	{	
		printf("CThread Created Error \n") ;
		exit(1) ;
	}
	else
	{
		printf("imu thread create successfully.\n");
			struct sched_param param;
			int rs = pthread_attr_getschedparam(attr,&param);
			assert(rs==0);
			printf("priority=%d",param.__sched_priority);
			printf("IMU thread priority is %d\n", param.__sched_priority);
		
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CThread_imu::exit(int)
{
	pthread_exit(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//mode = 1 为9轴41byte， mode = 2 为6轴27bye
int_32 CThread_imu::decode_imu(uint_8 *read_buffer, int_8 len, int mode, struct imu_struct *p_imu)
{
	struct timeval t;

#ifdef PRINT_IMU_DECODE_INFO
	printf("imu read LEN = %d\n", len);
	for(int i=0; i < len; i++)
		printf("%x ", (uint_8)read_buffer[i]);
	printf("\n");
#endif
	gettimeofday(&t, NULL);
        p_imu->post_time = t.tv_sec*1e6+t.tv_usec;

	if(mode == 1 && len == 41)
	{
		if(read_buffer[8] ==(uint_8)0xA0)
		{	
			p_imu->acc_x = (short)(read_buffer[9]  + (read_buffer[10]<<8));
			p_imu->acc_y = (short)(read_buffer[11] + (read_buffer[12]<<8));  
			p_imu->acc_z = (short)(read_buffer[13] + (read_buffer[14]<<8)); 
		}
		else
			return -2;
		if(read_buffer[15] ==(uint_8)0xB0)
		{	
			p_imu->ang_vel_x = (short)(read_buffer[16] + (read_buffer[17]<<8));
			p_imu->ang_vel_y = (short)(read_buffer[18] + (read_buffer[19]<<8));  
			p_imu->ang_vel_z = (short)(read_buffer[20] + (read_buffer[21]<<8)); 
		}
		else
			return -3;
		if(read_buffer[22] == (uint_8)0xC0)
		{
			p_imu->mag_x = (short)(read_buffer[23] + (read_buffer[24]<<8));
			p_imu->mag_y = (short)(read_buffer[25] + (read_buffer[26]<<8));  
			p_imu->mag_z = (short)(read_buffer[27] + (read_buffer[28]<<8)); 

		}
		else
			return -4;
		if(read_buffer[29] == (uint_8)0xd0)
		{
			p_imu->pitch = ((float)(short)(read_buffer[30] + (read_buffer[31]<<8)))/100;
			p_imu->roll = ((float)(short)(read_buffer[32] + (read_buffer[33]<<8)))/100;
			p_imu->yaw = ((float)(short)(read_buffer[34] + (read_buffer[35]<<8)))/10;
		}	
		else
			return -5;
	}
	else if(mode == 2 && len == 27)
	{

		if(read_buffer[6] == (uint_8)0xA0)
		{	

			p_imu->acc_x = (short)(read_buffer[7]  + (read_buffer[8]<<8));
			p_imu->acc_y = (short)(read_buffer[9] + (read_buffer[10]<<8));  

			p_imu->acc_z = (short)(read_buffer[11] + (read_buffer[12]<<8)); 
		}

		if(read_buffer[13] == (uint_8)0xB0)
		{	
			p_imu->ang_vel_x = (short)(read_buffer[14] + (read_buffer[15]<<8));
			p_imu->ang_vel_y = (short)(read_buffer[16] + (read_buffer[17]<<8));  
			p_imu->ang_vel_z = (short)(read_buffer[18] + (read_buffer[19]<<8)); 
		}


		if(read_buffer[20] == (uint_8)0xD0)
		{
			p_imu->pitch = ((float)(short)(read_buffer[21] + (read_buffer[22]<<8)))/100;
			p_imu->roll = ((float)(short)(read_buffer[23] + (read_buffer[24]<<8)))/100;
			p_imu->yaw = ((float)(short)(read_buffer[25] + (read_buffer[26]<<8)))/10;
		}	
	}
	else
		return -1;

	pimu_msg_struct->update = 1 ;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
int_32 CThread_imu::print_imu(pimu_struct msg)
{
	if(msg)
	{
		std::cout << "acc_x =" << msg->acc_x << std::endl;
		std::cout << "acc_y =" << msg->acc_y << std::endl;
		std::cout << "acc_z =" << msg->acc_z << std::endl;

		std::cout << "ang_vel_x=" << msg->ang_vel_x<< std::endl;
		std::cout << "ang_vel_y=" << msg->ang_vel_y<< std::endl;
		std::cout << "ang_vel_z=" << msg->ang_vel_z<< std::endl;

		std::cout << "pitch=" << msg->pitch<< std::endl;
		std::cout << "roll=" << msg->roll<< std::endl;
		std::cout << "yaw=" << msg->yaw<< std::endl;

		std::cout << "mag_x=" << msg->mag_x<< std::endl;
		std::cout << "mag_y=" << msg->mag_y<< std::endl;
		std::cout << "mag_z=" << msg->mag_z<< std::endl;

	}
	else
	{
		printf(" xrobot msg is NULL\n");
		return -1;
	}

	return 0;
}

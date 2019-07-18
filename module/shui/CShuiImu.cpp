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
#include <string>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <iostream>

#include "CThread_Shuiimu.h"
#include "CThread_imu.h"

#define	PRINT_SHUI_DEBUG_INFO 	0
#define	PRINT_SHUI_DECODE_INFO 	0

using namespace std;

#define _GLIBCXX_USE_CXX11_ABI 0


extern imu_struct imu_msg_struct;
extern pimu_struct pimu_msg_struct;


CThread_shui::CThread_shui()
{
    //save_dir = "./";
    //save_fname = "shui0.csv";
    cout << save_dir << endl;

    init_shui_msg();
}

CThread_shui::CThread_shui(string dir, string filename)
{
    save_dir = dir + "/./";
    save_fname = filename;
    cout << save_dir << endl;

    init_shui_msg();
}

CThread_shui::~CThread_shui()
{
}

void CThread_shui::init_shui_msg()
{
    pimu_msg_struct = &imu_msg_struct;
    shui_msg.ang_vel = 0.0;
    shui_msg.yaw = 0.0;
}

shui_struct CThread_shui::get_data() const
{
    return shui_msg;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CThread_shui::process_shui(int para)
{

    fd_set fs_read;
    int fs_sel;
    int decode_ret;
    cout << "Shui Imu thread is running" << endl;
    CSerial *pshui_serial;

    pshui_serial = new CSerial();
    if(NULL == pshui_serial)
    {
        return;
    }
    pshui_serial->openPort('0');
    pshui_serial->setPort(0, 115200, '8', '0', '0', '0', '0', '1');
    pshui_serial->uTtyDelay = 10*1000000/115200;

    //ofstream ofile;               //定义输出文件
    //ofile.open(save_dir + save_fname);     //作为输出文件打开
    //ofile.open(save_dir + save_fname);     //作为输出文件打开
    //cout << save_dir + save_fname <<endl;
    //线程处理

    while(1)
    {
        FD_ZERO(&fs_read);
        FD_SET(pshui_serial->m_nFdcom, &fs_read);
        FD_SET(0, &fs_read);
        fs_sel = select(pshui_serial->m_nFdcom + 1, &fs_read, NULL, NULL, NULL);
        if(-1 == fs_sel)
        {
            ; //TODO
        }

        if(FD_ISSET(pshui_serial->m_nFdcom, &fs_read))
        {
            pshui_serial->getDataFromSerial();

            decode_ret = decode_shui(pshui_serial->m_chrRecvBuffer, pshui_serial->m_nRecvLen);

            if(decode_ret == 0)
            {
                //print_shui();
                //save2file();
                pimu_msg_struct->yaw = shui_msg.yaw;
                pimu_msg_struct->post_time = shui_msg.time;

//                std::cout << "Pimu_msg_struct->yaw = " << to_string(pimu_msg_struct->yaw) << std::endl;
            }
            else
            {
#if PRINT_SHUI_DEBUG_INFO
                cout <<" "<<endl;
                printf("process_shui decode ret = %d\n", decode_ret);
#endif
            }
        }
        usleep(1000);
    }
    //ofile.close();

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CThread_shui::exit(int)
{
    pthread_exit(0);
}

int32_t CThread_shui::decode_shui(uint8_t *read_buffer, int8_t len)
{
    struct timeval t;
    struct shui_struct *p_imu= &shui_msg;

#if PRINT_SHUI_DECODE_INFO
    printf("shui read LEN = %d\n", len);
    for(int i=0; i < len; i++)
        printf("%x ", (uint8_t)read_buffer[i]);
    printf("\n");
#endif
    gettimeofday(&t, NULL);
    p_imu->time = t.tv_sec  * 1e9 + t.tv_usec * 1e3;
    char  buf[64] = {0};
    short angular = 0;
    short angular_velocity = 0;
    short check_sum = 0;
    short check_angular = 0;
    short check_angular_velocity = 0;
    if(read_buffer[0]==0xFF&&read_buffer[1]==0xFF)
    {
        memcpy(&angular_velocity, &read_buffer[2], 2);
        memcpy(&angular, &read_buffer[4], 2);
        memcpy(&check_angular, &read_buffer[2], 2);
        memcpy(&check_angular_velocity, &read_buffer[4], 2);
        memcpy(&check_sum, &read_buffer[6], 2);
        if(check_sum ==check_angular_velocity + check_angular -1)
        {
            p_imu->ang_vel = (angular_velocity) / 100.0;
            p_imu->yaw= angular / 100.0;
        }
        else
        {
            cout << "Shui IMU checksum error" << endl;
            return -1;
        }
    }
    else
    {
        cout << "Shui IMU Decode error" << endl;
        return -2;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
int32_t CThread_shui::print_shui()
{
    pshui_struct msg = &shui_msg;
    if(msg)
    {
        cout << "Shui ang vel = " << msg->ang_vel  << "\tShui      yaw = " << msg->yaw<< endl;
    }
    else
    {
        printf(" xrobot msg is NULL\n");
        return -1;
    }

    return 0;
}

int CThread_shui::save2file()
{
    //定义输出文件

    string fences = ",";
    string string2file = to_string(shui_msg.time) + fences
            +to_string(shui_msg.ang_vel) + fences
            +to_string(shui_msg.yaw);
    char buf[1024];
    //sprintf(buf,"%lld, %f, %f, %f, %f, %f, %f, %f, %f, %f",shui_msg.time,shui_msg.ang_vel_x,shui_msg.ang_vel_y,shui_msg.ang_vel_z,shui_msg.acc_x,shui_msg.acc_y,shui_msg.acc_z,shui_msg.pitch,shui_msg.roll,shui_msg.yaw);

    //string string2file(buf);
    //ofile.write(stringtowrite, stringtowrite.length());//strlen(stringtowrite));
    ofile << string2file << endl;
}

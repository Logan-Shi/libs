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
#include <vector>
#include <iostream>

#include "types.h"
#include "Create2.h"
#include "serial.h"
#include "config.h"
#include "CThread_motion.h"

#include "fuse.h"



using namespace std;
//using namespace cv;
#define SLEEPTIME 2000
#define DEBUG
#define SHUIIMU
#define CREATE
//#define IMU
//#define ORB_FUSION
#define LINE_SPEED 150

/*
typedef struct {
   //FUNCALLBACK callback;
    int64_t init_timeStamp;
    int32_t frameNo;
    int64_t cur_timeStamp;

    int32_t update;
    float yaw;
} V2M_MSG;
*/
extern V2M_MSG *pvslam_data;

float init_imu_yaw_ang;
float init_vslam_yaw_ang;
float lightc_ang = 0;
float sum_cmp_ang = 0;

float OFFSET_YAW_ORB_IMU = 0;
float BIAS_YAW_ORB_IMU = 0;
int32_t ORB_INIT_FLAG = 0;
int32_t fd = 0;

float k1 = 0;
float k2 = 0;


vector<pos_ori> cur_pos(1);
int32_t run_times;

CThread_motion::CThread_motion()
{
    create2_read_len = 0;
    create2_write_len = 0;
}

CThread_motion::~CThread_motion()
{
}
////////////////////////
void* CThread_motion::process_motion(void *)
{

    const string inputSettingsFile =  "default.yaml";
        FileStorage fs(inputSettingsFile, FileStorage::READ);
        if (!fs.isOpened())
        {
            cout << "Could not open the configuration file: \"" << inputSettingsFile << "\"" << endl;
        }
        read(fs);
    while (1)
    {
        //motion control here
        printf("Thread motion here\n");

        fd = open_usb_serial(fd);
        open_creat_interface(fd);
        goto_safe_mode(fd);

        motion();
        sleep(1);

        //end of motion control
    }
    return (void *)NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CThread_motion::create(pthread_t &tid, pthread_attr_t *attr,int32_t sock)
{

    //if(pthread_create(&tid,NULL,process_motion,(void *)sock))
    if (pthread_create(&tid, attr, process_motion, &sock))
    {
        printf("CThread Created Error \n");
        exit(1);
    }
    else
        printf("**************CThread Created success*******************************************************\n");

}

//////////////////////////
void CThread_motion::exit(int)
{
    pthread_exit(0);
}


/**********************************************************************************/
int32_t CThread_motion::motion()
{

    cur_pos[0].x = 0;
    cur_pos[0].y = 0;
    cur_pos[0].theta = 0;
    cur_pos[0].timeStamp = getTStamp();
    float init_ang = 0;
//    float start_yaw, end_yaw;
//    float pre_yaw, cur_yaw;
//    int64_t time;
    int32_t motion_times = 0;


    printf("Begin to move!\n");
    create2_msg pre_msg;
    FILE *fid = fopen("DataLog.txt", "w"); //clear contents of DataLog.txt
    fclose(fid);

    while (1)
    {


        if (x_msg_struct.update == 1)
        {
            printf("Debug motion 2!\n");
            memcpy(&pre_msg, px_msg_struct, sizeof(pre_msg));
            printf("Debug motion 3!\n");
            x_msg_struct.update = 0;
            break;
        }
        usleep(SLEEPTIME);
    }

    printf("Begin to move!\n");
    while (1)
    {

//        update_xro_msg(&pre_msg);
//        printf("\033[2J");
//        printf("x_msg_struct.time: %lld\n", x_msg_struct.time/1000);
//        printf("x_msg_struct.bump_sensor.right: %d\n", x_msg_struct.bump_sensor.right);
//        printf("x_msg_struct.bump_sensor.left: %d\n", x_msg_struct.bump_sensor.left);
//        printf("x_msg_struct.light_sensor.right: %d\n", x_msg_struct.light_sensor.right);
//        printf("x_msg_struct.light_sensor.rmid: %d\n", x_msg_struct.light_sensor.rmid);
//        printf("x_msg_struct.light_sensor.mid: %d\n", x_msg_struct.light_sensor.mid);
//        printf("x_msg_struct.light_sensor.lmid: %d\n", x_msg_struct.light_sensor.lmid);
//        printf("x_msg_struct.light_sensor.left: %d\n", x_msg_struct.light_sensor.left);
//        usleep(5*SLEEPTIME);


        run_times++;
        init_imu_yaw_ang = imu_msg_struct.yaw;
        sum_cmp_ang = 0;


        printf("Begin to move!\n");

        usleep(SLEEPTIME);
        line_motion(&pre_msg, LINE_SPEED, init_ang);
        update_msg(&pre_msg);

        usleep(SLEEPTIME);
        spin(&pre_msg, init_ang + 1570,4000);
        update_msg(&pre_msg);

        usleep(2*SLEEPTIME);
        turn_motion(&pre_msg, 75, init_ang + 1570*2, 134);
        update_msg(&pre_msg);

        usleep(SLEEPTIME);
        line_motion(&pre_msg, LINE_SPEED, init_ang + 1570*2);
        update_msg(&pre_msg);

        usleep(SLEEPTIME);
        spin(&pre_msg, init_ang + 1570, 4000);
        update_msg(&pre_msg);

        usleep(2*SLEEPTIME);
        turn_motion(&pre_msg, 75, init_ang, 134);
        update_msg(&pre_msg);

        usleep(SLEEPTIME*100);
        motion_times++;
    }

    return 0;

}
/*******************************************************************/
int32_t CThread_motion::line_motion(pcreate2_msg ppre_msg, int32_t trans_vel_d, float des_ang)
{


    pos_ori prior_pos;
    pos_ori tem_pos = cur_pos.back();
    pos_ori init_pos = tem_pos;
    pos_ori current_pos;

    create2_msg curr_msg;	//解析后，create2的信息
    ctrl_cmd cmd;

    int32_t wheel_base = 268;
    int32_t obs_flag;  // flag for obstalces

    float y, y_d;
    float L_l, L_r;
    float change_angle = 0;
    float slope_line, line_b;
    float theta, theta_d;
    float pre_yaw, curr_yaw;
    float init_x;
    float origin[2];

    ORB_IMU_INIT();

    origin[0] = tem_pos.x;
    origin[1] = tem_pos.y;
    coor_tf(init_pos, tem_pos, origin, des_ang, change_angle);

    slope_line = tan(-0 / 1000);
    theta_d = -0;
    init_x= init_pos.x;
    line_b = init_pos.y - slope_line*init_pos.x;

    printf("Begin to execute line motion!\n");
    update_create2_msg(ppre_msg);
    prior_pos = init_pos;
    pre_yaw = imu_msg_struct.yaw;

    while (1)
    {

#ifdef DEBUG
        printf("----------------------Line motion----------------------\n");
        printf("-------------------------------------------------------\n");
#endif // DEBUG

#ifdef CREATE
        update_create2_msg(curr_msg);
        L_l = (curr_msg.left_pos - ppre_msg->left_pos)*72*3.14/508.8;
        L_r = (curr_msg.right_pos - ppre_msg->right_pos)*72*3.14/508.8;
#else
        update_xro_msg(curr_msg);
        L_l = (curr_msg.left_pos - ppre_msg->left_pos)*0.2167;
        L_r = (curr_msg.right_pos - ppre_msg->right_pos)*0.2167;
#endif

        curr_yaw = imu_msg_struct.yaw;
        update_irc_ang(pre_yaw, curr_yaw);
        pre_yaw = curr_yaw;

        current_pos.theta = -(lightc_ang + imu_msg_struct.yaw - init_imu_yaw_ang)*pi*1000 / 180
                 - change_angle;
        odometer(L_l, L_r, wheel_base, &prior_pos, &current_pos);

        obs_flag = response_obs_v1(curr_msg, ppre_msg, current_pos, change_angle, des_ang, pre_yaw, origin);
        switch (obs_flag) {
        case 1: // obs in front
            return 1;
            break;
        case 2: // obs on the left side
            if (des_ang < 100)
                return 1;
            else {
                printf("Begin to advoid left obstacles!\n");
                coor_tf(current_pos, cur_pos.back(), origin, des_ang, change_angle);
                spin(ppre_msg, des_ang - 1570, 4000);
                usleep(SLEEPTIME);
                turn_motion(ppre_msg, 75, des_ang + 1570, 180);
                usleep(SLEEPTIME);
                coor_tf(current_pos, cur_pos.back(), origin, des_ang, change_angle);
                if ((current_pos.y - slope_line*current_pos.x - line_b) < 10)
                    spin(ppre_msg, des_ang, 4000);
                else
                    forward_motion(ppre_msg, 75, slope_line*current_pos.x + line_b - current_pos.y,des_ang);
                coor_tf(current_pos, cur_pos.back(), origin, des_ang, change_angle);
            }
                break;
        case 3: // obs on the right side
            if (des_ang > 100)
                return 1;
            else {
                advoid_line_obs(ppre_msg, current_pos, origin, des_ang, change_angle);
                printf("Finished advoid obs motion!\n");
            }

                break;
        default: // no obs
            break;
        }


        y = current_pos.y;
        y_d = slope_line*current_pos.x + line_b;
        theta = current_pos.theta;

        cmd.trans_vel = trans_vel_d;
        cmd.ang_vel = motion_controller(y, y_d, theta, theta_d, 0, trans_vel_d, 1);
        set_cmd(create2_cmd_string, cmd.trans_vel, cmd.ang_vel);
        create2_cmd_update = 1;

        update_cur_pos(current_pos, change_angle, origin);
        memcpy(ppre_msg, &curr_msg, sizeof(create2_msg));
        prior_pos = current_pos;

#ifdef DEBUG
//        printf("cur_pos.back().timeStamp: %lld\n", cur_pos.back().timeStamp);
//        printf("imu_msg_struct.post_time %lld\n", imu_msg_struct.post_time);
//        printf("Delta time between IMU and Odo: %lld\n", cur_pos.back().timeStamp - imu_msg_struct.post_time);
        printf("imu_msg_struct.yaw: %f\n", imu_msg_struct.yaw);
        printf("init_imu_yaw_ang: %f\n", init_imu_yaw_ang);
        printf("lightc_ang: %f\n", lightc_ang);
        printf("sum_cmp_ang: %f\n", sum_cmp_ang);
        printf("current theta: %f\n", current_pos.theta);
        printf("cur_pos.back().theta: %f\n", cur_pos.back().theta);
        printf("Change_angle: %f\n", change_angle);
        printf("current x: %f\n", current_pos.x);
        printf("current y: %f\n", current_pos.y);
        printf("init x: %f\n", init_x);
        printf("---------------------------------------------------------------\n");
        printf("---------------------------------------------------------------\n");
#endif // DEBUG
        usleep(SLEEPTIME);
    }
}

/*************************************************************************/
int32_t CThread_motion::forward_motion(pcreate2_msg ppre_msg, int32_t trans_vel_d, int32_t distance, float des_ang)
{
    pos_ori prior_pos;
    pos_ori tem_pos = cur_pos.back();
    pos_ori init_pos = tem_pos;
    pos_ori current_pos;

    create2_msg curr_msg;	//解析后，create2的信息
    ctrl_cmd cmd;

    int32_t wheel_base = 268;
    int32_t sign = 1;
    int32_t obs_flag = 0;

    float y, y_d;
    float L_l, L_r;
    float change_angle = 0;
    float slope_line, line_b;
    float theta, theta_d;
    float pre_yaw, curr_yaw;
    float init_x;
    float origin[2];


    origin[0] = tem_pos.x;
    origin[1] = tem_pos.y;
    coor_tf(init_pos, tem_pos, origin, des_ang, change_angle);

    slope_line = tan(-0 / 1000);
    theta_d = -0;
    init_x = init_pos.x;
    line_b = init_pos.y - slope_line*init_pos.x;

    update_create2_msg(ppre_msg);
    prior_pos = init_pos;
    pre_yaw = imu_msg_struct.yaw;

    while (1)
    {
        update_create2_msg(curr_msg);
#ifdef DEBUG
        printf("----------------------Forward motion----------------------\n");
        printf("-------------------------------------------------------\n");
#endif // DEBUG

        curr_yaw = imu_msg_struct.yaw;
        update_irc_ang(pre_yaw, curr_yaw);
        pre_yaw = curr_yaw;

        current_pos.theta = -(lightc_ang + imu_msg_struct.yaw - init_imu_yaw_ang)*pi*1000 / 180
                 - change_angle;
        L_l = (curr_msg.left_pos - ppre_msg->left_pos)*0.2167;
        L_r = (curr_msg.right_pos - ppre_msg->right_pos)*0.2167;
        odometer(L_l, L_r, wheel_base, &prior_pos, &current_pos);


        if (trans_vel_d > 0) {
            obs_flag = response_obs_v1(curr_msg, ppre_msg, current_pos, change_angle, des_ang, pre_yaw, origin);
            switch (obs_flag) {
            case 1: // obs in front
                return 1;
                break;
            case 2: // obs on the left side
                if (des_ang > 100)
                    return 1;
                else {
                    coor_tf(current_pos, cur_pos.back(), origin, des_ang, change_angle);
                }
                    break;
            case 3: // obs on the right side
                if (des_ang < 100)
                    return 1;
                else {
                    coor_tf(current_pos, cur_pos.back(), origin, des_ang, change_angle);
                }
                    break;
            default: // no obs
                break;
            }
        }

        if(abs(abs(current_pos.x - init_x) - distance) < 5)
        {
            stop_cmd(&current_pos, ppre_msg, change_angle, pre_yaw, origin);
            return 1;
        }


        y = current_pos.y;
        y_d = slope_line*current_pos.x + line_b;
        theta = current_pos.theta;
        if (trans_vel_d > 0)
            sign = 1;
        else
            sign = -1;
        cmd.trans_vel = trans_vel_d;
        cmd.ang_vel = motion_controller(y, y_d, theta, theta_d, 0, trans_vel_d, sign);
        set_cmd(create2_cmd_string, cmd.trans_vel, cmd.ang_vel);
        create2_cmd_update = 1;

        update_cur_pos(current_pos, change_angle, origin);
        memcpy(ppre_msg, &curr_msg, sizeof(create2_msg));
        prior_pos = current_pos;

#ifdef DEBUG
//        printf("cur_pos.back().timeStamp: %lld\n", cur_pos.back().timeStamp);
//        printf("imu_msg_struct.post_time %lld\n", imu_msg_struct.post_time);
//        printf("Delta time between IMU and Odo: %lld\n", cur_pos.back().timeStamp - imu_msg_struct.post_time);
        printf("imu_msg_struct.yaw: %f\n", imu_msg_struct.yaw);
        printf("init_imu_yaw_ang: %f\n", init_imu_yaw_ang);
        printf("lightc_ang: %f\n", lightc_ang);
        printf("sum_cmp_ang: %f\n", sum_cmp_ang);
        printf("current theta: %f\n", current_pos.theta);
        printf("cur_pos.back().theta: %f\n", cur_pos.back().theta);
        printf("Change_angle: %f\n", change_angle);
        printf("trans_vel: %d\n", cmd.trans_vel);
        printf("ang_vel: %d\n", cmd.ang_vel);
        printf("current x: %f\n", current_pos.x);
        printf("current y: %f\n", current_pos.y);
        printf("init x: %f\n", init_x);
        printf("---------------------------------------------------------------\n");
        printf("---------------------------------------------------------------\n");
#endif // DEBUG
        usleep(SLEEPTIME);
    }

}
/*******************************************************************/

int32_t CThread_motion::backward_motion(pcreate2_msg ppre_msg, int32_t trans_vel_d, int32_t distance, float des_ang)
{
    pos_ori prior_pos;
    pos_ori tem_pos = cur_pos.back();
    pos_ori init_pos = tem_pos;
    pos_ori current_pos;

    create2_msg curr_msg;	//解析后，create2的信息
    ctrl_cmd cmd;

    int32_t wheel_base = 268;
    int32_t sign = 1;
    int32_t is_side_obs = 0;

    float y, y_d;
    float L_l, L_r;
    float change_angle = 0;
    float slope_line, line_b;
    float theta, theta_d;
    float pre_yaw, curr_yaw;
    float init_x;
    float origin[2];

    origin[0] = tem_pos.x;
    origin[1] = tem_pos.y;
    coor_tf(init_pos, tem_pos, origin, des_ang, change_angle);

    slope_line = tan(-0 / 1000);
    theta_d = -0;
    init_x = init_pos.x;
    line_b = init_pos.y - slope_line*init_pos.x;

    update_create2_msg(ppre_msg);
    prior_pos = init_pos;
    pre_yaw = imu_msg_struct.yaw;

    while (1)
    {
        update_create2_msg(curr_msg);
#ifdef DEBUG
        printf("----------------------Turn motion----------------------\n");
        printf("-------------------------------------------------------\n");
#endif // DEBUG

        curr_yaw = imu_msg_struct.yaw;
        update_irc_ang(pre_yaw, curr_yaw);
        pre_yaw = curr_yaw;

        current_pos.theta = -(lightc_ang + imu_msg_struct.yaw - init_imu_yaw_ang)*pi*1000 / 180
                 - change_angle;
        L_l = (curr_msg.left_pos - ppre_msg->left_pos)*0.2167;
        L_r = (curr_msg.right_pos - ppre_msg->right_pos)*0.2167;
        odometer(L_l, L_r, wheel_base, &prior_pos, &current_pos);

        if (is_side_obs == 0)
            is_side_obs = check_side_obs(curr_msg);
        if(abs(abs(current_pos.x - init_x) - distance) < 5)
        {
            stop_cmd(&current_pos, ppre_msg, change_angle, pre_yaw, origin);
            return is_side_obs;
        }


        y = current_pos.y;
        y_d = slope_line*current_pos.x + line_b;
        theta = current_pos.theta;
        if (trans_vel_d > 0)
            sign = 1;
        else
            sign = -1;
        cmd.trans_vel = trans_vel_d;
        cmd.ang_vel = motion_controller(y, y_d, theta, theta_d, 0, trans_vel_d, sign);
        set_cmd(create2_cmd_string, cmd.trans_vel, cmd.ang_vel);
        create2_cmd_update = 1;

        update_cur_pos(current_pos, change_angle, origin);
        memcpy(ppre_msg, &curr_msg, sizeof(create2_msg));
        prior_pos = current_pos;

#ifdef DEBUG
//        printf("cur_pos.back().timeStamp: %lld\n", cur_pos.back().timeStamp);
//        printf("imu_msg_struct.post_time %lld\n", imu_msg_struct.post_time);
//        printf("Delta time between IMU and Odo: %lld\n", cur_pos.back().timeStamp - imu_msg_struct.post_time);
        printf("imu_msg_struct.yaw: %f\n", imu_msg_struct.yaw);
        printf("init_imu_yaw_ang: %f\n", init_imu_yaw_ang);
        printf("lightc_ang: %f\n", lightc_ang);
        printf("sum_cmp_ang: %f\n", sum_cmp_ang);
        printf("current theta: %f\n", current_pos.theta);
        printf("cur_pos.back().theta: %f\n", cur_pos.back().theta);
        printf("Change_angle: %f\n", change_angle);
        printf("trans_vel: %d\n", cmd.trans_vel);
        printf("ang_vel: %d\n", cmd.ang_vel);
        printf("current x: %f\n", current_pos.x);
        printf("current y: %f\n", current_pos.y);
        printf("init x: %f\n", init_x);
        printf("---------------------------------------------------------------\n");
        printf("---------------------------------------------------------------\n");
#endif // DEBUG
        usleep(SLEEPTIME);
    }

}
/*******************************************************************************/
int32_t CThread_motion::spin(pcreate2_msg ppre_msg, float des_ang, int64_t span_time)
{
    pos_ori prior_pos = cur_pos.back();
    pos_ori current_pos;

    create2_msg  curr_msg;	//解析后，create2的信息
    ctrl_cmd cmd;

    int32_t wheel_base = 268;
    int32_t max_ang_vel = 2 * (des_ang - cur_pos.back().theta) * 1000 / span_time;
    int32_t des_ang_vel;
    int64_t init_time, curr_time;

    float L_l, L_r;
    float pre_yaw, curr_yaw;
    float origin[2] = {0};

    update_create2_msg(ppre_msg, init_time);
    pre_yaw = imu_msg_struct.yaw;

    while (1)
    {
        update_create2_msg(curr_msg);
#ifdef DEBUG
        printf("----------------------Spin motion----------------------\n");
        printf("-------------------------------------------------------\n");
#endif // DEBUG
        ORB_IMU_INIT();

        L_l = (curr_msg.left_pos - ppre_msg->left_pos)*0.2167;
        L_r = (curr_msg.right_pos - ppre_msg->right_pos)*0.2167;

        curr_yaw = imu_msg_struct.yaw;
        update_irc_ang(pre_yaw, curr_yaw);
        pre_yaw = curr_yaw;

        current_pos.theta = -(lightc_ang + imu_msg_struct.yaw - init_imu_yaw_ang)*pi*1000 / 180;
        odometer(L_l, L_r, wheel_base, &prior_pos, &current_pos);
        if (abs(des_ang - current_pos.theta) <= 10 /*&& abs(x_msg_struct.angular_vel) < 50*/)
        {
            stop_cmd(&current_pos, ppre_msg, 0, pre_yaw, origin);
            return 1;
        }

        curr_time = (curr_msg.time - init_time)/1000;
        if (curr_time <= (span_time / 2))
        {
            des_ang_vel = (2 * max_ang_vel* curr_time / span_time);
            cmd.ang_vel = (int32_t)(des_ang_vel);// +5 * (des_ang_vel*curr_time / 2000 + init_ang - pcurrent_pos->theta));
        }
        else if (curr_time < span_time && curr_time >(span_time / 2))
        {
            des_ang_vel = (2 * (int32_t)(span_time - (int32_t)curr_time)*max_ang_vel / span_time);
            cmd.ang_vel = (int32_t)(des_ang_vel);// +1 * (init_ang + turn_ang - des_ang_vel*
            //(span_time - curr_time) / 2000 - pcurrent_pos->theta));
        }
        else
        {
            des_ang_vel = 0;
            cmd.ang_vel = 0.5*(des_ang - current_pos.theta);
        }
        cmd.trans_vel = 0;
        set_cmd(create2_cmd_string, cmd.trans_vel, cmd.ang_vel);
        create2_cmd_update = 1;

        update_cur_pos(current_pos, 0, origin);
        memcpy(ppre_msg, &curr_msg, sizeof(create2_msg));
        prior_pos = current_pos;

        //fprintf(fid, "%10d  %10d\n", curr_time, cmd.ang_vel);
#ifdef DEBUG
//        printf("cur_pos.back().timeStamp: %lld\n", cur_pos.back().timeStamp);
//        printf("imu_msg_struct.post_time %lld\n", imu_msg_struct.post_time);
//        printf("Delta time between IMU and Odo: %lld\n", cur_pos.back().timeStamp - imu_msg_struct.post_time);

        printf("imu_msg_struct.yaw: %f\n", imu_msg_struct.yaw);
        printf("init_imu_yaw_ang: %f\n", init_imu_yaw_ang);
        printf("lightc_ang: %f\n", lightc_ang);
        printf("sum_cmp_ang: %f\n", sum_cmp_ang);
        printf("current_pos.theta: %f\n", cur_pos.back().theta);
        printf("cur_pos.back().theta: %f\n", cur_pos.back().theta);
        printf("current x: %f\n", current_pos.x);
        printf("max_ang_vel: %d\n", max_ang_vel);
        printf("cmd.ang_vel: %d\n", cmd.ang_vel);
        printf("---------------------------------------------------------------\n");
        printf("---------------------------------------------------------------\n");
#endif // DEBUG
        usleep(SLEEPTIME);

    }
}

/**********************************************************************************/

int32_t CThread_motion::turn_motion(pcreate2_msg ppre_msg, int32_t trans_vel_d, float des_ang, float Radius)
{

    pos_ori prior_pos;
    pos_ori tem_pos = cur_pos.back();
    pos_ori init_pos = tem_pos;
    pos_ori current_pos;
    create2_msg curr_msg;	//解析后，create2的信息
    ctrl_cmd cmd;

    int32_t wheel_base = 268;
    int32_t ang_vel_d, trans_vel;
    int32_t obs_flag = 0;
    int64_t init_time, curr_time;

    float y, y_d;
    float L_l, L_r;
    float change_angle = 0;
    float theta, theta_d;
    float pre_yaw, curr_yaw;
    float origin[2];
    origin[0] = tem_pos.x;
    origin[1] = tem_pos.y;
    coor_tf(init_pos, tem_pos, origin, des_ang, change_angle);

    FILE *fid = fopen("DataLog.txt", "a+");
    update_create2_msg(ppre_msg, init_time);
    prior_pos = init_pos;
    pre_yaw = imu_msg_struct.yaw;


    while (1)
    {
        update_create2_msg(curr_msg);
        obs_flag = response_obs_v1(curr_msg, ppre_msg, current_pos, change_angle, cur_pos.end()->theta, pre_yaw, origin);
        if (obs_flag != 0)
            return 1; // There is an obstalce


#ifdef DEBUG
        printf("----------------------Turn motion----------------------\n");
        printf("-------------------------------------------------------\n");
#endif // DEBUG
        ORB_IMU_INIT();
        //        ORB_IMU_SYNC();

        curr_yaw = imu_msg_struct.yaw;
        update_irc_ang(pre_yaw, curr_yaw);
        pre_yaw = curr_yaw;

        L_l = (curr_msg.left_pos - ppre_msg->left_pos)*0.2167;
        L_r = (curr_msg.right_pos - ppre_msg->right_pos)*0.2167;
        current_pos.theta = -(lightc_ang + imu_msg_struct.yaw - init_imu_yaw_ang)*pi*1000 / 180 - change_angle;
        odometer(L_l, L_r, wheel_base, &prior_pos, &current_pos);
        if (abs(des_ang - current_pos.theta - change_angle) <= 20 /*&& abs(x_msg_struct.angular_vel) < 50*/)
        {
//            stop_cmd(&current_pos, ppre_msg, 0, pre_yaw);
            update_cur_pos(current_pos, change_angle, origin);
            printf("Finished turn motion!\n");
            fprintf(fid, "-------------------------------------------------------\n");
            fprintf(fid, "-------------------------------------------------------\n");
            fclose(fid);
            return 0; // There is no obstalce
        }

        trans_vel = trans_vel_d;
        y = current_pos.y;
        theta = current_pos.theta;
        if (des_ang <= change_angle) {
            theta_d = -asin(current_pos.x/(Radius))*1000;
            y_d = -(Radius)*(1 - cos(theta_d/1000));
            ang_vel_d = -trans_vel*1000/(Radius);
        }
        else {
            theta_d = asin(current_pos.x/(Radius))*1000;
            y_d = (Radius)*(1 - cos(theta_d/1000));
            ang_vel_d = trans_vel*1000/(Radius);
        }

        curr_time = (curr_msg.time - init_time)/1000;
        if (abs(des_ang - tem_pos.theta) > 1570) {
            cmd.trans_vel = trans_vel_d;
            cmd.ang_vel = ang_vel_d;
        } else {
            cmd.trans_vel = trans_vel;
            if (curr_time <= 1000)
                cmd.ang_vel = ang_vel_d;
            else
                cmd.ang_vel = motion_controller(y, y_d, theta, theta_d, ang_vel_d, cmd.trans_vel, 1);
            if (cmd.ang_vel > 800)
                cmd.ang_vel = 800;
            else if (cmd.ang_vel < -800)
                cmd.ang_vel = -800;
        }

        set_cmd(create2_cmd_string, cmd.trans_vel, cmd.ang_vel);
        create2_cmd_update = 1;

        update_cur_pos(current_pos, change_angle, origin);
        memcpy(ppre_msg, &curr_msg, sizeof(create2_msg));
        prior_pos = current_pos;

        fprintf(fid, "%10ld  %10f  %10f  %10f\n", curr_time, current_pos.x, current_pos.y, current_pos.theta);
#ifdef DEBUG
//        printf("cur_pos.back().timeStamp: %lld\n", cur_pos.back().timeStamp);
//        printf("imu_msg_struct.post_time %lld\n", imu_msg_struct.post_time);
//        printf("Delta time between IMU and Odo: %lld\n", cur_pos.back().timeStamp - imu_msg_struct.post_time);
        printf("x_msg_struct.time: %ld\n", x_msg_struct.time/1000);
        printf("imu_msg_struct.yaw: %f\n", imu_msg_struct.yaw);
        printf("init_imu_yaw_ang: %f\n", init_imu_yaw_ang);
        printf("lightc_ang: %f\n", lightc_ang);
        printf("sum_cmp_ang: %f\n", sum_cmp_ang);
        printf("current_pos.theta: %f\n",current_pos.theta);
        printf("cur_pos.back().theta: %f\n", cur_pos.back().theta);
        printf("current x: %f\n", current_pos.x);
        printf("cmd.ang_vel: %d\n", cmd.ang_vel);
        printf("y_d: %f\n", y_d);
        printf("theta_d: %f\n", theta_d);
        printf("Change_angle: %f\n", change_angle);
        printf("current x: %f\n", current_pos.x);
        printf("current y: %f\n", current_pos.y);
        printf("L_l: %f\n", L_l);
        printf("L_r: %f\n", L_r);
        printf("theta_d: %f\n",theta_d);
        printf("y_d: %f\n",y_d);
        printf("---------------------------------------------------------------\n");
        printf("---------------------------------------------------------------\n");
#endif // DEBUG
        usleep(SLEEPTIME);
    }
}

/****************************************************************************/

int32_t CThread_motion::turn_motion(pcreate2_msg ppre_msg, int32_t trans_vel_d, float des_ang, float Radius, float thread_y)
{

    pos_ori prior_pos;
    pos_ori tem_pos = cur_pos.back();
    pos_ori init_pos = tem_pos;
    pos_ori current_pos;
    create2_msg curr_msg;	//解析后，create2的信息
    ctrl_cmd cmd;

    int32_t wheel_base = 268;
    int32_t ang_vel_d, trans_vel;
    int32_t obs_flag = 0;
    int64_t init_time, curr_time;

    float y, y_d;
    float L_l, L_r;
    float change_angle = 0;
    float theta, theta_d;
    float pre_yaw, curr_yaw;
    float origin[2];
    origin[0] = tem_pos.x;
    origin[1] = tem_pos.y;
    coor_tf(init_pos, tem_pos, origin, des_ang, change_angle);

    FILE *fid = fopen("DataLog.txt", "a+");
    update_create2_msg(ppre_msg, init_time);
    prior_pos = init_pos;
    pre_yaw = imu_msg_struct.yaw;


    while (1)
    {
        update_create2_msg(curr_msg);
        obs_flag = response_obs_v1(curr_msg, ppre_msg, current_pos, change_angle, cur_pos.end()->theta, pre_yaw, origin);
        if (obs_flag != 0)
            return 1; // There is an obstalce

        if (abs(cur_pos.back().y - thread_y) < 10)
            return 0;


#ifdef DEBUG
        printf("----------------------Turn motion----------------------\n");
        printf("-------------------------------------------------------\n");
#endif // DEBUG
        ORB_IMU_INIT();
        //        ORB_IMU_SYNC();

        curr_yaw = imu_msg_struct.yaw;
        update_irc_ang(pre_yaw, curr_yaw);
        pre_yaw = curr_yaw;

        L_l = (curr_msg.left_pos - ppre_msg->left_pos)*0.2167;
        L_r = (curr_msg.right_pos - ppre_msg->right_pos)*0.2167;
        current_pos.theta = -(lightc_ang + imu_msg_struct.yaw - init_imu_yaw_ang)*pi*1000 / 180 - change_angle;
        odometer(L_l, L_r, wheel_base, &prior_pos, &current_pos);
        if (abs(des_ang - current_pos.theta - change_angle) <= 20 /*&& abs(x_msg_struct.angular_vel) < 50*/)
        {
//            stop_cmd(&current_pos, ppre_msg, 0, pre_yaw);
            update_cur_pos(current_pos, change_angle, origin);
            printf("Finished turn motion!\n");
            fprintf(fid, "-------------------------------------------------------\n");
            fprintf(fid, "-------------------------------------------------------\n");
            fclose(fid);
            return 0; // There is no obstalce
        }

        trans_vel = trans_vel_d;
        y = current_pos.y;
        theta = current_pos.theta;
        if (des_ang <= change_angle) {
            theta_d = -asin(current_pos.x/(Radius))*1000;
            y_d = -(Radius)*(1 - cos(theta_d/1000));
            ang_vel_d = -trans_vel*1000/(Radius);
        }
        else {
            theta_d = asin(current_pos.x/(Radius))*1000;
            y_d = (Radius)*(1 - cos(theta_d/1000));
            ang_vel_d = trans_vel*1000/(Radius);
        }

        curr_time = (curr_msg.time - init_time)/1000;
        if (abs(des_ang - tem_pos.theta) > 1570) {
            cmd.trans_vel = trans_vel_d;
            cmd.ang_vel = ang_vel_d;
        } else {
            cmd.trans_vel = trans_vel;
            if (curr_time <= 1000)
                cmd.ang_vel = ang_vel_d;
            else
                cmd.ang_vel = motion_controller(y, y_d, theta, theta_d, ang_vel_d, cmd.trans_vel, 1);
            if (cmd.ang_vel > 800)
                cmd.ang_vel = 800;
            else if (cmd.ang_vel < -800)
                cmd.ang_vel = -800;
        }

        set_cmd(create2_cmd_string, cmd.trans_vel, cmd.ang_vel);
        create2_cmd_update = 1;

        update_cur_pos(current_pos, change_angle, origin);
        memcpy(ppre_msg, &curr_msg, sizeof(create2_msg));
        prior_pos = current_pos;

        fprintf(fid, "%10ld  %10f  %10f  %10f\n", curr_time, current_pos.x, current_pos.y, current_pos.theta);
#ifdef DEBUG
//        printf("cur_pos.back().timeStamp: %lld\n", cur_pos.back().timeStamp);
//        printf("imu_msg_struct.post_time %lld\n", imu_msg_struct.post_time);
//        printf("Delta time between IMU and Odo: %lld\n", cur_pos.back().timeStamp - imu_msg_struct.post_time);
        printf("x_msg_struct.time: %ld\n", x_msg_struct.time/1000);
        printf("imu_msg_struct.yaw: %f\n", imu_msg_struct.yaw);
        printf("init_imu_yaw_ang: %f\n", init_imu_yaw_ang);
        printf("lightc_ang: %f\n", lightc_ang);
        printf("sum_cmp_ang: %f\n", sum_cmp_ang);
        printf("current_pos.theta: %f\n",current_pos.theta);
        printf("cur_pos.back().theta: %f\n", cur_pos.back().theta);
        printf("current x: %f\n", current_pos.x);
        printf("cmd.ang_vel: %d\n", cmd.ang_vel);
        printf("y_d: %f\n", y_d);
        printf("theta_d: %f\n", theta_d);
        printf("Change_angle: %f\n", change_angle);
        printf("current x: %f\n", current_pos.x);
        printf("current y: %f\n", current_pos.y);
        printf("L_l: %f\n", L_l);
        printf("L_r: %f\n", L_r);
        printf("theta_d: %f\n",theta_d);
        printf("y_d: %f\n",y_d);
        printf("---------------------------------------------------------------\n");
        printf("---------------------------------------------------------------\n");
#endif // DEBUG
        usleep(SLEEPTIME);
    }
}

/***********************************************************************/

inline int32_t CThread_motion::check_side_obs(create2_msg curr_msg)
{
    if ((curr_msg.light_sensor.right == 0 &&
         curr_msg.light_sensor.rmid == 0) &&
         (curr_msg.light_sensor.lmid == 1 ||
         curr_msg.light_sensor.left == 1))
        return 1; // obs on the left side
    else if ((curr_msg.light_sensor.right == 1 ||
              curr_msg.light_sensor.rmid == 1) &&
             (curr_msg.light_sensor.lmid == 0 &&
              curr_msg.light_sensor.left == 0))
        return 2; // obs on the right side
    else
        return 0; // obs in front
}

//int32_t CThread_motion::follow_obs(pxrobot_msg ppre_msg, int32_t trans_vel_d, float des_ang, float des_y)
//{

//    pos_ori prior_pos;
//    pos_ori tem_pos = cur_pos.back();
//    pos_ori init_pos = tem_pos;
//    pos_ori current_pos;
//    xrobot_msg curr_msg;	//解析后，xrobot的信息
//    ctrl_cmd cmd;

//    int32_t wheel_base = 268;
//    int32_t ang_vel_d, trans_vel;
//    int64_t init_time, curr_time;

//    float y, y_d;
//    float L_l, L_r;
//    float change_angle = 0;
//    float theta, theta_d;
//    float pre_yaw, curr_yaw;
//    float origin[2];
//    origin[0] = tem_pos.x;
//    origin[1] = tem_pos.y;
//    coor_tf(init_pos, tem_pos, origin, tem_pos.theta, change_angle);

//    FILE *fid = fopen("DataLog.txt", "a+");
//    update_xro_msg(ppre_msg, init_time);
//    prior_pos = init_pos;
//    pre_yaw = imu_msg_struct.yaw;


//    while (1)
//    {
//        update_xro_msg(curr_msg);
//#ifdef DEBUG
//        printf("----------------------Turn motion----------------------\n");
//        printf("-------------------------------------------------------\n");
//#endif // DEBUG
//        ORB_IMU_INIT();
//        //        ORB_IMU_SYNC();

//        curr_yaw = imu_msg_struct.yaw;
//        update_irc_ang(pre_yaw, curr_yaw);
//        pre_yaw = curr_yaw;

//        L_l = (curr_msg.left_pos - ppre_msg->left_pos)*0.2167;
//        L_r = (curr_msg.right_pos - ppre_msg->right_pos)*0.2167;
//        current_pos.theta = -(lightc_ang + imu_msg_struct.yaw - init_imu_yaw_ang)*pi*1000 / 180 - change_angle;
//        odometer(L_l, L_r, wheel_base, &prior_pos, &current_pos);
//        if (abs(des_ang - current_pos.theta - change_angle) <= 10 /*&& abs(x_msg_struct.angular_vel) < 50*/)
//        {
////            stop_cmd(&current_pos, ppre_msg, 0, pre_yaw);
//            update_cur_pos(current_pos, change_angle, origin);
//            fprintf(fid, "-------------------------------------------------------\n");
//            fprintf(fid, "-------------------------------------------------------\n");
//            fclose(fid);
//            return 1;
//        }

//        curr_time = (curr_msg.time - init_time)/1000;

//        trans_vel = trans_vel_d;
//        y = current_pos.y;
//        theta = current_pos.theta;
//        if (des_ang <= change_angle) {
//            theta_d = -asin(current_pos.x/(Radius))*1000;
//            y_d = -(Radius)*(1 - cos(theta_d/1000));
//            ang_vel_d = -trans_vel*1000/(Radius);
//        }
//        else {
//            theta_d = asin(current_pos.x/(Radius))*1000;
//            y_d = (Radius)*(1 - cos(theta_d/1000));
//            ang_vel_d = trans_vel*1000/(Radius);
//        }
//        cmd.trans_vel = trans_vel;
//        if (curr_time <= 1000)
//            cmd.ang_vel = ang_vel_d;
//         else
//        cmd.ang_vel = motion_controller(y, y_d, theta, theta_d, ang_vel_d, cmd.trans_vel, 1);
//        if (cmd.ang_vel > 800)
//                cmd.ang_vel = 800;
//        else if (cmd.ang_vel < -800)
//            cmd.ang_vel = -800;
//        set_cmd(xro_cmd_string, cmd.trans_vel, cmd.ang_vel);
//        xro_cmd_update = 1;

//        update_cur_pos(current_pos, change_angle, origin);
//        memcpy(ppre_msg, &curr_msg, sizeof(create2bot_msg));
//        prior_pos = current_pos;

//        fprintf(fid, "%10lld  %10f  %10f  %10f\n", curr_time, current_pos.x, current_pos.y, current_pos.theta);
//#ifdef DEBUG
////        printf("cur_pos.back().timeStamp: %lld\n", cur_pos.back().timeStamp);
////        printf("imu_msg_struct.post_time %lld\n", imu_msg_struct.post_time);
////        printf("Delta time between IMU and Odo: %lld\n", cur_pos.back().timeStamp - imu_msg_struct.post_time);
//        printf("x_msg_struct.time: %lld\n", x_msg_struct.time/1000);
//        printf("imu_msg_struct.yaw: %f\n", imu_msg_struct.yaw);
//        printf("init_imu_yaw_ang: %f\n", init_imu_yaw_ang);
//        printf("lightc_ang: %f\n", lightc_ang);
//        printf("sum_cmp_ang: %f\n", sum_cmp_ang);
//        printf("current_pos.theta: %f\n",current_pos.theta);
//        printf("cur_pos.back().theta: %f\n", cur_pos.back().theta);
//        printf("current x: %f\n", current_pos.x);
//        printf("cmd.ang_vel: %d\n", cmd.ang_vel);
//        printf("y_d: %f\n", y_d);
//        printf("theta_d: %f\n", theta_d);
//        printf("Change_angle: %f\n", change_angle);
//        printf("current x: %f\n", current_pos.x);
//        printf("current y: %f\n", current_pos.y);
//        printf("L_l: %f\n", L_l);
//        printf("L_r: %f\n", L_r);
//        printf("theta_d: %f\n",theta_d);
//        printf("y_d: %f\n",y_d);
//        printf("---------------------------------------------------------------\n");
//        printf("---------------------------------------------------------------\n");
//#endif // DEBUG
//        usleep(SLEEPTIME);
//    }
//}

/*********************************************************************************/

void CThread_motion::stop_cmd(ppos_ori pcurrent_pos, pcreate2_msg ppre_msg, float change_angle, float pre_yaw, float origin[2])
{
    uint_8 send_cmd[64];
    int32_t stop_count = 0;
    int32_t wheel_base = 268;
    float curr_yaw;
    float  L_l, L_r;

    set_cmd(send_cmd, 0, 0);
    create2_cmd_update = 1;
    pos_ori prior_pos = *pcurrent_pos;
    create2_msg curr_msg;

    memcpy(ppre_msg, px_msg_struct, sizeof(create2_msg));

    while (1)
    {
        stop_count++;
        while (1)
        {
            if (x_msg_struct.update == 1)
            {
                x_msg_struct.update = 0;
                break;
            }
            else
            {
                usleep(SLEEPTIME);
                continue;
            }
        }

#ifdef CREATE
        update_create2_msg(curr_msg);
        L_l = (curr_msg.left_pos - ppre_msg->left_pos)*72*3.14/508.8;
        L_r = (curr_msg.right_pos - ppre_msg->right_pos)*72*3.14/508.8;
#else
        update_xro_msg(curr_msg);
        L_l = (curr_msg.left_pos - ppre_msg->left_pos)*0.2167;
        L_r = (curr_msg.right_pos - ppre_msg->right_pos)*0.2167;
#endif
        printf("stop_cmd-- L_l: %f\n", L_l);
        printf("stop_cmd-- L_r: %f\n", L_r);

        curr_yaw = imu_msg_struct.yaw;
        update_irc_ang(pre_yaw, curr_yaw);
        pre_yaw = curr_yaw;

        pcurrent_pos->theta = -(lightc_ang + imu_msg_struct.yaw - init_imu_yaw_ang)*pi*1000 / 180
                - change_angle;
        odometer(L_l, L_r, wheel_base, &prior_pos, pcurrent_pos);
        update_cur_pos(*pcurrent_pos, change_angle, origin);

        printf("stop_cmd-- current_pos.x: %f\n", pcurrent_pos->x);
        printf("stop_cmd-- current_pos.theta: %f\n", pcurrent_pos->theta);
        memcpy(ppre_msg, px_msg_struct, sizeof(x_msg_struct));
        prior_pos = *pcurrent_pos;
        set_cmd(create2_cmd_string, 0, 0);
        create2_cmd_update = 1;
        printf("---------------------------------------------------------------\n");
        printf("---------------------------------------------------------------\n");

        if (abs(L_l) < 0.01 && abs(L_r) < 0.01)
            return;
    }
}

/*************************************************************************/

void CThread_motion::update_msg(pcreate2_msg ppre_msg)
{
    int32_t wheel_base = 268;
    float L_l, L_r;
    float pre_yaw, curr_yaw;

    ppos_ori pinit_pos = &cur_pos.back();
    cur_pos.resize(cur_pos.size() + 1);
    pre_yaw = imu_msg_struct.yaw;

    printf("Update msg.\n");
    while (1)
    {
        if (x_msg_struct.update == 1)
        {
            L_l = (x_msg_struct.left_pos - ppre_msg->left_pos)*0.2167;
            L_r = (x_msg_struct.right_pos - ppre_msg->right_pos)*0.2167;
            cur_pos.back().timeStamp = x_msg_struct.time;
            x_msg_struct.update = 0;
            break;
        }
        else
        {
            usleep(SLEEPTIME);
            continue;
        }
    }

    curr_yaw = imu_msg_struct.yaw;
    update_irc_ang(pre_yaw, curr_yaw);
    pre_yaw = curr_yaw;

    cur_pos.back().theta = -(lightc_ang + imu_msg_struct.yaw - init_imu_yaw_ang)*pi*1000 / 180;
    L_l = (x_msg_struct.left_pos - ppre_msg->left_pos)*0.2167;
    L_r = (x_msg_struct.right_pos - ppre_msg->right_pos)*0.2167;
    odometer(L_l, L_r, wheel_base, pinit_pos, &cur_pos.back());
    pinit_pos = &cur_pos.back();
    memcpy(ppre_msg, px_msg_struct, sizeof(x_msg_struct));


    printf("current x: %f\n", cur_pos.back().x);
    printf("current theta: %f\n", cur_pos.back().theta);

    return;
}
/*************************************************************************/
int32_t CThread_motion::odometer(float L_l, float L_r, int32_t wheel_base, ppos_ori pprior_pos, ppos_ori current_pos)
{

    float sum_dis = L_l + L_r;
//    float differ_dis = L_r - L_l;
//    float deta_theta;

//    printf("Debug sum_dis = %f\n", sum_dis);
//    printf("Debug differ_dis = %f\n", differ_dis);

    current_pos->x = pprior_pos->x + sum_dis*(cos(current_pos->theta / 1000) + cos(pprior_pos->theta / 1000)) / 4;
    current_pos->y = pprior_pos->y + sum_dis*(sin(current_pos->theta / 1000) + sin(pprior_pos->theta / 1000)) / 4;

    return 1;
}

int32_t CThread_motion::motion_controller(float y, float y_d, float theta, float theta_d,
                                         int32_t angluar_vel_d, int32_t trans_vel_d, int32_t clight_sign)
{

    float x1 = y_d / 1000 - y / 1000;
    float x2 = (tan(theta_d / 1000) - tan(theta / 1000));
    printf("x1 = %f\n", x1);
    printf("x2 = %f\n", x2);
    printf("k1 = %f\n", k1);
    printf("k2 = %f\n", k2);
    float a_vel = pow(cos(theta / 1000), 3)*trans_vel_d*((k1*x1 + k2*x2)
                   + angluar_vel_d/(pow(cos(theta_d/1000),3)*trans_vel_d));
    printf("a_ve = %f\n", a_vel);
    int32_t angluar_vel = a_vel;
    printf("angluar_vel = %d\n", angluar_vel);
    printf("trans_vel_d = %d\n", trans_vel_d);
    return angluar_vel;
}

int64_t CThread_motion::getTStamp()
{
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1e6+tv.tv_usec;
}

void CThread_motion::ORB_IMU_INIT()
{
    if (ORB_INIT_FLAG == 0 && pvslam_data->update == 1)
    {
        int32_t size_cur_pos = cur_pos.size();

        if (size_cur_pos == 1)
        {
            OFFSET_YAW_ORB_IMU = imu_msg_struct.yaw - lightc_ang;
            ORB_INIT_FLAG = 1;
            printf("ORB IMU fusion init sucess!\n");
            printf("init_imu_yaw_ang = %f\n", init_imu_yaw_ang);
            printf("OFFSET_YAW_ORB_IMU = %f\n", OFFSET_YAW_ORB_IMU);
            return;
        }

        for(int32_t i = size_cur_pos; i >= 1 ; i--)
        {
            if (cur_pos[i].timeStamp == pvslam_data->init_timeStamp)
            {
                OFFSET_YAW_ORB_IMU = -cur_pos[i].theta*180/pi/1000 +
                        init_imu_yaw_ang  - lightc_ang;
                ORB_INIT_FLAG = 1;
                printf("ORB IMU fusion init sucess1!\n");
                printf("init_imu_yaw_ang = %f\n", init_imu_yaw_ang);
                printf("OFFSET_YAW_ORB_IMU = %f\n", OFFSET_YAW_ORB_IMU);
                return;
            }

            if (cur_pos[i].timeStamp > pvslam_data->init_timeStamp &&
                    cur_pos[i-1].timeStamp < pvslam_data->init_timeStamp)
            {

                float k = (pvslam_data->init_timeStamp - cur_pos[i-1].timeStamp)/
                        (cur_pos[i].timeStamp - cur_pos[i-1].timeStamp);
                ORB_INIT_FLAG = 1;
                OFFSET_YAW_ORB_IMU = -(cur_pos[i - 1].theta +
                        k*(cur_pos[i].theta - cur_pos[i - 1].theta))*180/pi/1000 + init_imu_yaw_ang;
                printf("ORB IMU fusion init sucess2!\n");
                printf("init_imu_yaw_ang = %f\n", init_imu_yaw_ang);
                printf("OFFSET_YAW_ORB_IMU = %f\n", OFFSET_YAW_ORB_IMU);
                return;
            }
        }
    }
}

void CThread_motion::ORB_IMU_SYNC(float &yaw)
{
    int64_t time = getTStamp();
    while(1)
    {
        ORB_IMU_INIT();
        if (pvslam_data->update == 1 && pvslam_data->cur_timeStamp >= time)
        {
            yaw = pvslam_data->yaw;
            printf("ORB IMU SYNC SUCCESSED!\n");
            break;
        }
        else
        {
            usleep(10*SLEEPTIME);
            printf("WAIT FOR ORB DATA!\n");
            continue;
        }
    }
}

//void CThread_motion::ORB_IMU_SYNC()
//{
//    if (ORB_INIT_FLAG == 0 || pvslam_data->update == 0)
//        return;

//    int32_t size_cur_pos = cur_pos.size();
//    float vlm_yaw;

//    if (pvslam_data->yaw < 0)
//        vlm_yaw = 360 - pvslam_data->yaw + OFFSET_YAW_ORB_IMU;
//    else
//        vlm_yaw = -pvslam_data->yaw + OFFSET_YAW_ORB_IMU;

//    for(int32_t i = size_cur_pos; i >= 1 ; i--)
//    {
//        if (cur_pos[i].timeStamp == pvslam_data->cur_timeStamp)
//        {
//            BIAS_YAW_ORB_IMU = -cur_pos[i].theta*180/pi/1000 + init_imu_yaw_ang - lightc_ang
//                     - vlm_yaw;
//            pvslam_data->update = 0;

//            if (abs(BIAS_YAW_ORB_IMU) > 180)
//                BIAS_YAW_ORB_IMU = -(BIAS_YAW_ORB_IMU + 360);

//            printf("ORB IMU sync init sucess1!\n");
//            printf("cur_theta: %f\n", cur_pos[i].theta);
//            printf("vslam_yaw: %f\n", vlm_yaw);
//            printf("BIAS_YAW_ORB_IMU = %f\n", BIAS_YAW_ORB_IMU);
//            return;
//        }

//        if (cur_pos[i].timeStamp > pvslam_data->cur_timeStamp &&
//                cur_pos[i-1].timeStamp < pvslam_data->cur_timeStamp)
//        {

//            float k = (pvslam_data->cur_timeStamp - cur_pos[i-1].timeStamp)/
//                    (cur_pos[i].timeStamp - cur_pos[i-1].timeStamp);

//            float cur_theta = cur_pos[i - 1].theta +
//                    k*(cur_pos[i].theta - cur_pos[i - 1].theta);
//            BIAS_YAW_ORB_IMU = -cur_theta*180/pi/1000 + init_imu_yaw_ang - lightc_ang
//                     - vlm_yaw;
//            if (abs(BIAS_YAW_ORB_IMU) > 180)
//                BIAS_YAW_ORB_IMU = -(BIAS_YAW_ORB_IMU + 360);

//            pvslam_data->update = 0;
//            printf("ORB IMU sync init sucess2!\n");
//            printf("cur_theta: %f\n", cur_theta);
//            printf("vslam_yaw: %f\n", vlm_yaw);
//            printf("BIAS_YAW_ORB_IMU = %f\n", BIAS_YAW_ORB_IMU);
//            return;
//        }
//    }
//}

void CThread_motion::read(const FileStorage& fs)
{
    fs["k1" ] >> k1;
    fs["k2"] >> k2;
}

//########################################//
//        coordinate transformation       //
//########################################//
inline void CThread_motion::coor_tf(pos_ori &init_pos, pos_ori &tem_pos,
                     float origin[2], float des_ang, float &change_angle)
{
    init_pos.x = cos(des_ang/1000)*(tem_pos.x - origin[0]) +
            sin(des_ang/1000)*(tem_pos.y - origin[1]);
    init_pos.y = -sin(des_ang/1000)*(tem_pos.x - origin[0]) +
            cos(des_ang/1000)*(tem_pos.y - origin[1]);
    init_pos.theta = tem_pos.theta - des_ang;
    init_pos.timeStamp = tem_pos.timeStamp;
    change_angle = des_ang;
}

inline void CThread_motion::update_irc_ang(float pre_yaw, float curr_yaw)
{
#ifdef SHUIIMU
    if ((pre_yaw <= 180) && (pre_yaw >= 175) && (curr_yaw >= -180) && curr_yaw <= -175)
    {
        lightc_ang = lightc_ang + 360;
    }
    else if ((curr_yaw <= 180) && (curr_yaw >= 175) && (pre_yaw >= -180) && pre_yaw <= -175)
    {
        lightc_ang = lightc_ang - 360;
    }
#endif

#ifdef IMU
    if ((pre_yaw <= 360) && (pre_yaw >= 350) && (curr_yaw >= 0) && curr_yaw <= 5)
    {
        lightc_ang = lightc_ang + 360;
    }
    else if ((curr_yaw <= 360) && (curr_yaw >= 350) && (pre_yaw >= 0) && pre_yaw <= 5)
    {
        lightc_ang = lightc_ang - 360;
    }
#endif
}


inline void CThread_motion::update_create2_msg(pcreate2_msg ppre_msg, int64_t &init_time)
{
    while (1)
    {
        if (x_msg_struct.update == 1)
        {
            memcpy(ppre_msg, px_msg_struct, sizeof(create2_msg));
            init_time = x_msg_struct.time;
            x_msg_struct.update = 0;
            return;
        }
        else
        {
            usleep(SLEEPTIME);
            continue;
        }
    }
}

inline void CThread_motion::update_create2_msg(pcreate2_msg ppre_msg)
{
    while (1)
    {
        if (x_msg_struct.update == 1)
        {
            memcpy(ppre_msg, px_msg_struct, sizeof(create2_msg));
            x_msg_struct.update = 0;
            return;
        }
        else
        {
            usleep(SLEEPTIME);
            continue;
        }
    }
}

inline void CThread_motion::update_create2_msg(create2_msg &curr_msg)
{
    while (1) {
        if (x_msg_struct.update == 1)
        {
            memcpy(&curr_msg, px_msg_struct, sizeof(create2_msg));
            cur_pos.resize(cur_pos.size() + 1);
            cur_pos.back().timeStamp = x_msg_struct.time;
            px_msg_struct->update = 0;
            return;
        }
        else
        {
            usleep(SLEEPTIME);
            continue;
        }
    }
}

int32_t CThread_motion::response_obs(create2_msg &curr_msg, pcreate2_msg ppre_msg, pos_ori &current_pos,
                              float change_angle, float des_ang, float pre_yaw, float origin[2]) // response to obstacles
{
//    int32_t is_side_obs = 0;

    if (curr_msg.bump_sensor.right != 0 ||
            curr_msg.bump_sensor.left != 0 ||
            curr_msg.light_sensor.right == 0 ||
            curr_msg.light_sensor.rmid != 0 ||
            curr_msg.light_sensor.mid != 0 ||
            curr_msg.light_sensor.lmid != 0 ||
            curr_msg.light_sensor.left != 0)
    {
        stop_cmd(&current_pos, ppre_msg, change_angle, pre_yaw, origin);
        forward_motion(ppre_msg, -100, 100, des_ang);
        update_msg(ppre_msg);
        return 1;
    }
    return 0;
}

int32_t CThread_motion::response_obs_v1(create2_msg &curr_msg, pcreate2_msg ppre_msg, pos_ori &current_pos,
                                     float change_angle, float des_ang, float pre_yaw, float origin[2]) // response to obstacles
{
    int32_t is_side_obs = 0;

    if (curr_msg.bump_sensor.right == 1 &&
            curr_msg.bump_sensor.left == 1 )
    {
        stop_cmd(&current_pos, ppre_msg, change_angle, pre_yaw, origin);
        is_side_obs = backward_motion(ppre_msg, -100, 30, des_ang);
        update_msg(ppre_msg);
        return is_side_obs + 1;
    }

    if ((curr_msg.bump_sensor.right == 1 || curr_msg.light_sensor.rmid == 1 ||curr_msg.light_sensor.right == 1) &&
         (curr_msg.bump_sensor.left + curr_msg.light_sensor.lmid + curr_msg.light_sensor.left == 0))
        is_side_obs = 2;
    else if ((curr_msg.bump_sensor.left == 1 || curr_msg.light_sensor.lmid == 1 ||curr_msg.light_sensor.left == 1) &&
                 (curr_msg.bump_sensor.right + curr_msg.light_sensor.rmid + curr_msg.light_sensor.right == 0))
        is_side_obs = 1;

    if (is_side_obs != 0) {
        stop_cmd(&current_pos, ppre_msg, change_angle, pre_yaw, origin);
        forward_motion(ppre_msg, -100, 30, des_ang);
        update_msg(ppre_msg);
        return is_side_obs + 1;
    }

    return 0;
}

inline void CThread_motion::update_cur_pos(pos_ori current_pos, float change_angle, float origin[2])
{
    cur_pos.back().x = cos(change_angle/1000)*current_pos.x -
            sin(change_angle/1000)*current_pos.y + origin[0];
    cur_pos.back().y = sin(change_angle/1000)*current_pos.x +
            cos(change_angle/1000)*current_pos.y + origin[1];
    cur_pos.back().theta = current_pos.theta + change_angle;
}

int32_t CThread_motion::advoid_line_obs(pcreate2_msg ppre_msg, pos_ori &current_pos, float origin[2],
float des_ang, float change_angle)
{
    int32_t is_turn_obs = 0;
    while (1) {

        spin(ppre_msg, des_ang + 500, 1000);
        usleep(SLEEPTIME);
        is_turn_obs = turn_motion(ppre_msg, 75, des_ang - 1570, 180, origin[1]);
        printf("is_turn_obs: %d\n", is_turn_obs);

        usleep(SLEEPTIME);
        if (is_turn_obs)
            continue;
        coor_tf(current_pos, cur_pos.back(), origin, des_ang, change_angle);
        printf("current_pos.x: %f\n", current_pos.x);
        printf("current_pos.y: %f\n", current_pos.y);
        if ((current_pos.y - 0) < 10)
            spin(ppre_msg, des_ang, 4000);
        else {
            forward_motion(ppre_msg, 75, current_pos.y - 0, des_ang - 1570);
            spin(ppre_msg, des_ang, 4000);
        }
        coor_tf(current_pos, cur_pos.back(), origin, des_ang, change_angle);
        return 1;
    }

}





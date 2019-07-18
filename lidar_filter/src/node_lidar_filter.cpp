#include "lidar_filter.h"

//Constructor and destructor
NodeBraitenberg2::NodeBraitenberg2(ros::Publisher pub, double angleC, double speed)
{
  angleCoef = angleC;
  robotSpeed = speed;
  pubMessage = pub;
}

NodeBraitenberg2::~NodeBraitenberg2()
{
}

//Publisher
void NodeBraitenberg2::publishMessage()
{
  //preparing message
  geometry_msgs::Twist msg;

  if (distMinLeft >= distMinRight)
  {
    msg.angular.z = -(angleCoef*distMinLeft/distMinRight-angleCoef);
  }
  else
  {
    msg.angular.z = angleCoef*distMinRight/distMinLeft-angleCoef;
  }

  msg.linear.x = robotSpeed;
  //if (distMinLeft < 0.25 && distMinRight < 0.25 && angleMinLeft < 0.7 && angleMinRight < 0.7)
  //{
  //  msg.angular.z*=50;
  //  msg.linear.x*=0.5;
  //}
	
  if (distMinFrount< 0.40 )
  {
    msg.angular.z*=0;
    msg.linear.x*=0;
  }
  //publishing message
  pubMessage.publish(msg);
}

//Subscriber
void NodeBraitenberg2::messageCallback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
  //Calculation of array size from angle range and angle increment.
  int size = msg->ranges.size();
  int minIndexLeft = 0;
  int minIndexFrount = 0;
  int minIndexRight = size/2;
  //This cycle goes through array and finds minimum on the left and right
  for(int i=size/4; i<size/2; i++)
  {
    if (msg->ranges[i] < msg->ranges[minIndexRight] && msg->ranges[i] > 0.05){
      minIndexRight = i;
    } 
  }
  for (int i = size/2; i <size/4*3 ; i++)
  {
    if (msg->ranges[i] < msg->ranges[minIndexLeft] && msg->ranges[i] > 0.05){
      minIndexLeft = i;
    } 
  }
  for (int i = size/360*150; i <size/360*210 ; i++)
  {
    if (msg->ranges[i] < msg->ranges[minIndexFrount] && msg->ranges[i] > 0.05){
      minIndexFrount = i;
    } 
  }
  int counter=0;
  for(int i=0;i<360;i++)
  {
  if(i>60&&i<300)
  continue;
  if(msg->ranges[size*i/360]<0.4&&msg->ranges[size*i/360]>0.05)
  counter++;
  }
  //if(counter>20)
  //stop;

  //Calculation of angle from indexes and storing data to class variables.
  angleMinLeft = (minIndexLeft-size/2)*msg->angle_increment;
  distMinLeft = msg->ranges[minIndexLeft];
  angleMinRight = (minIndexRight-size/2)*msg->angle_increment;
  distMinRight = msg->ranges[minIndexRight];
  angleMinFrount = (minIndexFrount-size/2)*msg->angle_increment;
  distMinFrount= msg->ranges[minIndexFrount];
  //Invoking method for publishing message
  publishMessage();
}

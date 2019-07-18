#include "ros/ros.h"
#include "std_msgs/Int64.h"
#include "beginner_tutorials/Num.h"
#include <sstream>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<beginner_tutorials::Num>("chatter", 1000);
  ros::Rate loop_rate(10);

  while (ros::ok())
  {
    beginner_tutorials::Num msg;

    std::stringstream ss;
    ss <<"4";
    msg.data = ss.str();

    ROS_INFO("%s", msg.data.c_str());

    chatter_pub.publish(msg);

    ros::spinOnce();

    loop_rate.sleep();
  }


  return 0;
}

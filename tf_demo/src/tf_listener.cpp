#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>

int main(int argc, char** argv) {
    ros::init(argc, argv, "my_tf_listener");

    ros::NodeHandle node;

    ros::Publisher turtle_vel =
       node.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 10);
    tf::TransformListener listener;
    ros::Rate rate(10.0);
    while (node.ok()) {
        tf::StampedTransform transform;
        try {
            listener.waitForTransform("odom", "world", ros::Time(0), ros::Duration(0.1));
            listener.lookupTransform("odom", "world", ros::Time(0), transform);
        } catch (tf::TransformException ex) {
            ROS_ERROR("%s",ex.what());
        }
        geometry_msgs::Twist vel_msg;
        //speed setup
	vel_msg.linear.x = 1;
	vel_msg.angular.z = 1;
	turtle_vel.publish(vel_msg);
	rate.sleep();
    }
    return 0;
};

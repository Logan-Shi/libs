# include "lidar_filter.h"

# define SUBSCRIBER_BUFFER_SIZE 1  // Size of buffer for subscriber.
# define PUBLISHER_BUFFER_SIZE 1000  // Size of buffer for publisher.
double ANGLE_COEF=2; // Coeficient for angles.
double ROBOT_SPEED=0.20; // Speed of robot [m/s].
//define PUBLISHER_TOPIC "/syros/base_cmd_vel"
# define PUBLISHER_TOPIC "/cmd_vel"
// #define SUBSCRIBER_TOPIC "/syros/laser_laser"
# define SUBSCRIBER_TOPIC "/scan"

/* task: "Braitenberg vehicle 2"
 * 
 * In main function is created Subscribing node, which transmits messages 
 * to NodeBraitenberg2 object. There are the messages proce ssed and commands 
 * generated.
 */

int main(int argc, char **argv)
{


  //Initialization of node
  ros::init(argc, argv, "braitenberg2");
  ros::NodeHandle n;

  //Creating publisher
  ros::Publisher pubMessage = n.advertise<geometry_msgs::Twist>(PUBLISHER_TOPIC, PUBLISHER_BUFFER_SIZE);

  ros::param::get("ANGLE_COEF",ANGLE_COEF);
  ros::param::get("ROBOT_SPEED",ROBOT_SPEED);

  //Creating object, which stores data from sensors and has methods for
  //publishing and subscribing
  NodeBraitenberg2 *nodeBraitenberg2 = new NodeBraitenberg2(pubMessage, ANGLE_COEF, ROBOT_SPEED);

  //Creating subscriber
  ros::Subscriber sub = n.subscribe(SUBSCRIBER_TOPIC, SUBSCRIBER_BUFFER_SIZE, &NodeBraitenberg2::messageCallback, nodeBraitenberg2);
  ros::spin();

  return 0;
}

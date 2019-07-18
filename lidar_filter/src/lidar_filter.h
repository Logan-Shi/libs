#ifndef SR_NODE_BRAITENBERG2
#define SR_NODE_BRAITENBERG2

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Twist.h"

/* Demonstration task: "Breitenberg Vehicle"
 *
 * This class controls robot. It's behavior is inspired by Breitenberg's 
 * vehicle. In this case robot find minimal value on the left and right 
 * side and goes, where the value is higher.
 */

class NodeBraitenberg2
{
public:

  /* Constructor:
   * 
   * pub    Publisher, which can send commands to robot.
   * angleC Value, which will be stored in angleCoef.
   * speed     Value, which will be stored in robotSpeed.
   */

  NodeBraitenberg2(ros::Publisher pub, double angleC, double speed);

  ~NodeBraitenberg2();

  /* This method reads data from sensor and processes them to variables.
   * 
   * This method finds minimal distances on the left and right side 
   * and saves them to variables distMinLeft, distMinRight.
   * 
   * @param msg Message, which came from robot and contains data from
   * laser scan.
   */

  void messageCallback(const sensor_msgs::LaserScan::ConstPtr& msg);

private:

  /* This method publishes commands for robot.
   *
   * Commands are generated from data, which are stored in variables
   * (distMinLeft, distMinRight). Robot turns to direction, which has higher 
   * value. Robot turns sharper, if higher value >> lower value.
   */

  void publishMessage();

  // Variables

  double angleCoef;       // Coeficient for transfering angles to speed.
  double robotSpeed;        // Speed of robot [m/s].
  double angleMinLeft;       // Angle, at which was measured the shortest distance on the left.
  double distMinLeft;        // Minimum distance masured by sensor on the left.
  double angleMinRight;      // Angle, at which was measured the shortest distance on the right.
  double distMinRight; 
 double angleMinFrount;      // Angle, at which was measured the shortest distance on the right.
  double distMinFrount;       // Minimum distance masured by sensor on the right.
  ros::Publisher pubMessage; // Object for publishing messages.
};

#endif

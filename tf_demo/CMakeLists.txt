cmake_minimum_required(VERSION 2.8.3)
project(tf_demo)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  tf
  turtlesim
  nav_msgs
  message_generation
 )

generate_messages(
  DEPENDENCIES
  nav_msgs
 )

catkin_package(
  CATKIN_DEPENDS message_runtime
 )

include_directories(
  include_directories(include ${catkin_INCLUDE_DIRS})
)

add_executable(tf_broadcaster src/tf_broadcaster.cpp)
target_link_libraries(tf_broadcaster ${catkin_LIBRARIES})
add_dependencies(tf_broadcaster tf_demo_tf_broadcaster_cpp)

add_executable(tf_listener src/tf_listener.cpp)
target_link_libraries(tf_listener ${catkin_LIBRARIES})
add_dependencies(tf_listener tf_demo_tf_listener_cpp)

add_executable(odometry src/odometry.cpp)
target_link_libraries(odometry ${catkin_LIBRARIES})
add_dependencies(odometry tf_demo_odometry_cpp)

#include <cstdio>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "dynamixel_sdk_custom_interfaces/msg/set_position.hpp"


using namespace std;

using SetPosition = dynamixel_sdk_custom_interfaces::msg::SetPosition;

// Motor id and servo values

#define A_id = 1
#define a_low = 0
#define a_middle = 250
#define a_high = 500

#define B_id = 10
#define b_low = 0
#define b_middle = 250
#define b_high = 500


struct motor_pose
{
    int a_id;
    int a_pos_val;
    int b_id;
    int b_pos_val;
};



class ControllerNode : public rclcpp::Node
{
private:
    
    // ROS2 sub
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr pose_est_sub_;

    // ROS2 pub
    rclcpp::Publisher<SetPosition>::SharedPtr set_position_pub_;

    // Callback functions
    void Set_motor_position(const geometry_msgs::msg::PoseStamped::SharedPtr msg);


    // Publish Motor positions
    void publish_motor_position(motor_pose m_pos);



public:
    ControllerNode(/* args */);
    ~ControllerNode();
};


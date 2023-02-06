#include "controller_node.h"

// should be the interface between Bram and Dynamixel


ControllerNode::ControllerNode(/* args */)
{

    // initialize subscriber
    pose_est_sub_ = this->create_subscription<geometry_msgs::msg::PoseStamped>(
        "/controller/pos_est", 10,
        std::bind(&ControllerNode::Set_motor_position, this, std::placeholders::_1));

    // initialize publisher
    set_position_pub_ = this->create_publisher<SetPosition>(
        "set_position", 10);


}

void ControllerNode::Set_motor_position(const geometry_msgs::msg::PoseStamped::SharedPtr msg){


    motor_pose m_position;
    m_position.a_id = A_id; // motor id 
    m_position.b_id = B_id; // motor id

    if (msg->data[0] == 1)
    {
        m_position.a_pos_val = a_low;
        m_position.b_pos_val = b_low;
    };
    if (msg->data[0] == 2)
    {
        m_position.a_pos_val = a_middle;
        m_position.b_pos_val = b_middle;
    };
    if (msg->data[0] == 3)
    {
        m_position.a_pos_val = a_high;
        m_position.b_pos_val = b_high;
    };
    

    publish_motor_position(m_position);

}

void ControllerNode::publish_motor_position(motor_pose m_pos){

    SetPosition set_position_msg;

    // publish for motor a
    set_position_msg.id = m_pos.a_id;
    set_position_msg.position = m_pos.a_pos_val;
    set_position_pub_->publish(set_position_msg);

    // publish for motor b
    set_position_msg.id = m_pos.b_id;
    set_position_msg.position = m_pos.b_pos_val;
    set_position_pub_->publish(set_position_msg);

}




ControllerNode::~ControllerNode()
{
}

#include <cstdio>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"

#include <queue>
#include <chrono>
#include <thread>

#include "bram_uio.h"


#define BRAM0 1
#define BRAM1 2
// remove maybe?
#define Controller_UIO "/dev/uio0"

#define BRAM_SIZE 8000
#define Time = 40

using namespace std;

// coming from Bram
struct pose_est
{
  float x;
  float y;
  float z;
};


class BramNode : public rclcpp::Node
{
public:
  BramNode(const std::string &node_name = "bram_node", const std::string &node_namespace = "bram");
  ~BramNode();

private:
  // ROS2 subscribers
  rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr cam_image_sub_;

  // ROS2 publishers
  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pos_est_pub_;

  // Callback functions
  void cam_image_callback(const std_msgs::msg::Float32MultiArray::SharedPtr msg);
  void pose_est_callback();

  // Publish estimated position
  void publish_pos_est(pose_est pos_est);

  queue<int> pixel_queue;


  // Add here BRAM 
  BRAM Bram_to_FPGA(BRAM0, BRAM_SIZE);
  BRAM Bram_from_FPGA(BRAM1, BRAM_SIZE);

};

int main(int argc, char **argv);
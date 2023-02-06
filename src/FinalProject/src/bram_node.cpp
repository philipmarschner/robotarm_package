#include "bram_node.h"

BramNode::BramNode(const std::string &node_name, const std::string &node_namespace) : rclcpp::Node(node_name, node_namespace)
{

  // Custom code here to initialize BRAM and xkalmanfilterkernel
  // ...
  /*
  x_status = XKalmanfilterkernel_Initialize(&kf_kernel, KALMAN_UIO);

  XKalmanfilterkernel_Set_q(&kf_kernel, q);
  XKalmanfilterkernel_Set_r(&kf_kernel, r);
*/


    // change the paths currently stated for both sub and pub !!!


  // Initialize subscribers
  cam_image_sub_ = this->create_subscription<std_msgs::msg::Float32MultiArray>(
      "/camera/image",
      10,
      std::bind(&BramNode::cam_image_callback, this, std::placeholders::_1));


  // Initialize publishers
  pos_est_pub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("/controller/pos_est", 10);

  while (!XKalmanfilterkernel_IsIdle(&kf_kernel))
    ;
}

BramNode::~BramNode()
{

  //XKalmanfilterkernel_Release(&kf_kernel);
  ~Bram_to_FPGA();
  ~Bram_from_FPGA();
}

void BramNode::cam_image_callback(const std_msgs::msg::Float32MultiArray::SharedPtr msg)
{
  
    int image_size = 100;

    for (size_t i = 0; i < image_size; i++)
    {
        pixel_queue.push(msg->data[i]);
    }
  
       
    // write to bram
    int index = 0;
    while (!pixel_queue.empty())
    {
        Bram_to_FPGA[index] = *(uint32_t *)(&(pixel_queue.front()));
        pixel_queue.pop();
        index += 1;

    }
    
    // should be changed to see if the DNN is ready or something like that.
    /*
    while (!XKalmanfilterkernel_IsReady(&kf_kernel))
      ;

    XKalmanfilterkernel_Start(&kf_kernel);

    while (!XKalmanfilterkernel_IsIdle(&kf_kernel))
      ;
    */
}


void BramNode::pose_est_callback()
{
 
    // read from bram
    pose_est position;
    position.x =Bram_from_FPGA[0];
    //position.y =Bram_from_FPGA[1];
    //position.z =Bram_from_FPGA[2];

    publish_pos_est(position);

    std::this_thread::sleep_for(std::chrono::milliseconds(Time)) // 40
    // use sleep if we don't have a data is ready signal
    //XKalmanfilterkernel_Start(&kf_kernel);
    
  
}


void BramNode::publish_pos_est(pose_est pos_est)
{
  geometry_msgs::msg::PoseStamped pos_est_msg;
  pos_est_msg.header.stamp = this->get_clock()->now();
  pos_est_msg.header.frame_id = "Position";
  pos_est_msg.pose.position.x = pos_est.x;
  //pos_est_msg.pose.position.y = pos_est.y;
  //pos_est_msg.pose.position.z = pos_est.z
  pos_est_pub_->publish(pos_est_msg);
}

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<BramNode>();

  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}
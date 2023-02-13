#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
//#include "dynamixel_sdk/dynamixel_sdk.h"
#include "dynamixel_sdk_custom_interfaces/msg/set_position.hpp"
//#include "dynamixel_sdk_custom_interfaces/srv/get_position.hpp"
#include <chrono>
#include <thread>
#include <stdio.h>
#include <math.h>

#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
using SetPosition = dynamixel_sdk_custom_interfaces::msg::SetPosition;

// Motor id 
#define A_id 1
#define B_id 10

// pnt 1
#define x1 
#define y1
// pnt 2 
#define x2 
#define y2
// pnt 3 
#define x3 
#define y3 

// link lenghts ˝given in cm˝
#define L1 15
#define L2 15

#define PI 3.14159265 

struct motor_pose
{
    int a_id;
    int a_pos_val;
    int b_id;
    int b_pos_val;
};


struct pose
{
	x;
	y;
	q1;
	q2;
};



class ImageSubscriber : public rclcpp::Node
{
	
	public:
		ImageSubscriber() : Node("image_subscriber") {
			RCLCPP_INFO(this->get_logger(), "Initializing ImageSubscriber node");

			RCLCPP_INFO(this->get_logger(), "Starting camera subscription");

			camera_subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
					"/image_raw",
					10,
					std::bind(&ImageSubscriber::onImageMsg, this, std::placeholders::_1)
			);

			pos_est_pub_ = this->create_publisher<SetPosition>("/set_position", 10);

		}
		void calc_forward_kin(float q1, float q2){
	
			int x = L1*std::cos(q1) + L2*std::cos(q1+q2);
			int y = L1*std::sin(q1) + L2*std::sin(q1+q2);
			
			std::cout << "x: " << x << " y: " << y << "\n";

		};


		void calc_inverse_kin(pose pt){
			
			
			// calc q2
			float t1 = std::pow(pt.x , 2) + std::pow(pt.y , 2) - std::pow(L1 ,2) - std::pow(L2 ,2);
			float b1 = 2*L1*L2;
			pt.q2 = std::acos((t1)/(b1));
			
			// calc q1
			float t2 = L2*std::sin(pt.q2);
			float b2 = L1+L2*std::cos(pt.q2);
			pt.q1 = std::atan(pt.y/pt.x) - std::atan(t2/b2);

		};

		int calc_encoder_position(float ang){
			int pos;
			float ang_res = 0.3;
			return pos = (int)*ang/ang_res;
		};
		
		void Set_motor_position(int label){


		    pose pos;
			motor_pose m_position;
			m_position.a_id = A_id; // motor id 
			m_position.b_id = B_id; // motor id

			if (msg->data[0] == 1)
			{
				pos.x = x1;
				pos.y = y1;
			
			};
			if (msg->data[0] == 2)
			{
				pos.x = x2;
				pos.y = y2;
			
			};
			if (msg->data[0] == 3)
			{
				pos.x = x3;
				pos.y = y3;
			
			};
			
			calc_inverse_kin(pos);

			m_position.a_pos_val = calc_encoder_position(pos.q1);
			m_position.b_pos_val = calc_encoder_position(pos.q2);

			publish_motor_position(m_position);

		}
		

		void publish_motor_position(motor_pose m_pos){

			SetPosition set_position_msg;

			// publish for motor a
			set_position_msg.id = m_pos.a_id;
			set_position_msg.position = m_pos.a_pos_val;
			pos_est_pub_->publish(set_position_msg);

			// publish for motor b
			set_position_msg.id = m_pos.b_id;
			set_position_msg.position = m_pos.b_pos_val;
			pos_est_pub_->publish(set_position_msg);
			

		}


	private:
		rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr camera_subscription_;
		
		// ROS2 publishers
  		rclcpp::Publisher<SetPosition>::SharedPtr pos_est_pub_;





		void onImageMsg(const sensor_msgs::msg::Image::SharedPtr msg) {
			RCLCPP_INFO(this->get_logger(), "Received image3!");
			//sensor_msgs::msg::Image temp = msg;
			
			cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, msg->encoding);
			cv::Mat img = cv_ptr->image;
			

			RCLCPP_INFO(this->get_logger(), "Successfully loaded image");
			
			// convert to grayscale
			cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

			// convert to 10x10 image
			cv::resize(img, img, cv::Size(10,10), cv::INTER_AREA);

			// divide by 255 (normalize)
			img.convertTo(img, CV_32F, 1.0/255.0);

			// convert to 1D array
			std::vector<float>vec(img.begin<float>(), img.end<float>());

			// Stream image (vec) to FPGA

			//write data to bram
			int temp = sleep_time;
			//wait for response
			//std::this_thread::sleep_for(std::chrono::milliseconds(temp));
			
			/*
            cv::imshow("imgCallBack",img);
            cv::waitKey(1);
            RCLCPP_INFO(this->get_logger(), "imgcallback hopefully");
            */
          
			//read data from bram
			int label = 1;


			
			//write to dynamixel topic

			Set_motor_position(label);
			

		}

};

int main(int argc, char *argv[])
{
	setvbuf(stdout,NULL,_IONBF,BUFSIZ);

	rclcpp::init(argc,argv);
	rclcpp::spin(std::make_shared<ImageSubscriber>());

	rclcpp::shutdown();
	return 0;
}

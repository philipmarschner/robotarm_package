#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>

#include <cv_bridge/cv_bridge.h>

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

		}

	private:
		rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr camera_subscription_;

		void onImageMsg(const sensor_msgs::msg::Image::SharedPtr msg) {
			RCLCPP_INFO(this->get_logger(), "Received image!");

			cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, msg->encoding);
			cv::Mat img = cv_ptr->image;

			RCLCPP_INFO(this->get_logger(), "Successfully loaded image");
			
			// convert to grayscale
			cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

			// convert to 10x10 image
			cv::resize(img, img, cv::Size(10,10), INTER_AREA);

			// divide by 255 (normalize)
			img.convertTo(img, CV_32F, 1.0/255.0);

			// convert to 1D array
			std::vector<float>vec(img.begin<float>(), img.end<float>());

			// Stream image (vec) to FPGA

			//write data to bram

			//wait for response

			//read data from bram

			//write to dynamixel topic

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

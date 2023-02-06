cmake_minimum_required(VERSION 3.8)
project(FinalProject)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/Modules/FindDynamixelSDK.cmake)

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(dynamixel_sdk REQUIRED)
find_package(dynamixel_sdk_custom_interface REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(rcutils REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rcutils REQUIRED)
find_package(OpenCV REQUIRED)


add_executable(bram_node src/bram_node.cpp)
ament_target_dependencies(bram_node rclcpp std_msgs geometry_msgs)
add_executable(controller_node src/controller_node.cpp)
ament_target_dependencies(controller_node rclcpp geometry_msgs dynamixel_sdk_custom_interface)
add_executable(image_subscriber src/image_subscriber.cpp)
ament_target_dependencies(image_subscriber sensor_msgs rclcpp cv_bridge OpenCV)
add_executable(read_write_node src/read_write_node.cpp)
ament_target_dependencies(read_write_node rclcpp dynamixel_sdk dynamixel_sdk_custom_interface rcutils)

# Dont know if this is needed??
#target_include_directories(image_publisher PUBLIC
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#  $<INSTALL_INTERFACE:include>)
#target_compile_features(image_publisher PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

install(TARGETS
  bram_node
  controller_node
  image_subscriber
  read_write_node
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  #set(ament_cmake_cpplint_FOUND TRUE)
  #ament_lint_auto_find_test_dependencies()
endif()

ament_package()

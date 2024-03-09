// #include <Arduino.h>
// #include <micro_ros_platformio.h>
#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>
#include <sensor_msgs/msg/imu.h>
#include <cube_msgs/msg/imu_data.h>

#include "wifi_settings.h"

rcl_publisher_t publisher;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;
// rcl_node_options_t node_options;
rcl_init_options_t init_options;

// std_msgs__msg__Int32 int_msg;
// sensor_msgs__msg__Imu imu_msg;
cube_msgs__msg__ImuData imu_msg;

#define RCCHECK(fn)              \
  {                              \
    rcl_ret_t temp_rc = fn;      \
    if ((temp_rc != RCL_RET_OK)) \
    {                            \
      error_loop();              \
    }                            \
  }
#define RCSOFTCHECK(fn)          \
  {                              \
    rcl_ret_t temp_rc = fn;      \
    if ((temp_rc != RCL_RET_OK)) \
    {                            \
    }                            \
  }

void error_loop()
{
  while (1)
  {
    delay(100);
    // printf("Error\n");
  }
}

void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL)
  {
    imu_msg.linear_acceleration.x = 1;
    imu_msg.linear_acceleration.y = 2;
    imu_msg.linear_acceleration.z = 3;
    imu_msg.angular_velocity.x = 4;
    imu_msg.angular_velocity.y = 5;
    imu_msg.angular_velocity.z = 6;

    // RCSOFTCHECK(rcl_publish(&publisher, &int_msg, NULL));
    RCSOFTCHECK(rcl_publish(&publisher, &imu_msg, NULL));
  }
}

void setup()
{
  // Serial.begin(115200);
  set_microros_transports();
  set_microros_wifi();

  delay(2000);

  allocator = rcl_get_default_allocator();

  // create init_options
  // RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  init_options = rcl_get_zero_initialized_init_options();
  size_t domain_id = 30;
  RCCHECK(rcl_init_options_init(&init_options, allocator));
  RCCHECK(rcl_init_options_set_domain_id(&init_options, domain_id));
  RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));
  // node_options = rcl_node_get_default_options();
  // RCCHECK(rclc_node_init_with_options(&node, "micro_ros_arduino_node", "", &support, &node_options));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
      &publisher,
      &node,
      // ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32), "/int_test"));
      // ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu), "/imu"));
      ROSIDL_GET_MSG_TYPE_SUPPORT(cube_msgs, msg, ImuData), "/imu"));

  // create timer,
  const unsigned int timer_timeout = 1000;
  RCCHECK(rclc_timer_init_default(
      &timer,
      &support,
      RCL_MS_TO_NS(timer_timeout),
      timer_callback));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));
}

void loop()
{
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}
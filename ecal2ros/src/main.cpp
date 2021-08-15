#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4456)
#endif
#include <rclcpp/rclcpp.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <ecal/ecal.h>
#include <ecal/msg/protobuf/subscriber.h>

#include <functional>
#include <memory>

#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <sensor_msgs/msg/temperature.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
#include "sensor_msgs/navsatfix.pb.h"
#include "sensor_msgs/temperature.pb.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

class GatewayNode : public rclcpp::Node
{
public:
  GatewayNode() : Node("ecal2ros")
  {
    // pub <sensor_msgs::msg::NavSatFix> / sub <pb::sensor_msgs::NavSatFix>
    pub_nav_ = this->create_publisher<sensor_msgs::msg::NavSatFix>("navsatfix_ros", 10);
    sub_nav_ = eCAL::protobuf::CSubscriber<pb::sensor_msgs::NavSatFix>("navsatfix");
    sub_nav_.AddReceiveCallback(std::bind(&GatewayNode::navsatfix_cb, this, std::placeholders::_2));

    // <sensor_msgs::msg::Temperature> / sub <pb::sensor_msgs::Temperature>
    pub_tmp_ = this->create_publisher<sensor_msgs::msg::Temperature>("temperature_ros", 10);
    sub_tmp_ = eCAL::protobuf::CSubscriber<pb::sensor_msgs::Temperature>("temperature");
    sub_tmp_.AddReceiveCallback(std::bind(&GatewayNode::temperature_cb, this, std::placeholders::_2));
  }

private:
  void navsatfix_cb(const pb::sensor_msgs::NavSatFix& msg)
  {
    // header
    msg_tmp_.header.stamp.set__sec(msg.header().stamp().sec()); //-V807
    msg_tmp_.header.stamp.set__nanosec(msg.header().stamp().nanosec());
    msg_tmp_.header.set__frame_id(msg.header().frame_id());

    // status
    switch (msg.status().status())
    {
    case pb::sensor_msgs::NavSatStatus::STATUS_FIX:       // unaugmented fix
      msg_nav_.status.set__status(sensor_msgs::msg::NavSatStatus::STATUS_FIX);
      break;
    case pb::sensor_msgs::NavSatStatus::STATUS_NO_FIX:    // unable to fix position
      msg_nav_.status.set__status(sensor_msgs::msg::NavSatStatus::STATUS_NO_FIX);
      break;
    case pb::sensor_msgs::NavSatStatus::STATUS_SBAS_FIX:  // with satellite-based augmentation
      msg_nav_.status.set__status(sensor_msgs::msg::NavSatStatus::STATUS_SBAS_FIX);
      break;
    case pb::sensor_msgs::NavSatStatus::STATUS_GBAS_FIX:  // with ground-based augmentation
      msg_nav_.status.set__status(sensor_msgs::msg::NavSatStatus::STATUS_GBAS_FIX);
      break;
    default:
      break;
    }

    // latitude
    msg_nav_.set__latitude(msg.latitude());

    // longitude
    msg_nav_.set__longitude(msg.longitude());

    // altitude
    msg_nav_.set__altitude(msg.altitude());

    // position_covariance[]
    std::copy(msg.position_covariance().begin(), msg.position_covariance().end(), msg_nav_.position_covariance.begin());

    // send it to ROS
    pub_nav_->publish(msg_nav_);
  }

  void temperature_cb(const pb::sensor_msgs::Temperature& msg)
  {
    // header
    msg_tmp_.header.stamp.set__sec(msg.header().stamp().sec()); //-V807
    msg_tmp_.header.stamp.set__nanosec(msg.header().stamp().nanosec());
    msg_tmp_.header.set__frame_id(msg.header().frame_id());

    // temperature
    msg_tmp_.set__temperature(msg.temperature());

    // variance
    msg_tmp_.set__variance(msg.variance());

    // send it to ROS
    pub_tmp_->publish(msg_tmp_);
  }

  // pub <sensor_msgs::msg::NavSatFix> / sub <pb::sensor_msgs::NavSatFix>
  rclcpp::Publisher<sensor_msgs::msg::NavSatFix>::SharedPtr    pub_nav_;
  eCAL::protobuf::CSubscriber<pb::sensor_msgs::NavSatFix>                   sub_nav_;
  sensor_msgs::msg::NavSatFix                                  msg_nav_;

  // pub <sensor_msgs::msg::Temperature> / sub <pb::sensor_msgs::Temperature>
  rclcpp::Publisher<sensor_msgs::msg::Temperature>::SharedPtr  pub_tmp_;
  eCAL::protobuf::CSubscriber<pb::sensor_msgs::Temperature>                 sub_tmp_;
  sensor_msgs::msg::Temperature                                msg_tmp_;
};


int main(int argc, char* argv[])
{
  // initialize eCAL and set process state
  eCAL::Initialize(argc, argv, "ecal2ros");
  eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "I'm fine ..");

  // initialize ROS node
  rclcpp::init(argc, argv);

  // start ROS node
  auto node = std::make_shared<GatewayNode>();
  rclcpp::spin(node);

  // shutdown ROS node
  rclcpp::shutdown();

  // finalize eCAL
  eCAL::Finalize();

  return 0;
}

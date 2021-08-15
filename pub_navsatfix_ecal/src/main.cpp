#include <ecal/ecal.h>
#include <ecal/msg/protobuf/publisher.h>

#include "sensor_msgs/navsatfix.pb.h"

int main(int argc, char** argv)
{
  // initialize eCAL and set process state
  eCAL::Initialize(argc, argv, "pub_navsatfix_ecal");
  eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "I'm fine ..");

  // create the publisher
  eCAL::protobuf::CPublisher<pb::sensor_msgs::NavSatFix> pub("navsatfix");

  // create message class instance
  pb::sensor_msgs::NavSatFix msg;

  // enter main loop
  while (eCAL::Ok())
  {
    // reset message
    msg.Clear();

    // fill message
    // header
    auto header = msg.mutable_header();
    header->mutable_stamp()->set_sec(1);
    header->mutable_stamp()->set_nanosec(2);
    header->set_frame_id("id");

    // status
    msg.mutable_status()->set_status(pb::sensor_msgs::NavSatStatus::STATUS_FIX);

    // latitude
    msg.set_latitude(0.1);

    // longitude
    msg.set_longitude(0.2);

    // altitude
    msg.set_altitude(0.3);

    // position_covariance[]
    for (int i = 1; i < 10; ++i)
    {
      msg.add_position_covariance(i*1.0);
    }

    // send message
    pub.Send(msg);

    // sleep 100 ms
    eCAL::Process::SleepMS(100);
  }

  // finalize eCAL API
  eCAL::Finalize();

  return(0);
}

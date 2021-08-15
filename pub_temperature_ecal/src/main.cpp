#include <ecal/ecal.h>
#include <ecal/msg/protobuf/publisher.h>

#include "sensor_msgs/temperature.pb.h"

int main(int argc, char** argv)
{
  // initialize eCAL and set process state
  eCAL::Initialize(argc, argv, "pub_temperature_ecal");
  eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "I'm fine ..");

  // create the publisher
  eCAL::protobuf::CPublisher<pb::sensor_msgs::Temperature> pub("temperature");

  // create message class instance
  pb::sensor_msgs::Temperature msg;

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

    // temperature
    msg.set_temperature(42.0);

    // variance
    msg.set_variance(1.234);

    // send message
    pub.Send(msg);

    // sleep 100 ms
    eCAL::Process::SleepMS(100);
  }

  // finalize eCAL API
  eCAL::Finalize();

  return(0);
}

/*
 *  Copyright (C) 2012 Austin Robot Technology, Jack O'Quin
 * 
 *  License: Modified BSD Software License Agreement
 *
 *  $Id$
 */

/** \file
 *
 *  ROS driver nodelet for the Velodyne 3D LIDARs
 */

#include <string>
#include <boost/thread.hpp>

#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>

#include "driver.h"

namespace velodyne_driver
{

class DriverNodelet: public nodelet::Nodelet
{
public:

  DriverNodelet():
    running_(false)
  {}

  ~DriverNodelet()
  {
    if (running_)
      {
        NODELET_INFO("shutting down driver thread");
        running_ = false;
        deviceThread_->join();
        NODELET_INFO("driver thread stopped");
      }

    dvr_.shutdown();
  }

private:

  virtual void onInit(void);
  virtual void devicePoll(void);

  volatile bool running_;               ///< device thread is running
  boost::shared_ptr<boost::thread> deviceThread_;

  VelodyneDriver dvr_;                  ///< driver implementation class
};

void DriverNodelet::onInit()
{
  dvr_.startup(getNodeHandle(), getPrivateNodeHandle());

  // spawn device thread
  running_ = true;
  deviceThread_ = boost::shared_ptr< boost::thread >
    (new boost::thread(boost::bind(&DriverNodelet::devicePoll, this)));
}

void DriverNodelet::devicePoll()
{
  while(running_)
    {
      // poll device until end of file
      running_ = dvr_.poll();
    }
}

} // namespace velodyne_driver

// Register this plugin with pluginlib.  Names must match nodelet_velodyne.xml.
//
// parameters are: package, class name, class type, base class type
PLUGINLIB_DECLARE_CLASS(velodyne_driver, DriverNodelet,
                        velodyne_driver::DriverNodelet, nodelet::Nodelet);

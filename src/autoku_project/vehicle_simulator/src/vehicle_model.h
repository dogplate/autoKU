#ifndef VEHICLE_MODEL_H
#define VEHICLE_MODEL_H

#include <math.h>
#include <ros/ros.h>
#include <autoku_msgs/VehicleOutput.h>
#include <autoku_msgs/VehicleInput.h>
#include <tf/transform_broadcaster.h>

#define PI 3.14159265358979323846 /* pi */

class Vehicle {
protected:
  ros::NodeHandle m_rosNodeHandler;
  ros::Publisher m_rosPubVehicleOutput;
  ros::Subscriber m_rosSubCmd;
  tf::TransformBroadcaster m_rosTfBroadcaster;

  double m_dDt = 0.0;
  double m_dPrevTime;

  double m_dAccelInput = 0.0;
  double m_dBrakeInput = 0.0;
  double m_dFrontAngle_rps = 0.0;

  double m_dWheelBase = 0.6;
  double m_dLf = 0.3;
  double m_dLr = 0.3;


  double m_dX = 0.0;
  double m_dY = 0.0;
  double m_dYaw = 0.0;
  double m_dYawRate = 0.0;
  double m_dVehicleVel_ms = 0.0;

public:
  Vehicle();
  ~Vehicle();
 
  bool simVehicleModel();
  void setInitialState(double x, double y, double yaw);

  void callback(const autoku_msgs::VehicleInput::ConstPtr &msg);

private:
  void broadcasting_tf();
};

#endif

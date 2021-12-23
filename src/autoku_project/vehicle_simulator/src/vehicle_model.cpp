#include "vehicle_model.h"
#include <math.h>
#include <ros/ros.h>
#include <autoku_msgs/VehicleOutput.h>
#include <autoku_msgs/VehicleInput.h>
#include <geometry_msgs/Pose.h>

#include <tf/transform_broadcaster.h>

#define PI 3.14159265358979323846 /* pi */

Vehicle::Vehicle() {
    m_rosPubVehicleOutput =
    m_rosNodeHandler.advertise<autoku_msgs::VehicleOutput>(
        "vehicle_output", 10);
        
    m_rosSubCmd = m_rosNodeHandler.subscribe("/driving_input", 100,
                                            &Vehicle::callback, this);
    
    double init_x, init_y, init_yaw;
    m_rosNodeHandler.param("model/init_x", init_x, 0.0);
    m_rosNodeHandler.param("model/init_y", init_y, 0.0);
    m_rosNodeHandler.param("model/init_yaw", init_yaw, 0.0);
    this->setInitialState(init_x, init_y, init_yaw);

    


    m_dDt = 0.0;
    m_dPrevTime = ros::Time::now().toSec();



}

Vehicle::~Vehicle() {}


void Vehicle::callback(const autoku_msgs::VehicleInput::ConstPtr& msg) {
    m_dAccelInput = msg->accel;
    m_dBrakeInput = msg->brake;
    m_dFrontAngle_rps = msg->steering;
    }


void Vehicle::setInitialState(double x, double y, double yaw) {
  m_dX = x;
  m_dY = y;
  m_dYaw = yaw;
}

bool Vehicle::simVehicleModel() {
    // Calculate dt_ by dividing the previous time and the current time
    m_dDt = ros::Time::now().toSec() - m_dPrevTime;
    if (m_dDt < 0) {
    return false;
    }

    m_dPrevTime = ros::Time::now().toSec();


    ////////////////////////////////////////////////////////////////////////////////
    // Kinematics simulation using velocity and steering angle
  
    m_dVehicleVel_ms = m_dVehicleVel_ms + m_dDt*(m_dAccelInput - m_dBrakeInput);

    double beta = atan2((m_dLf * tan(m_dFrontAngle_rps) ),(m_dLf + m_dLr));

    double dx = m_dDt * m_dVehicleVel_ms * cos(m_dYaw + beta);
    double dy = m_dDt * m_dVehicleVel_ms * sin(m_dYaw + beta);
    double dyaw = m_dDt * m_dVehicleVel_ms * cos(beta)/(m_dLf + m_dLr)*(tan(m_dFrontAngle_rps) );

    m_dYaw += dyaw;
    m_dX += dx;
    m_dY += dy;
        
    broadcasting_tf();

    autoku_msgs::VehicleOutput output;
    ROS_INFO_STREAM("\nX :\t"<<m_dX<<"\nY :\t"<<m_dY<<"\nYaw :\t"<<m_dYaw<<"\nVel :\t"<<m_dVehicleVel_ms*3.6<<"\n\n");

    output.x = m_dX;
    output.y = m_dY;
    output.yaw = m_dYaw;
    output.velocity = m_dVehicleVel_ms;
    output.id = "";
    m_rosPubVehicleOutput.publish(output);

    return true;
}

void Vehicle::broadcasting_tf() {
    tf::Quaternion q;
    
    q.setRPY(0, 0, m_dYaw);
    q.normalize();
    geometry_msgs::Pose pose;

    pose.position.x = m_dX;
    pose.position.y = m_dY;
    pose.orientation.x = q.getX();
    pose.orientation.y = q.getY();
    pose.orientation.z = q.getZ();
    pose.orientation.w = q.getW();

    tf::Transform transform;

    transform.setOrigin(tf::Vector3(m_dX, m_dY, 0.0));
    transform.setRotation(q);

    // broadcasting the vehicle's body coordinate system
    // The parent coordinate is world, child coordinate is body.
    m_rosTfBroadcaster.sendTransform(tf::StampedTransform(
        transform, ros::Time::now(), "/world", "/body"));
}


int main(int argc, char** argv) {
  ros::init(argc, argv, "vehicle");
  Vehicle vehicle;

  // The approximate control time is 100 Hz
  ros::Rate loop_rate(100);
  
  while (ros::ok()) {
    vehicle.simVehicleModel();
    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}

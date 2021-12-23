#include "autoku_msgs/WaypointData.h"
#include "autoku_msgs/VehicleOutput.h"
#include "visualization_msgs/Marker.h"
#include "visualization_msgs/MarkerArray.h"
#include <math.h>
#include <ros/ros.h>
#include <tf/tf.h>

#define PI 3.1415926579

class Display {
protected:
  ros::NodeHandle m_rosNodeHandler;


  ros::Publisher m_rosPubWaypointMarkerArray;
  ros::Subscriber m_rosSubWaypoints;

  ros::Publisher m_rosPubVehicleMarker;
  ros::Subscriber m_rosSubVehicleOutput;

public:
  Display() {

    m_rosPubWaypointMarkerArray =
        m_rosNodeHandler.advertise<visualization_msgs::MarkerArray>(
            "csv_waypoints_marker", 10);
            
    m_rosSubWaypoints = m_rosNodeHandler.subscribe(
        "csv_waypoints", 10, &Display::csvWaypointsCallback, this);


    m_rosPubVehicleMarker =
        m_rosNodeHandler.advertise<visualization_msgs::Marker>("vehicle_mark",
                                                               100);
    m_rosSubVehicleOutput = m_rosNodeHandler.subscribe(
      "vehicle_output", 10, &Display::vehicleOutputCallback, this);


  }

  ~Display() {}

protected:
  autoku_msgs::WaypointData m_csvWaypoints;

public:
  void csvWaypointsCallback(const autoku_msgs::WaypointData::ConstPtr &msg) {
    m_csvWaypoints.frame_id = msg->frame_id;
    m_csvWaypoints.points = msg->points;
  }

  // Waypoints visualization
  void mark_csvWaypoints() {

    int id = 0;

    visualization_msgs::MarkerArray markerArray;
    ROS_INFO_STREAM(m_csvWaypoints.points.size());
    if(m_csvWaypoints.points.size()>0){
      for (auto i_point = 0; i_point < m_csvWaypoints.points.size()-1; i_point++) {
        visualization_msgs::Marker marker;
        
        marker.header.frame_id = m_csvWaypoints.frame_id;
        marker.header.stamp = ros::Time::now();
        marker.id = id++;
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.orientation.w = 1.0;
        marker.color.r = 0.03f;
        marker.color.g = 0.9f;
        marker.color.b = 0.1f;
        marker.color.a = 0.7;
        marker.scale.x = 0.5;
        marker.scale.y = 0.9;
        marker.lifetime = ros::Duration();
        marker.type = visualization_msgs::Marker::ARROW;
        
        geometry_msgs::Point startPoint, endPoint;
        startPoint.x = m_csvWaypoints.points[i_point].x;
        startPoint.y = m_csvWaypoints.points[i_point].y;
        endPoint.x = m_csvWaypoints.points[i_point+1].x;
        endPoint.y = m_csvWaypoints.points[i_point+1].y;
        marker.points.push_back(startPoint);
        marker.points.push_back(endPoint);
        markerArray.markers.push_back(marker);
      }

      m_rosPubWaypointMarkerArray.publish(markerArray);
    }
    
  }

  protected:
    std::string m_sVehicle_id = "";
    double m_dVehicleX = 0.0;
    double m_dVehicleY = 0.0;
    double m_dVehicleYaw = 0.0;
    double m_dVehicleVel = 0.0;
    bool m_isVehicleExist = false;

  public:
    void vehicleOutputCallback(const autoku_msgs::VehicleOutput::ConstPtr &msg){
      m_sVehicle_id = msg->id;
      m_dVehicleX = msg->x;
      m_dVehicleY = msg->y;
      m_dVehicleYaw = msg->yaw;
      m_dVehicleVel = msg->velocity;    
      m_isVehicleExist = true;
    }

    void markVehicle() {
      if (m_isVehicleExist == true) {
        tf::Quaternion q_temp;
        tf::Matrix3x3 m(q_temp);
        q_temp.setRPY(0.0 / 180.0 * PI, 0, 90.0 / 180.0 * PI + m_dVehicleYaw);
        tf::Quaternion q(q_temp.getX(), q_temp.getY(), q_temp.getZ(),
                        q_temp.getW());

        visualization_msgs::Marker vehicle_marker;
        vehicle_marker.header.frame_id = "/world";
        vehicle_marker.header.stamp = ros::Time::now();
        vehicle_marker.ns = m_sVehicle_id;
        vehicle_marker.id = 0;
        // Set the marker type
        vehicle_marker.type = visualization_msgs::Marker::MESH_RESOURCE;
        vehicle_marker.mesh_resource =
            "package://vehicle_simulator/meshes/dilivery_platform.dae";
        vehicle_marker.mesh_use_embedded_materials = true;
        // vehicle_marker.type = visualization_msgs::Marker::CUBE;
        
        vehicle_marker.pose.position.x = m_dVehicleX;
        vehicle_marker.pose.position.y = m_dVehicleY;
        vehicle_marker.pose.position.z = 0.0;
        vehicle_marker.pose.orientation.x = q.getX();
        vehicle_marker.pose.orientation.y = q.getY();
        vehicle_marker.pose.orientation.z = q.getZ();
        vehicle_marker.pose.orientation.w = q.getW();
        // Set the scale of the marker
        // vehicle_marker.scale.x = 0.675;
        // vehicle_marker.scale.y = 1.0;
        // vehicle_marker.scale.z = 0.8;
        vehicle_marker.scale.x = 1.0;
        vehicle_marker.scale.y = 1.0;
        vehicle_marker.scale.z = 1.0;

        vehicle_marker.color.r = 1.0;
        vehicle_marker.color.g = 1.0;
        vehicle_marker.color.b = 1.0;
        vehicle_marker.color.a = 1.0;

        vehicle_marker.lifetime = ros::Duration(0.1);

        m_rosPubVehicleMarker.publish(vehicle_marker);
      }
    }
};

int main(int argc, char **argv) {
  ros::init(argc, argv, "display");
  // Vehicle vehicle;
  Display display;

  double prev_vehiclesMarkTime = ros::Time::now().toSec();
  double prev_csvWaypointsMarkTime = ros::Time::now().toSec();
  
  // 60 Hz visualization
  ros::Rate loop_rate(60);
  while (ros::ok()) {

    if ((ros::Time::now().toSec() - prev_csvWaypointsMarkTime) > 1.0) {
      prev_csvWaypointsMarkTime = ros::Time::now().toSec();
      display.mark_csvWaypoints();
    }
    if ((ros::Time::now().toSec() - prev_vehiclesMarkTime) > 0.1) {
      prev_vehiclesMarkTime = ros::Time::now().toSec();
      display.markVehicle();
    }
    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}

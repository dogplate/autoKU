#include <ros/ros.h>

#include <autoku_msgs/VehicleOutput.h>
#include <autoku_msgs/VehicleInput.h>
#include <autoku_msgs/LocalWaypointData.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/PointStamped.h>
#include <tf/transform_listener.h>

#include <cmath>

#define MAX_SPEED 15 // km/h
#define WHEELBASE 1.0
#define P_STEER 0.03

class VehicleController {
protected:
    ros::NodeHandle m_rosNodeHandler;

    ros::Subscriber m_rosSubLocalWaypoints;
    ros::Subscriber m_rosSubVehicleOutput;

    ros::Publisher m_rosPubVehicleInput;

	autoku_msgs::VehicleInput m_vehicleInput;

public:
    bool m_getVehicleOutput = false;
    bool m_getWaypoint = false;
    
    VehicleController(){
        m_rosSubLocalWaypoints = m_rosNodeHandler.subscribe(
            "local_waypoints", 10, &VehicleController::localWaypointsCallback, this);

        m_rosSubVehicleOutput = m_rosNodeHandler.subscribe(
            "vehicle_output", 10, &VehicleController::vehicleOutputCallback, this);
    

    	m_rosPubVehicleInput =
			m_rosNodeHandler.advertise<autoku_msgs::VehicleInput>(
				"driving_input", 10);
	}

    ~VehicleController() {}

protected:
   

    autoku_msgs::LocalWaypointData m_localWaypoints;
    autoku_msgs::VehicleOutput m_vehicleOutput;

    void vehicleOutputCallback(const autoku_msgs::VehicleOutput::ConstPtr &msg){
        m_vehicleOutput = *msg;
        m_getVehicleOutput = true;
        
    }
    void localWaypointsCallback(const autoku_msgs::LocalWaypointData::ConstPtr &msg) {
        m_localWaypoints = *msg;
        m_getWaypoint = true;
    }

public:

    void lateralController() {

        int lookahead_point = int(10*(m_vehicleOutput.velocity*0.237+4.1));

        ROS_INFO_STREAM("lp:"<<lookahead_point <<" td: "<< m_localWaypoints.points[lookahead_point].x<<"  "<<m_localWaypoints.points[lookahead_point].y);

        double target_distance = pow(pow(m_localWaypoints.points[lookahead_point].x,2)+pow(m_localWaypoints.points[lookahead_point].y,2),0.5);

        double target_alpha = atan2(m_localWaypoints.points[lookahead_point].y,m_localWaypoints.points[lookahead_point].x);
        double s_error = sin(target_alpha)*target_distance;

        double steer_output = atan2(2.0*WHEELBASE*s_error,target_distance*target_distance);

        double steer_p_error = P_STEER*m_localWaypoints.points[3].y;

        ROS_INFO_STREAM("s in: "<< s_error << " " << target_distance <<" "<<WHEELBASE);

        steer_output = steer_output + steer_p_error;
            
        
        ROS_INFO_STREAM("s out: "<< steer_output*180/M_PI);

        m_vehicleInput.steering = steer_output;

    }

    void longitudinalController() {

        double target_speed = MAX_SPEED / 3.6;
        if (m_vehicleOutput.velocity > target_speed){
            m_vehicleInput.accel = 0.0;
            m_vehicleInput.brake = 1.0;
        }
        else{
            m_vehicleInput.accel = 1.0;
            m_vehicleInput.brake = 0.0;
        }
    }

    void pubVehicleInput() {
        m_rosPubVehicleInput.publish(m_vehicleInput);
    }


};


int main(int argc, char** argv) {
    ros::init(argc, argv, "vehicle_controller");
    VehicleController controller;

    // The approximate control time is 100 Hz
    ros::Rate loop_rate(100);
    
    while (ros::ok()) {
        if(controller.m_getVehicleOutput && controller.m_getWaypoint){
            controller.lateralController();
            controller.longitudinalController();
            controller.pubVehicleInput();
        }
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}




#include <ros/ros.h>

#include <autoku_msgs/VehicleOutput.h>
#include <autoku_msgs/WaypointData.h>
#include <autoku_msgs/LocalWaypointData.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/PointStamped.h>
#include <tf/transform_listener.h>

#include <limits>
#include <iostream>
#include <cstring>
#include <cmath>
#include <functional>
#include <cassert>
#include <vector>

#define TOTAL_LOOKAHEAD_WAYPOINT_IDX 10
#define WAYPOINT_RESOLUTION 5 // (m) TARGET WAYPOINT RESOLUTION : 0.1m

class CubicHermiteSpline
{
public:
	CubicHermiteSpline(const double* xVec, const double* yVec, const double* mVec, const size_t size)
	: xVec_(nullptr), yVec_(nullptr), mVec_(nullptr), size_(0)
	{
		allocate_heap_memory_(xVec, yVec, mVec, size);
	}


	virtual ~CubicHermiteSpline()
	{
		delete_heap_memory_();
	}


	double operator()(const double x) const
	{
		const size_t idx = binary_search_(x);

		if(idx == size_ - 1) {
			return yVec_[size_-1];
		}

		const double t = (x - xVec_[idx])/(xVec_[idx+1] - xVec_[idx]);

		return interp_f_(t, idx);
	}


protected:
	double* xVec_;
	double* yVec_;
	double* mVec_;
	size_t size_;

	explicit CubicHermiteSpline()
	: xVec_(nullptr), yVec_(nullptr), mVec_(nullptr), size_(0)
	{}


	void delete_heap_memory_()
	{
		if(xVec_ != nullptr) {
			delete [] xVec_;
		}
		if(yVec_ != nullptr) {
			delete [] yVec_;
		}
		if(mVec_ != nullptr) {
			delete [] mVec_;
		}
	}


	void allocate_heap_memory_(const double* xVec, const double* yVec, const double* mVec, const size_t size)
	{
		delete_heap_memory_();

		xVec_ = new double [size];
		yVec_ = new double [size];
		mVec_ = new double [size];
		size_ = size;

		std::memcpy(xVec_, xVec,sizeof(double)*size);
		std::memcpy(yVec_, yVec,sizeof(double)*size);
		std::memcpy(mVec_, mVec,sizeof(double)*size);
	}


	size_t binary_search_(const double& x) const
	{
        // std::cout<< x <<", "<< xVec_[0] <<", "<< xVec_[size_-1] <<std::endl;
		assert(xVec_[0] <= x && x <= xVec_[size_-1]);
		size_t idx_l = 0, idx_r = size_ - 1, idx = size_/2;

		auto is_x_in_Boundary = [this, &x](const size_t& idx) -> bool
					{
						return this -> xVec_[idx] <= x and x < this -> xVec_[idx + 1];
					};

		while(1)
		{
			if(idx_r - idx_l == 1) 
			{
				if(is_x_in_Boundary(idx)) {
					return idx;
				}
				else {
					return idx + 1;
				}
			}

			if(is_x_in_Boundary(idx)) {
				return idx;
			}
			else if(xVec_[idx+1] <= x)
			{
				idx_l = idx;
				idx = (idx_r - idx_l)/2 + idx_l;
			}
			else
			{
				idx_r = idx;
				idx = (idx_r - idx_l)/2 + idx_l;
			}
		}
	}


	double interp_f_(const double& t, const size_t& idx) const
	{
		return (2*std::pow(t,3) - 3*std::pow(t,2) + 1)*yVec_[idx] +
		(std::pow(t,3) - 2*std::pow(t,2) + t)*(xVec_[idx+1] - xVec_[idx])*mVec_[idx] +
		(-2*std::pow(t,3) + 3*std::pow(t,2))*yVec_[idx+1] +
		(std::pow(t,3) - std::pow(t,2))*(xVec_[idx+1] - xVec_[idx])*mVec_[idx+1];
	}
};


class MonotoneCubicInterpolation : public CubicHermiteSpline
{
public:
	MonotoneCubicInterpolation(const double* xVec, const double* yVec, const size_t size)
	: CubicHermiteSpline()
	{
		std::vector<double> delta(size, 0);
		std::vector<double> mVec(size, 0);

		for(int i=0; i<size-1; ++i) {
			delta[i] = (yVec[i+1] - yVec[i])/(xVec[i+1] - xVec[i]);
		}

		for(int i=1; i<size-1; ++i) {
			mVec[i] = (delta[i-1] + delta[i])/2.;
		}
	
		mVec[0] = delta[0]; mVec[size-1] = delta[size-2];

		for(int i=0; i<size-1; ++i)
		{
			if(std::abs(delta[i]) < 1e-30) {
				mVec[i] = mVec[i+1] = 0.;
			}
		}

		allocate_heap_memory_(xVec, yVec, &mVec[0], size);
	}

	virtual ~MonotoneCubicInterpolation() {}
};


class LocalWaypointGenerator {
protected:
    ros::NodeHandle m_rosNodeHandler;

    ros::Subscriber m_rosSubWaypoints;
    ros::Subscriber m_rosSubVehicleOutput;

    ros::Publisher m_rosPubLocalWaypoints;

	autoku_msgs::LocalWaypointData m_localWaypoints;

public:
    LocalWaypointGenerator(){
        m_rosSubWaypoints = m_rosNodeHandler.subscribe(
            "csv_waypoints", 10, &LocalWaypointGenerator::csvWaypointsCallback, this);

        m_rosSubVehicleOutput = m_rosNodeHandler.subscribe(
            "vehicle_output", 10, &LocalWaypointGenerator::vehicleOutputCallback, this);
    

    	m_rosPubLocalWaypoints =
			m_rosNodeHandler.advertise<autoku_msgs::LocalWaypointData>(
				"local_waypoints", 10);
	}

    ~LocalWaypointGenerator() {}

protected:
    bool m_getVehicleOutput = false;
    bool m_getWaypoint = false;
    std::vector<double> local_waypoints_sparse_x_;
    std::vector<double> local_waypoints_sparse_y_;
    autoku_msgs::WaypointData m_csvWaypoints;
    autoku_msgs::VehicleOutput m_vehicleOutput;

    void vehicleOutputCallback(const autoku_msgs::VehicleOutput::ConstPtr &msg){
        m_vehicleOutput = *msg;
        m_getVehicleOutput = true;
        
    }
    void csvWaypointsCallback(const autoku_msgs::WaypointData::ConstPtr &msg) {
        m_csvWaypoints = *msg;
        m_getWaypoint = true;
    }
public:

    void calNextWaypoint() {
        
        double maxval = std::numeric_limits<double>::max() ;
        int maxidx = std::numeric_limits<int>::max();

        if(m_getVehicleOutput && m_getWaypoint){
            for(int waypoint_idx = 0; waypoint_idx < m_csvWaypoints.points.size(); waypoint_idx++ ){
                double dist = pow(pow(m_vehicleOutput.x - m_csvWaypoints.points[waypoint_idx].x , 2) + 
                                pow(m_vehicleOutput.y - m_csvWaypoints.points[waypoint_idx].y , 2) , 0.5);
                if(dist < maxval){  
                    maxval = dist;
                    maxidx = waypoint_idx;
                }
            }

            int total_lookahead_waypoint_idx = maxidx + TOTAL_LOOKAHEAD_WAYPOINT_IDX;
            if(total_lookahead_waypoint_idx > m_csvWaypoints.points.size()){
                total_lookahead_waypoint_idx = m_csvWaypoints.points.size();
            } 

			// tf::StampedTransform transform;
			// waypoints sparse -> local 
			tf::TransformListener listener;
			
			try
			{
				listener.waitForTransform("/body", "/world", ros::Time(0), ros::Duration(1.5));
				// listener.lookupTransform("/body", "/world", ros::Time(0), transform);

				// std::vector<std::pair<double,double>> local_waypoints_sparse;
				local_waypoints_sparse_x_.clear();
				local_waypoints_sparse_y_.clear();
				for(int local_waypoint_idx = maxidx; local_waypoint_idx < total_lookahead_waypoint_idx ; local_waypoint_idx++ ){
					// local_waypoints_sparse.push_back(std::pair<double,double>(m_csvWaypoints.points[local_waypoint_idx].x, m_csvWaypoints.points[local_waypoint_idx].y));
					geometry_msgs::PointStamped global_waypoint;
					geometry_msgs::PointStamped local_waypoint;

					global_waypoint.header.frame_id  = "/world";
					global_waypoint.point.x = m_csvWaypoints.points[local_waypoint_idx].x;
					global_waypoint.point.y = m_csvWaypoints.points[local_waypoint_idx].y;
					global_waypoint.point.z = 0.0;
									
					listener.transformPoint("/body", global_waypoint, local_waypoint);
					
					// std::cout<< local_waypoint.point.x <<" "<< local_waypoint.point.y<<std::endl;

					local_waypoints_sparse_x_.push_back(local_waypoint.point.x);
					local_waypoints_sparse_y_.push_back(local_waypoint.point.y);
				}
				calLocalWaypoint();

			}
			catch (tf::TransformException ex)
			{
				ROS_ERROR("%s", ex.what());
				ros::Duration(1.0).sleep();
			}

        
        }
    }

    void calLocalWaypoint(){
		MonotoneCubicInterpolation interpolated_points(&local_waypoints_sparse_x_[0], &local_waypoints_sparse_y_[0], 10);
            
			double start = (local_waypoints_sparse_x_.front());
			double end   = (local_waypoints_sparse_x_.back());
			
			double step = 0.5/WAYPOINT_RESOLUTION;

			m_localWaypoints.points.clear();
			
			m_localWaypoints.frame_id = "/body";

			for(double i=start; i<end; i += step) 
			{
				geometry_msgs::Point waypoint;
				if(i>0.0){
					waypoint.x = i;
					waypoint.y = interpolated_points(i);
					waypoint.z = 0.0;

					m_localWaypoints.points.push_back(waypoint);
				}
			}
			m_rosPubLocalWaypoints.publish(m_localWaypoints);
    }
};


int main(int argc, char** argv) {
    ros::init(argc, argv, "local_waypoint_generator");
    LocalWaypointGenerator localWayGen;

    // The approximate control time is 100 Hz
    ros::Rate loop_rate(100);
    
    while (ros::ok()) {
        localWayGen.calNextWaypoint();
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}




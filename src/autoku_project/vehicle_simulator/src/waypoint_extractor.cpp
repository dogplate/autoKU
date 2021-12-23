#include "autoku_msgs/WaypointData.h"
#include "geometry_msgs/Point.h"
#include "std_msgs/Float64.h"
#include <math.h>
#include <ros/ros.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

class CSVParser {
    public:
    std::string readFileIntoString(const std::string& path){
        auto ss = std::ostringstream{};
        std::ifstream input_file(path);
        if (!input_file.is_open()) {
            std::cerr << "Could not open the file - '"
                << path << "'" << std::endl;
            exit(EXIT_FAILURE);
        }
        ss << input_file.rdbuf();
        ROS_INFO_STREAM("Read CSV file");
        return ss.str();
    }

    std::vector<std::vector<double>> m_csv_contents;

    void parsing_csv(){
        std::string filename("/home/labdog/autoKU_ws/KU_planning_data/Konkuk_Waypoints_full.csv");
        std::string file_contents;
        std::vector<std::vector<double>> csv_contents;
        char delimiter = ',';

        file_contents = readFileIntoString(filename);
        std::istringstream sstream(file_contents);

        std::string record;
        int row_cnt = 0;
        while (std::getline(sstream, record)){
            std::istringstream line(record);
            if(row_cnt == 0){ row_cnt++; continue;}
         
            int col_cnt = 0;
            std::vector<double> row_data;

            while (std::getline(line, record, delimiter)) {
                col_cnt++;
                row_data.push_back(std::stod(record));
                
            }
            row_cnt++;
            csv_contents.push_back(row_data);
        }

        m_csv_contents = csv_contents;

    }
};

class CSV2Point {
protected:
    ros::NodeHandle m_rosNodeHandler;
    ros::Publisher m_rosPubWaypoints;

    autoku_msgs::WaypointData m_csvWaypoints;
    std::vector<std::vector<double>> m_csv_vector;

public:
    CSV2Point() {
    m_rosPubWaypoints =
        m_rosNodeHandler.advertise<autoku_msgs::WaypointData>(
            "csv_waypoints", 10);
    }

    ~CSV2Point() {}

    void loadWaypointData() {
        CSVParser csv_parser;
        csv_parser.parsing_csv();
        m_csv_vector = csv_parser.m_csv_contents;
    }

    void parsedCsv2msg() {
        m_csvWaypoints.frame_id = "/world";
        for(int i_row = 0; i_row<m_csv_vector.size(); i_row++){
            geometry_msgs::Point waypoint;

            // loop only for x, y, z, yaw, target velocity (index 0 to 4)
            for(int j_col = 0; j_col<m_csv_vector[i_row].size(); j_col++){
                if(j_col == 0){waypoint.x = m_csv_vector[i_row][j_col];}
                if(j_col == 1){waypoint.y = m_csv_vector[i_row][j_col];}
                if(j_col == 2){waypoint.z = m_csv_vector[i_row][j_col];}
            }
            m_csvWaypoints.points.push_back(waypoint);
            m_csvWaypoints.yaw.push_back(m_csv_vector[i_row][3]);
            m_csvWaypoints.target_velocity.push_back(m_csv_vector[i_row][4]);
        }

    }

    void publishCsvWaypointData() { m_rosPubWaypoints.publish(m_csvWaypoints); }

};

int main(int argc, char **argv) {
    ros::init(argc, argv, "csv2waypoints");
    
    CSV2Point CSV2Waypoint;

    CSV2Waypoint.loadWaypointData();
    CSV2Waypoint.parsedCsv2msg();
    
    ros::Rate loop_rate(1);
    while (ros::ok()) {
        CSV2Waypoint.publishCsvWaypointData();
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}

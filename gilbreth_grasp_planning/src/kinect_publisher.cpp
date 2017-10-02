// This node is a kinect camera publisher, which publishes the point cloud data once the laser beam is triggered.
// It could be later replaced by a plugin in Gazebo.

#include <ros/ros.h>
#include <osrf_gear/Proximity.h>
#include <sensor_msgs/PointCloud2.h>

sensor_msgs::PointCloud2 kinect_data;

class kinectPublisherClass
{
  public:
  	explicit kinectPublisherClass(ros::NodeHandle & node) {
      //kinect publisher, publish to "/gilbreth/kinect_points"		
	  kinect_publisher = node.advertise<sensor_msgs::PointCloud2>("/gilbreth/kinect_points", 10);
	}

    //callback function for break beam subscriber
    void break_beam_callback(const osrf_gear::Proximity::ConstPtr & msg) {
	  if ( msg -> object_detected) { 	  // If there is an object in laser break beam.
        ROS_INFO("Break beam triggered." );
        //wait for 0.5 second to get the points data where the new-coming object is in the center of the image
        //the waiting time is depending on the location of kinect camera
	    ros::Duration(0.5).sleep();
        kinect_publisher.publish(kinect_data);
   		ROS_INFO("Kinect point cloud data published.");
	  }
	}

    void kinect_callback(const sensor_msgs::PointCloud2::ConstPtr & msg) {
	  kinect_data = *msg;
	}
	
  private:
        ros::Publisher kinect_publisher;
};

int main(int argc, char ** argv){  	
  ros::init(argc, argv, "gilbreth_kinect_publish_node");

  ros::NodeHandle node;
  ros::Rate rate(10);

  //Instance object detection class;
  kinectPublisherClass kinect_publisher_class(node);

  //Subscribe to laser beam topic "/ariac/break_beam_change"
  ros::Subscriber break_beam_subscriber = node.subscribe("/gilbreth/break_beam_sensor_change", 10, &kinectPublisherClass::break_beam_callback, &kinect_publisher_class);
  //Subscribe to original point cloud data "/camera1/depth/points"
  ros::Subscriber kinect_callback = node.subscribe("/depth_camera_/depth_camera_/depth/points", 10, &kinectPublisherClass::kinect_callback, &kinect_publisher_class);

  ros::spin();
  
  return 0;
}

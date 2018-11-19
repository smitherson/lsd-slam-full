/**
* This file is part of LSD-SLAM.
*
* Copyright 2013 Jakob Engel <engelj at in dot tum dot de> (Technical University of Munich)
* For more information see <http://vision.in.tum.de/lsdslam> 
*
* LSD-SLAM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* LSD-SLAM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with LSD-SLAM. If not, see <http://www.gnu.org/licenses/>.
*/

#include "LiveSLAMWrapper.h"

#include <boost/thread.hpp>
#include "util/settings.h"
#include "util/globalFuncs.h"
#include "SlamSystem.h"

#include <sstream>
#include <fstream>
#include <dirent.h>
#include <algorithm>

#include "IOWrapper/OpenCVIO/OpenCVOutput3DWrapper.h"
#include "IOWrapper/OpenCVIO/opencvReconfigure.h"

#include "util/Undistorter.h"
#include <ros/package.h>

#include "opencv2/opencv.hpp"

using namespace lsd_slam;
int main( int argc, char** argv )
{
	//ros::init(argc, argv, "LSD_SLAM");

	//dynamic_reconfigure::Server<lsd_slam_core::LSDParamsConfig> srv(ros::NodeHandle("~"));
	//srv.setCallback(dynConfCb);

	//dynamic_reconfigure::Server<lsd_slam_core::LSDDebugParamsConfig> srvDebug(ros::NodeHandle("~Debug"));
	//srvDebug.setCallback(dynConfCbDebug);

	packagePath = "./"; //ros::package::getPath("lsd_slam_core")+"/";

	// get camera calibration in form of an undistorter object.
	// if no undistortion is required, the undistorter will just pass images through.
	std::string calibFile;
	Undistorter* undistorter = 0;
	/*if(ros::param::get("~calib", calibFile))
	{
		 ros::param::del("~calib");
	}*/

    undistorter = Undistorter::getUndistorterForFile(argv[2]);

	if(undistorter == 0)
	{
		printf("need camera calibration file! (set using _calib:=FILE)\n");
		exit(0);
	}

	int w = undistorter->getOutputWidth();
	int h = undistorter->getOutputHeight();

	int w_inp = undistorter->getInputWidth();
	int h_inp = undistorter->getInputHeight();

	float fx = undistorter->getK().at<double>(0, 0);
	float fy = undistorter->getK().at<double>(1, 1);
	float cx = undistorter->getK().at<double>(2, 0);
	float cy = undistorter->getK().at<double>(2, 1);
	Sophus::Matrix3f K;
	K << fx, 0.0, cx, 0.0, fy, cy, 0.0, 0.0, 1.0;


	// make output wrapper. just set to zero if no output is required.
	Output3DWrapper* outputWrapper = new OpenCVOutput3DWrapper(w,h);


	// make slam system
	SlamSystem* system = new SlamSystem(w, h, K, doSlam);
	system->setVisualization(outputWrapper);



	// open image files: first try to open as file.
	std::string source;
	std::vector<std::string> files;
	/*if(!ros::param::get("~files", source))
	{
		printf("need source files! (set using _files:=FOLDER)\n");
		exit(0);
	}
	ros::param::del("~files");*/


	// get HZ
	double hz =0;
	/*if(!ros::param::get("~hz", hz))
		hz = 0;
	ros::param::del("~hz");*/



	cv::Mat image = cv::Mat(h,w,CV_8U);
	int runningIDX=0;
	float fakeTimeStamp = 0;

	//ros::Rate r(hz);
    cv::VideoCapture cap =  cv::VideoCapture(argv[1]); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        exit(1);

	for(;;)
	{


		cv::Mat imageDist;// = cv::imread(files[i], CV_LOAD_IMAGE_GRAYSCALE);
        cap >> imageDist; // get a new frame from camera
        cvtColor(imageDist, imageDist, CV_BGR2GRAY);
        if (imageDist.rows==0){
            break;
        }
		if(imageDist.rows != h_inp || imageDist.cols != w_inp)
		{
			if(imageDist.rows * imageDist.cols == 0)
				printf("failed to load image  skipping.\n" );
			else
				printf("image has wrong dimensions - expecting %d x %d, found %d x %d. Skipping.\n",
						w,h,imageDist.cols, imageDist.rows);
			continue;
		}
		assert(imageDist.type() == CV_8U);

		undistorter->undistort(imageDist, image);
        //imshow("undistorted: ", image);
		assert(image.type() == CV_8U);

		if(runningIDX == 0)
			system->randomInit(image.data, fakeTimeStamp, runningIDX);
		else
			system->trackFrame(image.data, runningIDX ,hz == 0,fakeTimeStamp);
		runningIDX++;
		fakeTimeStamp+=0.03;

		//if(hz != 0)
			//r.sleep();

		if(fullResetRequested)
		{

			printf("FULL RESET!\n");
			delete system;

			system = new SlamSystem(w, h, K, doSlam);
			system->setVisualization(outputWrapper);

			fullResetRequested = false;
			runningIDX = 0;
		}

		//ros::spinOnce();

		/*if(!ros::ok())
			break;*/
	}


	system->finalize();



	delete system;
	delete undistorter;
	delete outputWrapper;
	return 0;
}

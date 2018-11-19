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
* along with dvo. If not, see <http://www.gnu.org/licenses/>.
*/

#include <sstream>

#include <iostream>
#include <memory>
#include "boost/thread.hpp"
#include "settings.h"
#include "PointCloudViewer.h"

//#include "lsd_slam_viewer/ViewerParamsConfig.h"
#include <qapplication.h>


#include "DataStructures/keyframeGraphMsg.h"
#include "DataStructures/keyframeMsg.h"
#include <iostream>
#include <exception>
#include <array>
#include <boost/asio.hpp>

#include "boost/foreach.hpp"
//#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

using namespace std;

PointCloudViewer* viewer = 0;


void dataReader( int argc, char** argv ) {
    boost::asio::io_service aios;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 6660);
    boost::asio::ip::tcp::acceptor acceptor(aios, endpoint);


    boost::asio::ip::tcp::iostream s;
    std::cout << "Server ready" << std::endl;
    acceptor.accept(*s.rdbuf()); // 2
    boost::archive::binary_iarchive ar(s);

    for(;;)
    {
        lsd_slam::keyframeMsg message;
        ar & message; 
        cout<< "Reading frame with id: " << message.id << endl;

        if (message.isKeyframe) {
            cout << "It is a keyframe, reading pointcloud.\n";
            cout.flush();
            ar >> message.pointcloud; 
        }
 
                std::cout << std::endl;
        viewer->addFrameMsg(&message);
    }
}

int main( int argc, char** argv )
{
	printf("Started QApplication thread\n");
	// Read command lines arguments.
	QApplication application(argc,argv);

	// Instantiate the viewer.
	viewer = new PointCloudViewer();

	#if QT_VERSION < 0x040000
		// Set the viewer as the application main widget.
		application.setMainWidget(viewer);
	#else
		viewer->setWindowTitle("PointCloud Viewer");
	#endif

	// Make the viewer window visible on screen.
	viewer->show();

    boost::thread readThread = boost::thread(dataReader, argc, argv);

    application.exec();

	printf("Shutting down... \n");
    readThread.join();
	printf("Done. \n");

}

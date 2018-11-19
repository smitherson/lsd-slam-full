#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
#include "DataStructures/keyframeMsg.h"
#include "IOWrapper/NotifyBuffer.h"
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>



namespace lsd_slam
{



class Publisher : public Notifiable  {

public:
 Publisher(char* address, char* port);
 ~Publisher();
 void publish(keyframeMsg* fMsg);
 void loop();
private:
    NotifyBuffer<lsd_slam::keyframeMsg*> *messages;
    boost::asio::ip::tcp::iostream *streamToRemote;
    boost::archive::binary_oarchive *archive;
    
    boost::thread writeThread;
};

}

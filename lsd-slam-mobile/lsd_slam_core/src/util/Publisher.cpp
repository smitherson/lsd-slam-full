#include "Publisher.h"
#include "util/globalFuncs.h"

namespace lsd_slam
{


Publisher::Publisher(char* address, char* port) {
    streamToRemote = new boost::asio::ip::tcp::iostream(address, port);
    archive = new portable_binary_oarchive(*streamToRemote);
    messages = new NotifyBuffer<keyframeMsg*>(30);

    writeThread = boost::thread(&Publisher::loop, this);

    /* boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[1], "daytime");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);
*/

}

Publisher::~Publisher() {
   delete streamToRemote;
   delete archive;
   delete messages;
   writeThread.join(); 
}

void Publisher::publish(keyframeMsg* fMsg) {
  
    LOGI("Debuggin: addng frame packet to queue.\n");
    if (messages->pushBack(fMsg)) {
        notifyCondition.notify_one();
    } else {
        delete fMsg;
    }
}

void Publisher::loop() {
    while (true) {
        //what is notifyCondition - its a boost primitive. Whis obvoiusly waits for an image to be captured and ready 
		boost::unique_lock<boost::recursive_mutex> waitLock(messages->getMutex());
    
		if ((messages->size() == 0)) {
			notifyCondition.wait(waitLock);
		}
		waitLock.unlock();
	    keyframeMsg* fMsg = messages->first();
		messages->popFront();
	
        LOGI("Debugging: sending frame packet with id %d\n", fMsg->id);

        *archive << *fMsg;
        if (fMsg->isKeyframe) {
            LOGI("Debugging: sending pointcloud with %d elements\n", fMsg->pointcloud.size());
            *archive << fMsg->pointcloud;
            LOGI("Debugging: Not Sent?");

        }
        delete fMsg;
   }
}


}

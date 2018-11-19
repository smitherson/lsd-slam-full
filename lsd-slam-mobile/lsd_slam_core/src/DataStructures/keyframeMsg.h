#pragma once

#include <vector>
#include <memory>

#include "sophus/sim3.hpp"
#include "util/SophusUtil.h"
#include "DataStructures/CommonStructures.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>

namespace lsd_slam
{

class keyframeMsg
{

    friend class boost::serialization::access;

public:
	typedef std::shared_ptr<keyframeMsg> Ptr;
	static Ptr Create()
	{
		return Ptr(new keyframeMsg());
	}

	double
		time;
	int
		id;
	unsigned int
		width,
		height;
	bool
		isKeyframe;
	float
		fx,fy,
		cx,cy,
		scale;
    
    std::vector<InputPointDense>
		pointcloud;
   
	float camToWorld[7];

	keyframeMsg() :
		time(0),
		id(0),
		width(0),
		height(0),
		isKeyframe(false),
		fx(0),
		fy(0),
		cx(0),
		cy(0),
		scale(1)
	{};
   
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & time;
        ar & id;
        ar & width;
        ar & height;
        ar & isKeyframe;
        ar & fx;
        ar & fy;
        ar & cx;
        ar & cy;
        ar & scale;
        ar & camToWorld;
        //ar & pointcloud;
    }
};

typedef const keyframeMsg* keyframeMsgConstPtr;

}



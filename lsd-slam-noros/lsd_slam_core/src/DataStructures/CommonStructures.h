#pragma once

struct InputPointDense
{
	float idepth;
	float idepth_var;
	unsigned char color[4];
};

struct GraphConstraint
{
	int from;
	int to;
	float err;
};

struct GraphFramePose
{
	int id;
	float camToWorld[7];
};

namespace boost {
    namespace serialization {

        template<class Archive>
            void serialize(Archive & ar, InputPointDense & p, const unsigned int version)
            {
                ar & p.idepth;
                ar & p.idepth_var;
                ar & p.color;
            }

    } // namespace serialization
} // namespace boost


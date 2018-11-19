cmake_minimum_required(VERSION 2.8.7)
project(lsd_slam_core)
#include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries

# rosbuild_init() 

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(SuiteParse REQUIRED) # Apparently needed by g2o

find_package(X11 REQUIRED)
find_package(OpenCV REQUIRED)

find_package(OpenGL REQUIRED)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTXML TRUE)
find_package(QGLViewer REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${QT_INCLUDES} ${EIGEN3_INCLUDE_DIR} ${QGLVIEWER_INCLUDE_DIR})


find_package(Boost COMPONENTS thread date_time program_options filesystem system serialization REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})


# FabMap
# uncomment this part to enable fabmap
#add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/openFabMap)
#include_directories(${PROJECT_SOURCE_DIR}/thirdparty/openFabMap/include)
#add_definitions("-DHAVE_FABMAP")
#set(FABMAP_LIB openFABMAP )

# Dynamic Reconfigure Services
# rosbuild_find_ros_package(dynamic_reconfigure)
# include(${dynamic_reconfigure_PACKAGE_PATH}/cmake/cfgbuild.cmake)
#gencfg()

# SSE flags
#rosbuild_check_for_sse()
#add_definitions("-DUSE_ROS")
add_definitions("-DENABLE_SSE")

# Also add some useful compiler flag
set(CMAKE_CXX_FLAGS
   "${CMAKE_CXX_FLAGS} ${SSE_FLAGS}  -msse3 -mfpmath=sse -pthread  -march=native  -std=c++0x -O3  -DNDEBUG"
) 

# Set source files
set(lsd_SOURCE_FILES
  ${PROJECT_SOURCE_DIR}/src/DataStructures/Frame.cpp
  ${PROJECT_SOURCE_DIR}/src/DataStructures/FramePoseStruct.cpp
  ${PROJECT_SOURCE_DIR}/src/DataStructures/FrameMemory.cpp
  ${PROJECT_SOURCE_DIR}/src/SlamSystem.cpp
  ${PROJECT_SOURCE_DIR}/src/LiveSLAMWrapper.cpp
  ${PROJECT_SOURCE_DIR}/src/DepthEstimation/DepthMap.cpp
  ${PROJECT_SOURCE_DIR}/src/DepthEstimation/DepthMapPixelHypothesis.cpp
  ${PROJECT_SOURCE_DIR}/src/util/globalFuncs.cpp
  ${PROJECT_SOURCE_DIR}/src/util/SophusUtil.cpp
  ${PROJECT_SOURCE_DIR}/src/util/settings.cpp
  ${PROJECT_SOURCE_DIR}/src/util/Undistorter.cpp
  ${PROJECT_SOURCE_DIR}/src/util/Publisher.cpp
  ${PROJECT_SOURCE_DIR}/src/Tracking/Sim3Tracker.cpp
  ${PROJECT_SOURCE_DIR}/src/Tracking/Relocalizer.cpp
  ${PROJECT_SOURCE_DIR}/src/Tracking/SE3Tracker.cpp
  ${PROJECT_SOURCE_DIR}/src/Tracking/TrackingReference.cpp
  ${PROJECT_SOURCE_DIR}/src/IOWrapper/Timestamp.cpp
  ${PROJECT_SOURCE_DIR}/src/GlobalMapping/FabMap.cpp
  ${PROJECT_SOURCE_DIR}/src/GlobalMapping/KeyFrameGraph.cpp
  ${PROJECT_SOURCE_DIR}/src/GlobalMapping/g2oTypeSim3Sophus.cpp
  ${PROJECT_SOURCE_DIR}/src/GlobalMapping/TrackableKeyFrameSearch.cpp
  ${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/Viewer/KeyFrameDisplay.cpp
  ${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/Viewer/KeyFrameGraphDisplay.cpp
  ${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/Viewer/PointCloudViewer.cpp
  ${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/Viewer/settings.cpp
)
set(SOURCE_FILES
  ${lsd_SOURCE_FILES}
  ${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/OpenCVImageStreamThread.cpp
  ${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/OpenCVOutput3DWrapper.cpp
  ${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCV/ImageDisplay_OpenCV.cpp
  ${PROJECT_SOURCE_DIR}/src/IOWrapper/NetworkOutput/NetworkOutput3DWrapper.cpp
)

include_directories(
  ${EIGEN3_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/thirdparty/Sophus
  ${CSPARSE_INCLUDE_DIR} #Has been set by SuiteParse
  ${CHOLMOD_INCLUDE_DIR} #Has been set by SuiteParse
)

link_directories( /usr/lib/x86_64-linux-gnu  )

message(" csparse at ${CSPARSE_LIBRARY} ")

# build shared library.

add_library(lsdslam SHARED ${SOURCE_FILES})
target_link_libraries(lsdslam ${Boost_LIBRARIES} ${FABMAP_LIB} ${OpenCV_LIBS} ${QGLViewer_LIBRARIES} ${QT_LIBRARIES} ${QGLVIEWER_LIBRARY} GL glut GLU g2o_core g2o_stuff cxsparse g2o_solver_csparse g2o_csparse_extension g2o_types_sim3 g2o_types_sba X11)
#link_boost(lsdslam thread)

# build live ros node
add_executable(live_slam src/main_live_odometry.cpp)
target_link_libraries(live_slam lsdslam ${Boost_LIBRARIES} )

# build image node
#add_executable(dataset_slam src/main_on_images.cpp)
#target_link_libraries(dataset_slam lsdslam ${Boost_LIBRARIES})

#add_executable(viewer src/main_viewer.cpp ${SOURCE_FILES} ${HEADER_FILES})
#target_link_libraries(viewer )




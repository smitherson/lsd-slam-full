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
set(CSPARSE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/SuiteSparse/CSparse/Include )
set(CHOLMOD_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/SuiteSparse/CHOLMOD/Include )
#message(${CSPARSE_INCLUDE_DIR})


#find_package(X11 REQUIRED)
#find_package(OpenCV REQUIRED)

include_directories(/home/smith/Work/android-ndk/OpenCV-android-sdk/sdk/native/jni/include)
#find_package(OpenGL REQUIRED)
#set(QT_USE_QTOPENGL TRUE)
#set(QT_USE_QTXML TRUE)
#find_package(QGLViewer REQUIRED)
#find_package(Eigen3 REQUIRED)
set(EIGEN3_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/Eigen)
include_directories(${QT_INCLUDES} ${EIGEN3_INCLUDE_DIR} ${QGLVIEWER_INCLUDE_DIR})

set(ANDROID_NDK /home/smith/Work/android-ndk/android-ndk-r10d)
set(SYSROOT   ${ANDROID_NDK}/platforms/android-21/arch-arm)
set(NDK_BOOST "/home/smith/Work/android-ndk/crystax-ndk-10.3.1")

#message ("NDK dir: " ${NDK})
set(BOOST_ROOT ${NDK_BOOST}/sources/boost/1.59.0/)
set(BOOST_INCLUDEDIR ${NDK_BOOST}/sources/boost/1.59.0/include )
#message("cmake find root path: " ${CMAKE_FIND_ROOT_PATH})
include_directories( ${BOOST_INCLUDEDIR}
${PROJECT_SOURCE_DIR}/thirdparty/g2o 
)
#find_package(Boost COMPONENTS thread date_time program_options filesystem system serialization REQUIRED)
#    include_directories(${Boost_INCLUDE_DIRS})

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
add_definitions("-DENABLE_NEON")

# Also add some useful compiler flag
set(CMAKE_CXX_FLAGS
   "${CMAKE_CXX_FLAGS} ${SSE_FLAGS}  -pthread -pie -march=armv7-a -mfpu=neon -std=c++0x -O3  -DNDEBUG --sysroot=${SYSROOT} "
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
  ${PROJECT_SOURCE_DIR}/src/boost_utils/portable_binary_oarchive.cpp
  #${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/Viewer/KeyFrameDisplay.cpp
  #${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/Viewer/KeyFrameGraphDisplay.cpp
  #${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/Viewer/PointCloudViewer.cpp
  #${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/Viewer/settings.cpp
)
set(SOURCE_FILES
  ${lsd_SOURCE_FILES}
  ${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/OpenCVImageStreamThread.cpp
  #${PROJECT_SOURCE_DIR}/src/IOWrapper/OpenCVIO/OpenCVOutput3DWrapper.cpp
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

# build shared library.
set (Boost_LIBRARIES boost_system boost_thread boost_date_time boost_program_options boost_filesystem boost_serialization )
set (OpenCV_LIBS opencv_calib3d
opencv_core
opencv_features2d
opencv_flann
opencv_highgui
opencv_imgcodecs
opencv_imgproc
opencv_java3.so
opencv_ml
opencv_objdetect
opencv_photo
opencv_shape
opencv_stitching
opencv_superres
opencv_ts
opencv_video
opencv_videoio
opencv_videostab
IlmImf  libjasper  libjpeg  libpng  libtiff  libwebp  tbb z log 
)
link_directories(
/home/smith/Work/android-ndk/crystax-ndk-10.3.1/sources/crystax/libs/${ARCH}
/home/smith/Work/android-ndk/OpenCV-android-sdk/sdk/native/3rdparty/libs/${ARCH} 
/home/smith/Work/android-ndk/OpenCV-android-sdk/sdk/native/libs/${ARCH}
/home/smith/Work/android-ndk/crystax-ndk-10.3.1/sources/boost/1.59.0/libs/${ARCH}/gnu-4.9/
${PROJECT_SOURCE_DIR}/thirdparty/g2o/lib/
${PROJECT_SOURCE_DIR}/thirdparty/SuiteSparse/CSparse/Lib/
${PROJECT_SOURCE_DIR}/thirdparty/SuiteSparse/CXSparse/Lib/
/home/smith/Work/android-ndk/android-ndk-r10d/platforms/android-21/arch-arm/usr/lib
)


get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "include dir='${dir}'")
endforeach()

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "link dir='${dir}'")
endforeach()

foreach(dir ${Boost_LIBRARIES})
  message(STATUS "${dir}")
endforeach()



add_library(lsdslam SHARED ${SOURCE_FILES})
target_link_libraries(lsdslam ${Boost_LIBRARIES} ${FABMAP_LIB} ${OpenCV_LIBS} ${QGLViewer_LIBRARIES} ${QT_LIBRARIES} ${QGLVIEWER_LIBRARY} g2o_core g2o_stuff csparse cxsparse g2o_solver_csparse g2o_csparse_extension g2o_types_sim3 g2o_types_sba m)
# GL glut GLU
#link_boost(lsdslam thread)

# build live ros node
#add_executable(live_slam src/main_live_odometry.cpp)
#target_link_libraries(live_slam lsdslam ${Boost_LIBRARIES} )

# build image node
#add_executable(dataset_slam src/main_on_images.cpp)
#target_link_libraries(dataset_slam lsdslam ${Boost_LIBRARIES})

#add_executable(viewer src/main_viewer.cpp ${SOURCE_FILES} ${HEADER_FILES})
#target_link_libraries(viewer )




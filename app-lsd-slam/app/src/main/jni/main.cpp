/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//BEGIN_INCLUDE(all)
#include <initializer_list>
#include <jni.h>
#include <errno.h>
#include <cassert>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>

#include "LiveSLAMWrapper.h"

#include <boost/thread.hpp>
#include "SlamSystem.h"
#include "IOWrapper/InputImageStream.h"

#include "IOWrapper/OpenCVIO/OpenCVImageStreamThread.h"
#include "IOWrapper/NetworkOutput/NetworkOutput3DWrapper.h"

using namespace lsd_slam;
using namespace cv;


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

InputImageStream* inputStream;
OpenCVImageStreamThread* cvStream;
LiveSLAMWrapper* ptrSlamNode = NULL;


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    LOGI("LIBRARY LOADED!!!");


    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
Java_com_example_native_1activity_NativeLoader_startSLAM(JNIEnv *env, jobject instance) {
    LOGI("STARTING LSD SLAM!!!");

    char* pathToStream = (char *) "0";

    inputStream = new OpenCVImageStreamThread(pathToStream);
    cvStream = (OpenCVImageStreamThread*)inputStream;

    /*std::string calibParams("834.42 834.42 319.5 239.5 0.444256 -3.41396 0 0\n"
                                    "320 240\n"
                                    "crop\n"
                                    "320 240");
    */

    std::string calibParams("710.74 710.74 159.5 119.5 1.07904 -20.1868 0 0\n"
                                   "320 240\n"
                                   "crop\n"
                                   "320 240");

    inputStream->setCalibrationFromString(calibParams);
    inputStream->run();
    printf("Staring the outpud thread!\n"); fflush(stdout);
    //Output3DWrapper* outputWrapper =new NetworkOutput3DWrapper(inputStream->width(), inputStream->height(), "192.168.88.187","6660");
    //Output3DWrapper* outputWrapper =new OpenCVOutput3DWrapper(inputStream->width(), inputStream->height());
    Output3DWrapper* outputWrapper = NULL;
    printf("Staring the slamNode!\n"); fflush(stdout);
    LiveSLAMWrapper slamNode(inputStream, outputWrapper);
    ptrSlamNode = &slamNode;
    slamNode.Loop();


    if (inputStream != nullptr)
        delete inputStream;
    if (outputWrapper != nullptr)
        delete outputWrapper;
        return ;
}

//END_INCLUDE(all)
JNIEXPORT jboolean JNICALL
Java_com_example_native_1activity_NativeLoader_pushImage(JNIEnv *env, jobject instance, jint width,
                                                         jint height, jbyteArray YUVFrameData_) {


    jbyte *pYUVFrameData = env->GetByteArrayElements(YUVFrameData_, 0);

    Mat mNV(height + height/2, width , CV_8U, (unsigned char*)pYUVFrameData);


    Mat mBgr(height, width, CV_8UC3);

    cv::cvtColor(mNV, mBgr, CV_YUV2BGRA_IYUV);


    if (cvStream) {
        cvStream->vidCb(mBgr);
    }
    env->ReleaseByteArrayElements(YUVFrameData_, pYUVFrameData, 0);

    return (jboolean) true;
}


JNIEXPORT jboolean JNICALL
Java_com_example_native_1activity_NativeLoader_getImage(JNIEnv *env, jobject instance,
                                                        jintArray frameData_) {

    jint *frameData = env->GetIntArrayElements( frameData_, NULL);

    if (!ptrSlamNode) return (jboolean) false;
    Mat* img = (ptrSlamNode->getSlamSystem()->getDebugImageDepth());

    Mat tmp;
    Mat mbgra(img->rows, img->cols, CV_8UC4, (unsigned char *)frameData);

    ptrSlamNode->getSlamSystem()->debugImageDepthLock();
    img->copyTo(tmp);
    ptrSlamNode->getSlamSystem()->debugImageDepthUnlock();
    cvtColor(tmp, mbgra, CV_BGR2BGRA);

    env->ReleaseIntArrayElements( frameData_, frameData, 0);

    mbgra.release();
    return (jboolean) true;
}

JNIEXPORT jboolean JNICALL
Java_com_example_native_1activity_NativeLoader_convertImage(JNIEnv *env, jobject instance, jint width,
                                                         jint height, jbyteArray YUVFrameData_, jintArray frameData_) {


    jbyte *pYUVFrameData = env->GetByteArrayElements(YUVFrameData_, 0);
    jint *frameData = env->GetIntArrayElements(frameData_, 0);

    Mat mNV(height + height/2, width , CV_8U, (unsigned char*)pYUVFrameData);

    Mat mBgr(width, height, CV_8UC4, (unsigned char *)frameData);

    cvtColor(mNV, mBgr, CV_YUV2BGRA_IYUV);

    env->ReleaseByteArrayElements(YUVFrameData_, pYUVFrameData, 0);
    env->ReleaseIntArrayElements( frameData_, frameData, 0);

    return (jboolean) true;
}
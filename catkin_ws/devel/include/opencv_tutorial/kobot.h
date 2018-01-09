#ifndef __KOBOT_H__
#define __KOBOT_H__

#pragma warning(disable:4819)

#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>

#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

#include "std_msgs/Int32.h"
#include "std_msgs/String.h"
#include "opencv_tutorial/msgTutorial.h"

extern cv::VideoCapture capture;
extern cv::Mat frame;

extern pthread_mutex_t frameLocker;
extern pthread_t UpdThread;

void *UpdateFrame(void* arg)
{
  while (1)
  {
    cv::Mat tempframe;
    capture >> tempframe;
    pthread_mutex_lock(&frameLocker);
    frame = tempframe;
    pthread_mutex_unlock(&frameLocker);
  }
  pthread_exit((void *)0);
}

#endif

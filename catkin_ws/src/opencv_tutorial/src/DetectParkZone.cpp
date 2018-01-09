#include "opencv_tutorial/kobot.h"

cv::VideoCapture capture;
cv::Mat frame;

pthread_mutex_t frameLocker;
pthread_t UpdThread;

int GetAngleABC (cv::Point a, cv::Point b, cv::Point c)
{
  	cv::Point ab(b.x - a.x, b.y - a.y);
	cv::Point cb(b.x - c.x, b.y - c.y);

	float dot = ( ab.x * cb.x + ab.y * cb.y );
	float cross = ( ab.x * cb.y - ab.y * cb.x );

	float alpha = atan2(cross, dot);

	return (int)floor(alpha * 180.0 / CV_PI + 0.5);
}

int main (int argc, char** argv)
{
	ros::init(argc, argv, "ros_parkzone_msg_publisher"); // 노드명 초기화
	ros::NodeHandle nh;

	ros::Publisher ros_ParkZone_pub = nh.advertise<std_msgs::Int32>("ros_parkzone_msg", 200);
	ros::Rate loop_rate(10); // 루프 주기를 설정한다.

	capture.open(0);

	capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

	// mutex 및 쓰레드 초기화
	pthread_mutex_init(&frameLocker, NULL);
	pthread_create(&UpdThread, NULL, UpdateFrame, NULL);

	while (ros::ok())
	{

		std_msgs::Int32 msg3;

		cv::Mat currentFrame, frame_gray, frame_result;

		pthread_mutex_lock(&frameLocker);
		currentFrame = frame;
		pthread_mutex_unlock(&frameLocker);


		if(!currentFrame.empty()){
    		cvtColor(currentFrame, frame_gray, CV_BGR2GRAY);
		}

	    //threshold (입력 이미지, 결과 이미지, threshold 값, maxval : 입력 이미지의 픽셀 값이 threshold보다 클 경우 , 작을 경우 : 0)
		cv::threshold(frame_gray, frame_gray, 15, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	    std::vector<std::vector<cv::Point> > contours;
		//findContours(입력 이미지, 외곽선 벡터(배열), hierarchy, 윤곽 검색 모드, 윤곽 근사 방법, offset)
	    cv::findContours(frame_gray, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	    std::vector<cv::Point2f> approx;
	    frame_result = currentFrame.clone();
		
		int rect_count=0;

   		for (size_t i = 0; i < contours.size(); i++)
   		{
			//approxPolyDP : 해당 contour가 (몇%까지의) 최대 거리를 판단 : 값이 작을 수록 자세하게 표현함
   			cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

      	  	//면적이 일정 크기 이상인지 판단함
			//fabs : 절대값 변환 함수
        	if (std::fabs (cv::contourArea(cv::Mat(approx))) > 100)
        	{

				int size = approx.size();

				if ( size == 4)
				{
		    	    //line (최종 영상, 첫 번째 꼭짓점, 마지막 꼭짓점, 색, 굵기)
		    	    cv::line (frame_result, approx[0], approx[approx.size() -1], cv::Scalar(0, 255, 0), 3);
	
		    	    for (int k = 0 ; k < size - 1; k++)
		   		    {
		   	     		cv::line (frame_result, approx[k], approx[k+1], cv::Scalar(0,255,0),3);
		        	}

		        	for (int k = 0; k < size ; k++)
		        	{
		        		cv::circle (frame_result, approx[k], 3, cv::Scalar(0,0,255));
		        	}
			
		     		//모든 코너의 각도를 구함
		     		std::vector<int> angle;

		    		for (int k = 0 ; k < size ; k++) {
		  				int ang = GetAngleABC(approx[k], approx[(k+1) % size], approx[(k+2) % size]);

		    			angle.push_back(ang);
					}

					sort(angle.begin(), angle.end());

	  				int minAngle = angle.front();
	  				int maxAngle = angle.back();
	  				int threshold = 10;

					// Count Rectangle
	 				if (size == 4 && minAngle <= (-90) + threshold && maxAngle >= (-90) - threshold)
	 				{
	 					rect_count++;
	 				}
				}
				else
				{
					continue;
				}

        	}

    	}
		msg3.data = rect_count;
        ROS_INFO("Rectangle Count : %d", msg3.data); // if Rectangle Count is 2 , there is Park Zone
        ros_ParkZone_pub.publish(msg3);
		
       	ros::spinOnce();
       	loop_rate.sleep();

    	if (!frame_result.empty())
    	{
      		cv::imshow("Result", frame_result);
    	}

    	if (cv::waitKey(10) == 27)	break;
		
	}

	return 0;
}

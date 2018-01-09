#include "opencv_tutorial/kobot.h"

cv::VideoCapture capture;
cv::Mat frame;

pthread_mutex_t frameLocker;
pthread_t UpdThread;

int main(int argc, char** argv)
{

	//  ——————————— ROS 초기화 ————————————— //   
	ros::init(argc, argv, "ros_tutorial_msg_publisher"); // 노드명 초기화
	ros::NodeHandle nh;

	ros::Publisher ros_Intercept_pub = nh.advertise<std_msgs::Int32>("ros_Intercept_msg", 200);
	ros::Rate loop_rate(10); // 루프 주기를 설정한다. 

	// 0번(하단) 카메라로 설정
	capture.open(2);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

	// mutex 및 쓰레드 초기화
	pthread_mutex_init(&frameLocker, NULL);
	pthread_create(&UpdThread, NULL, UpdateFrame, NULL);
	
	// 관심영역은 전체영상 320x240 중 왼쪽 절반인 160x240으로 설정
	cv::Rect roi(0, 0, 200, 240);
	cv::Mat gray, edge; 
	
	while(ros::ok())
	{
		// while문 내에서 쓰일 프레임을 보호하기 위하여 currentFrame을 따로 만들어 mutex내에서 받아온다.
		cv::Mat currentFrame;
		
		pthread_mutex_lock(&frameLocker);
		sleep(0.3);
		currentFrame = frame;
		pthread_mutex_unlock(&frameLocker);
		
		// 관심영역 부분만 알고리즘을 적용시킨다. 
		cv::Mat roiImage;
		if(!currentFrame.empty()){

			roiImage = currentFrame(roi);			
			cvtColor(roiImage, gray, CV_RGB2GRAY);			
		}


		Canny(gray, edge, 30, 90, 3);

		std::vector<cv::Vec4i> lines;  // 영상 내 직선들의 집합을 받아오기 위한 Vector
		HoughLinesP(edge, lines, 1, CV_PI / 180, 30, 30, 3);

		// 각 직선들의 기울기를 구하고 관심영역에 접하는 직선만 검출해 평균 기울기를 구한다.
		int count = 0;
		float avr_gradient = 0; 
		float gradient=0; 

		for(int i = 0; i < lines.size(); i++){
			cv::Vec4i L = lines[i];

			if((L[2] - L[0]) != 0){
				gradient = (float)(L[3] - L[1]) / (float)(L[2] - L[0]);
			}
			else{
				gradient = 99;
			}

			//if(gradient > -1.5 && gradient < 1.5)
			imshow("Gray" , gray);
			if(gradient!=0 && L[2] == 199 && ( gray.at<uchar>(L[1], L[0]) > 240 || gray.at<uchar>(L[3], L[2]) > 240) )			
			{
				line(currentFrame, cv::Point(L[0], L[1]), cv::Point(L[2], L[3]), cv::Scalar(0, 0, 255), 2);
				circle(currentFrame, cv::Point(L[0], L[1]), 3, cv::Scalar(0,255,0), 2,3);
				circle(currentFrame, cv::Point(L[2], L[3]), 3, cv::Scalar(0,255,0), 2,3);
				count++;			
				avr_gradient += gradient;
			}
		}

		// 위에서 구한 평균 기울기를 갖고 (320,120)을 지나는 직선을 구해 x좌표가 160인 중앙선과의 교점을 구한다. 
		int y_value;
		if(count != 0){

			avr_gradient = avr_gradient/count;
			y_value = (int)(120 - (160 * avr_gradient));

			if(y_value >240){
				line(currentFrame, cv::Point(320,120), cv::Point(160, 240), cv::Scalar(255,0,0),2);
			}
			else if(y_value < 0){
				line(currentFrame, cv::Point(320,120), cv::Point(160, 0), cv::Scalar(255,0,0),2);	
			}
			else{			
				line(currentFrame, cv::Point(320,120), cv::Point(160, y_value), cv::Scalar(255,0,0),2);
				std::cout << y_value << std::endl;
			}
		}
		
		if(!currentFrame.empty()){
			imshow("LineTracing", currentFrame);
		}
		
		// x=159 (중앙선)에서의 y절편을 구하여 메시지로 전달하고, 이 y값을 이용하여 터틀봇의 주행방향을 결정한다.
		std_msgs::Int32 msg; // ROS 메시지에 사용할 변수 선언
		msg.data = y_value;
		ROS_INFO("y value : %d", msg.data);
		ros_Intercept_pub.publish(msg);

		ros::spinOnce();	
		loop_rate.sleep();

		if(cvWaitKey(10) == 27) break;
		
		
	}
	return 0;
}




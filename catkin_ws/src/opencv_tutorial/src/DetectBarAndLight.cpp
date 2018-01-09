#include "opencv_tutorial/kobot.h"

cv::VideoCapture capture;
cv::Mat frame;

pthread_mutex_t frameLocker;
pthread_t UpdThread;

cv::Mat Binaryframe, small_Binaryframe;

//신호등은 미션 당 한 번 등장하므로 algorithm_flag를 두어 한 번만 실행하게 한다.
bool algorithm_flag = false;
bool green_flag = false, red_flag = false, yellow_flag = false;

//영상에서 빨강, 초록, 노랑의 색을 이진화 시키는 함수 
void BinaryColors(cv:: Mat &in_frame, cv::Mat &out_frame, int colors[], int colorsTotalNum){

	cv::Mat Temp, temp, temp2, F_red, F_green, F_yellow, sum, x;
	cv::Mat a, b;

	cv::Mat small(2, 2, CV_8U, cv::Scalar(1));
	cv::Mat element(4, 4, CV_8U, cv::Scalar(1));
	cv::Vec3b red = cv::Vec3b(0, 0, 255);
	cv::Vec3b yellow = cv::Vec3b(0, 255, 255);
	cv::Vec3b green = cv::Vec3b(0, 255, 0);

	//빛의 영향은 불가피하므로 HSV로 바꿈
	cvtColor(in_frame, Temp, CV_BGR2HSV);
	cvtColor(in_frame, x, CV_BGR2HSV);

	//영상 내 원하는 색의 이진화를 시킴
	for (int i = 0; i < colorsTotalNum ; i++){
		switch (colors[i]){
			
			//빨간색의 이진화
			case 1 : 
				inRange(Temp, cv::Scalar(0, 0, 0), cv::Scalar(10, 255, 255), temp);
				inRange(Temp, cv::Scalar(170, 0, 0), cv::Scalar(180, 255, 255), temp2);
	
				a = x.clone(); b = x.clone();
	
				a.setTo(0); b.setTo(0);
	
				a.setTo(red, temp);
				b.setTo(red, temp2);

				F_red = a | b;
	
				//침식 후 팽창 - 잡음 제거
				cv::erode(F_red, F_red, small);
				cv::dilate(F_red, F_red, cv::Mat());

				sum = F_red.clone();
			break;

			// 초록색의 이진화 
			case 2 : 
				cvtColor(frame, F_green, CV_BGR2YCrCb) ; 
		
				inRange(F_green, cv::Scalar(1,0,0), cv::Scalar(160,119,119),temp )  ;
				inRange(F_green, cv::Scalar(50,0,0), cv::Scalar(250,115,115),temp2 )  ;
	
				temp = temp | temp2 ;
					
				a = x.clone();
				a.setTo(0);
				a.setTo(green, temp);

				F_green= a.clone();
			
				cv::erode(F_green,F_green, small) ; // default 3x3
				cv::dilate(F_green,F_green , element) ; // element
				cv::dilate(F_green, F_green, element) ;

				sum = F_green.clone();	
			break;

			//노란색의 이진화
			case 3 : 
				inRange(Temp, cv::Scalar(25,10,180), cv::Scalar(32,255,255),temp) ;
				a = x.clone();
				a.setTo(0);
				a.setTo(yellow, temp);

				F_yellow = a.clone();

				cv::erode(F_yellow, F_yellow, small);
				cv::dilate(F_yellow, F_yellow, cv::Mat());

				sum = F_yellow.clone();
			break;
		}
		//Binaryframe에는 빨간색, 초록색, 노란색이 이진화된 한 영상으로 합쳐짐.
		if (i == 0) Binaryframe = sum;
		else Binaryframe = sum | Binaryframe;
	}
	out_frame = Binaryframe.clone();
	cv::resize(out_frame, out_frame, cv::Size(320, 240), 0, 0, CV_INTER_NN);
}


//영상 내 B, G, R의 값을 얻는 함수
void GetColor(bool is_color, cv::Mat image, int j, int i, int* B, int* G = 0, int* R = 0)
{
	uchar *data = image.data;
	uchar *red, *blue, *green;

	int nRow = image.rows;
	int nCol = image.cols;

	if (i > nCol - 1 || j > nRow - 1)
	{
		return;
	}
	
	blue = image.data + j*image.step + i*image.elemSize() + 0;

	if (is_color)
	{
		green = image.data + j*image.step + i*image.elemSize() + 1;
		red = image.data + j*image.step + i*image.elemSize() + 2;
	}

	*B = (int)*blue;

	if (is_color)
	{
		*G = (int)*green;
		*R = (int)*red;
	}
}

//차단바 내의 빨간색 픽셀의 개수를 구하는 함수
void get_redframe_pixel(cv::Mat &frame,int input_up, int input_down, int input_left, int input_right, int input_nMaskW, int input_nMaskH, int* r1, int* r2, int* r3, int* r4, int* r5, int* r6, int* r7, int* r8)
{
	//마스크를 사용하여 빨간색이 응집되어 있는 경우에만 마스크의 개수를 센다. 
	int nMaskW = input_nMaskW, nMaskH = input_nMaskH;
	int hW = nMaskW / 2, hH = nMaskH / 2;
	
	int up = input_up + hH, down = input_down + hH, left = input_left + hW, right = input_right + hW;

	int Red_count = 0, Yellow_count = 0, Green_count = 0;
	int Red_count1 = 0, Red_count2 = 0, Red_count3 = 0, Red_count4 = 0, Red_count5 = 0, Red_count6 = 0, Red_count7 = 0, Red_count8 = 0;

	int R, G, B;	

	int y, x;
	for (y = up - hH ; y < down - hH ; y++)
	{
		for ( x = left - hW ; x < right - hW ; x++)
		{
			int red_maskCnt = 0, yellow_maskCnt = 0, green_maskCnt = 0;
			int r,c;
			for ( r = 0 ; r < nMaskH ; r++ )
			{
				for ( c = 0 ; c < nMaskW ; c++ )
				{
					GetColor(true, frame, r - hH + y, c - hW + x, &B, &G, &R);
					
					//GetColor를 통해 얻어온 B,G,R 값 내에서 R이 255인 경우(빨간색인 경우)에만 red_maskCnt + 1
					if (R==255)
					{
						red_maskCnt++;
					}
				}
			}
			
			//차단바의 위치상 오른쪽에 치우쳐져 위치 - 16칸의 영역으로 나누어 7번째 ~ 15번째의 영역의 redpixel 개수를 센다.
			//마스크 내 픽셀의 개수 중 80% 이상이 빨간색인 경우 해당 마스크가 빨간색이라고 판단.
			if ((float)red_maskCnt / ((float)nMaskH * (float)nMaskW) > 0.8 )
			{
				if ((x >= left-hW + (float(right-left))*(0.4375)) && (x< left-hW + (float(right-left))*(0.5) ) )
					{ Red_count1++; }
				else if ((x >= left-hW + (float(right-left))*(0.5)) && (x< left-hW + (float(right-left))*(0.5625) ) )
					{ Red_count2++; }
				else if ((x >= left-hW + (float(right-left))*(0.5625)) && (x< left-hW + (float(right-left))*(0.625) ) )
					{ Red_count3++; }
				else if ((x >= left-hW + (float(right-left))*(0.625)) && (x< left-hW + (float(right-left))*(0.6875) ) )
					{ Red_count4++; }
				else if ((x >= left-hW + (float(right-left))*(0.6875)) && (x< left-hW + (float(right-left))*(0.75) ) )
					{ Red_count5++; }
				else if ((x >= left-hW + (float(right-left))*(0.75)) && (x< left-hW + (float(right-left))*(0.8125) ) )
					{ Red_count5++; }
				else if ((x >= left-hW + (float(right-left))*(0.8125)) && (x< left-hW + (float(right-left))*(0.875) ) )
					{ Red_count7++; }
				else if ((x >= left-hW + (float(right-left))*(0.875)) && (x< left-hW + (float(right-left))*(0.9375) ) )
					{ Red_count8++; }
				
			}
		}
	}
	*r1 = Red_count1;
	*r2 = Red_count2;
	*r3 = Red_count3;
	*r4 = Red_count4;
	*r5 = Red_count5;
	*r6 = Red_count6;
	*r7 = Red_count7;
	*r8 = Red_count8;
}

//신호등 내의 빨강색, 노랑색, 초록색 개수를 세는 함수
void get_color_pixel(cv::Mat &frame,int input_up, int input_down, int input_left, int input_right, int input_nMaskW, int input_nMaskH, int *redCnt, int *yellowCnt, int *greenCnt)
{
	int nMaskW = input_nMaskW, nMaskH = input_nMaskH;
	int hW = nMaskW / 2, hH = nMaskH / 2;
	
	int up = input_up + hH, down = input_down + hH, left = input_left + hW, right = input_right + hW;

	int Red_count = 0, Yellow_count = 0, Green_count = 0;

	int R, G, B;	

	int y, x;
	for (y = up - hH ; y < down - hH ; y++)
	{
		for ( x = left - hW ; x < right - hW ; x++)
		{
			int red_maskCnt = 0, yellow_maskCnt = 0, green_maskCnt = 0;
			int r,c;
			for ( r = 0 ; r < nMaskH ; r++ )
			{
				for ( c = 0 ; c < nMaskW ; c++ )
				{
					GetColor(true, frame, r - hH + y, c - hW + x, &B, &G, &R);
					if (G == 255 && R ==255)
					{	
						yellow_maskCnt++;
					}
					else if (G == 255) 
					{
						green_maskCnt++;
					}
					else if (R == 255)
					{
						red_maskCnt++;
					}
				}
			}
			if ((float)red_maskCnt / ((float)nMaskH * (float)nMaskW) > 0.8 )
			{
				Red_count++;	
			}

			if ((float)yellow_maskCnt / ((float)nMaskH * (float)nMaskW) > 0.8 )
			{
				Yellow_count++;
			}
			if ((float)green_maskCnt / ((float)nMaskH * (float)nMaskW) > 0.8 )
			{
				Green_count++;
			}
		}
	}
	*redCnt = Red_count;
	*yellowCnt = Yellow_count;
	*greenCnt = Green_count;
}

int main(int argc, char** argv)
{

	//  ——————————— ROS 초기화 ————————————— //   
	ros::init(argc, argv, "ros_bar_light_publisher"); // 노드명 초기화
	ros::NodeHandle nh;

	ros::Publisher ros_tutorial_pub = nh.advertise<std_msgs::Int32>("ros_bar_light_msg", 200);
	ros::Rate loop_rate(10); // 루프 주기를 설정한다. 

	// 0번(하단) 카메라로 설정
	capture.open(1);
	
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

	// mutex 및 쓰레드 초기화
	pthread_mutex_init(&frameLocker, NULL);
	pthread_create(&UpdThread, NULL, UpdateFrame, NULL);
	
	// 관심영역은 전체영상 320x240 중 왼쪽 절반인 160x240으로 설정
	//cv::Rect roi(0,70, 320, 170);
	cv::Mat gray, edge; 
	cv::Mat sumFrame;
	
	while(ros::ok())
	{
		// while문 내에서 쓰일 프레임을 보호하기 위하여 currentFrame을 따로 만들어 mutex내에서 받아온다.
		cv::Mat currentFrame, clonedFrame, clonedFrame2;
		
		pthread_mutex_lock(&frameLocker);
		sleep(0.3);
		currentFrame = frame;
		pthread_mutex_unlock(&frameLocker);
		
		
		//currentFrame이 준비된 경우에 빨간색, 노란색, 초록색만 띄운 이진화 영상을 띄운다. 
		if(!currentFrame.empty()){
			imshow("Current Frame", currentFrame);

			//원하는 색의 이진화 영상을 선택함 - 1 : red, 2 : green, 3 : yellow
			int colors[] = {1,2,3};
			int colorsTotalNum = 3;
			
			BinaryColors(currentFrame, sumFrame, colors, colorsTotalNum);

			imshow("RED Frame", sumFrame);

		}


		int r1, r2, r3, r4, r5, r6, r7, r8;
		int RedLight, YellowLight, GreenLight;
		
		// ROS 메시지에 사용할 변수 선언
		std_msgs::Int32 msg; 


		//차단바가 있는 위치인 화면 중앙에 관심 영역을 설정하여 빨간색의 pixel값을 얻는다. 
		get_redframe_pixel(sumFrame, 80, 160, 5, 315, 4, 4, &r1, &r2, &r3, &r4, &r5, &r6, &r7, &r8);
		//std::cout << "Redcount1 : " << r1 << " Redcount2 : " << r2 << " Redcount3 : " << r3 << " Redcount4 : " << r4 << std::endl;

		//차단바의 특성상 흰색-빨간색이 연속하여 반복됨을 이용, 빨간색이 교차하여 나타났을 때 신호등이라고 판단. 
		if ((r1 > 500 && r3 > 500 && r5 > 500) || (r2 > 500 && r4 > 500 && r6 > 500)) {
			msg.data = 5;
			//ROS_INFO("STOP");
			ros_tutorial_pub.publish(msg);
			
		}
		else {
			msg.data =0;
			ros_tutorial_pub.publish(msg);
			//ROS_INFO("signal : %d", msg.data);
		
		}

		//신호등 감지를 위한 빨간색, 노란색, 초록색 값 얻기
		get_color_pixel(sumFrame, 10, 235, 240, 310, 4, 4, &RedLight, &YellowLight, &GreenLight);
		std::cout << "RedLight : " << RedLight << " YellowLight : " << YellowLight << " GreenLight : " << GreenLight << std::endl;

	
		//신호등이 감지되지 않은 상태고, GreenLight가 일정 수준 이상 감지된 경우 초록색이 들어왔다는 신호를 줌
		if (algorithm_flag == false && GreenLight > 80 &&green_flag == false){
			green_flag = true;
			msg.data = 1;
			ros_tutorial_pub.publish(msg);
			std::cout << "PASS : GREEN LIGHT" << std::endl;
		}
		
		//신호등 중 초록색을 감지한 상태고, YelloLight가 일정 수준 이상 감지된 경우 노란색이 들어왔다는 신호를 줌
		if (YellowLight > 10 && green_flag == true && yellow_flag == false){
			yellow_flag = true;
			msg.data = 2;
			ros_tutorial_pub.publish(msg);
			std::cout << "DOWN : YELLOW LIGHT" << std::endl;
		}
		
		//신호등 중 초록색에 이어서 노란색이 감지된 상태고, RedLight이 감지된 경우 빨간색이 들어왔다는 신호를 줌
		if (GreenLight<20 && YellowLight <5 && yellow_flag == true && red_flag == false){
			red_flag = true;
			msg.data = 3;
			ros_tutorial_pub.publish(msg);
			std::cout << "STOP : RED LIGHT" << std::endl;
		}

		//신호등 중 초록색, 노란색, 빨간색이 감지된 상태고, 다시 초록색이 감지된 경우 차는 출발을 함. 이어서 신호등 알고리즘이 끝남.
		if (red_flag == true && GreenLight > 80){
			red_flag = false;
			green_flag = false;
			yellow_flag = false;
			algorithm_flag = true;
			msg.data = 4;
			ros_tutorial_pub.publish(msg);
			std::cout << "FINAL : GREEN LIGHT" << std::endl;
		}


		ros_tutorial_pub.publish(msg);

		ros::spinOnce();	
		loop_rate.sleep();

		// 키보드에서 a를 누르면 b를 누르기 전까지 모터가 멈추게 였다.
		if(cvWaitKey(10) == 27) break;

	}
	return 0;
}




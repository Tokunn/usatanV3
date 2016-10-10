#include <iostream>
#include <sstream>

#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include <atlbase.h>

#include "error_check.h"
#include "KinectApp.cpp"


void draw_depth(std::vector<UINT16> *p_depth, int height, int width) {
	if (p_depth != nullptr) {
		// Depthデータを表示する
		cv::Mat depthImage(height, width, CV_8UC1);

		// Depthデータを0-255のグレーデータにする
		for (int i = 0; i < depthImage.total(); ++i) {
			depthImage.data[i] = (*p_depth)[i] % 255;
		}
		cv::imshow("Depth", depthImage);
	}
}

void draw_2ddepth(std::vector<UINT16> *p_depth, int height, int width) {
	std::vector<UINT16> depth2dBuffer(width, 0);
	for (int i = 0; i < width; ++i) {
		depth2dBuffer[i] = (*p_depth)[width*int(height / 2) + i];
	}
	cv::Mat depth2dImage(width, 800, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < width; ++i) {
		depth2dImage.data[(i * 800) + depth2dBuffer[i] / 10] = 255;
	}
	cv::imshow("2D Depth", depth2dImage);
}


void main()
{
	try {
		KinectApp kinect;
		kinect.initialize();

		std::vector<UINT16> *p_depth = nullptr;

		while (1) {
			kinect.update();

			p_depth = kinect.get_depth();
			draw_depth(p_depth, kinect.depthHeight, kinect.depthWidth);
			draw_2ddepth(p_depth, kinect.depthHeight, kinect.depthWidth);

			auto key = cv::waitKey(10);
			if (key == 27) {
				break;
			}
		}
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
}
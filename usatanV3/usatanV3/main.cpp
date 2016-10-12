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
	cv::Mat depth2dImage(800, width, CV_8UC1, cv::Scalar(0));
	//cv::Mat depth2dImage(width, 800, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < width; ++i) {
		//depth2dImage.data[(i * 800) + depth2dBuffer[i] / 10] = 255;
		depth2dImage.data[i + (depth2dBuffer[i]/10 * width)] = 255;
	}
	cv::imshow("2D Depth", depth2dImage);
}

void draw_2ddepth_worldaxis(std::vector<UINT16> *p_depth, int height, int width) {
	std::vector<UINT16> depth2dBuffer(width, 0);
	for (int i = 0; i < width; ++i) {
		depth2dBuffer[i] = (*p_depth)[width*int(height / 2) + i];
	}
	cv::Mat depth2dImage(810, width, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < width; ++i) {
		depth2dImage.data[i + (depth2dBuffer[i]/10 * width)] = 255;
	}

	//cv::Mat depth2dWorldAxisImage(width, 800, CV_8UC1, cv::Scalar(0));
	cv::Mat depth2dWorldAxisImage(900, 1600, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < width; ++i) {
		double ratio = (double)i / (double)256 - 1;
		double max_x = depth2dBuffer[i] / 10 * 2.75;
		INT16 worldX = ratio * max_x * 0.3 + 800;
		//INT16 i_tmp = 512, z_tmp = 50;
		//worldX = (i_tmp - 256) / 256 * z_tmp * 2.75 * 0.3 + 800;
		//worldX = (i - 256) * 3 + 800;
		//worldX = (i - 256) / 256 * depth2dImage.data[i] * 2.75 + 800;
		depth2dWorldAxisImage.data[worldX + (depth2dBuffer[i] / 10 * 1600)] = 255;
	}
	cv::imshow("2D Depth World Axis", depth2dWorldAxisImage);

	/*for (int i = 0; i < width; ++i) {
		for (int j = 0; j < 800; ++j) {
			INT16 projectionX = 256 * 8000 / 2.75*(i - 256) / j * 256 + 256;
			// INT16 projectionX = i;
			INT16 projectionZ = j;
			depth2dWorldAxisImage.data[i*800 + j]
				= depth2dImage.data[projectionX*800+projectionZ];
		}
	}*/
}

void main() {
	try {
		KinectApp kinect;
		kinect.initialize();

		std::vector<UINT16> *p_depth = nullptr;

		while (1) {
			kinect.update();

			p_depth = kinect.get_depth();
			draw_depth(p_depth, kinect.depthHeight, kinect.depthWidth);
			draw_2ddepth(p_depth, kinect.depthHeight, kinect.depthWidth);
			draw_2ddepth_worldaxis(p_depth, kinect.depthHeight, kinect.depthWidth);

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
#include <iostream>
#include <sstream>

#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include <atlbase.h>

#include "error_check.h"
#include "KinectApp.cpp"

#include <math.h>

#define PI 3.141592


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

void draw_2ddepth_worldaxis_parce(std::vector<UINT16> *p_depth, int height, int width) {
	// 2次元距離情報を抽出
	std::vector<UINT16> depth2dBuffer(width, 0);
	for (int i = 0; i < width; ++i) {
		depth2dBuffer[i] = (*p_depth)[width*int(height / 2) + i];
	}
	// パースを考慮せずに上面図化
	cv::Mat depth2dImage(810, width, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < width; ++i) {
		depth2dImage.data[i + (depth2dBuffer[i]/10 * width)] = 255;
	}
	cv::imshow("2D Depth", depth2dImage);
}

void draw_2ddepth_worldaxis(std::vector<UINT16> *p_depth, int height, int width) {
	// 2次元距離情報を抽出
	std::vector<UINT16> depth2dBuffer(width, 0);
	for (int i = 0; i < width; ++i) {
		depth2dBuffer[i] = (*p_depth)[width*int(height / 2) + i];
	}

	// 逆透視投影変換をして上面図化
#define DEPTH2DWORLD_WIDTH 1000
#define DEPTH2DWORLD_HEIGHT 1000 
#define KINECT_FOV_HORI 70
	// TODO 縦横比が気持ち悪い
	// TODO マジックナンバーが多い
	cv::Mat depth2dWorldAxisImage(DEPTH2DWORLD_HEIGHT, DEPTH2DWORLD_WIDTH, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < width; ++i) {
		double projection_ratio = (double)i / (double)(width/2) - 1;			// 比率
		double max_x = (depth2dBuffer[i] / 10) * tan(PI/180*KINECT_FOV_HORI);	// そのzにおけるx方向の最大長さ(cm単位)
		INT16 worldX = projection_ratio * max_x;								// x方向の座標(cm単位)
		INT16 worldXimg = worldX * 0.3 + (DEPTH2DWORLD_WIDTH / 2);				// 表示用に画像座標に変換
		depth2dWorldAxisImage.data[worldXimg + (int)(depth2dBuffer[i] / 10 * 0.3) * DEPTH2DWORLD_WIDTH] = 255;
	}
	cv::imshow("2D Depth World Axis", depth2dWorldAxisImage);
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
			//draw_2ddepth(p_depth, kinect.depthHeight, kinect.depthWidth);
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
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
		// Depth�f�[�^��\������
		cv::Mat depthImage(height, width, CV_8UC1);

		// Depth�f�[�^��0-255�̃O���[�f�[�^�ɂ���
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
	// 2�����������𒊏o
	std::vector<UINT16> depth2dBuffer(width, 0);
	for (int i = 0; i < width; ++i) {
		depth2dBuffer[i] = (*p_depth)[width*int(height / 2) + i];
	}
	// �p�[�X���l�������ɏ�ʐ}��
	cv::Mat depth2dImage(810, width, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < width; ++i) {
		depth2dImage.data[i + (depth2dBuffer[i]/10 * width)] = 255;
	}
	cv::imshow("2D Depth", depth2dImage);
}

void draw_2ddepth_worldaxis(std::vector<UINT16> *p_depth, int height, int width) {
	// 2�����������𒊏o
	std::vector<UINT16> depth2dBuffer(width, 0);
	for (int i = 0; i < width; ++i) {
		depth2dBuffer[i] = (*p_depth)[width*int(height / 2) + i];
	}

	// �t�������e�ϊ������ď�ʐ}��
#define DEPTH2DWORLD_WIDTH 1000
#define DEPTH2DWORLD_HEIGHT 1000 
#define KINECT_FOV_HORI 70
	// TODO �c���䂪�C��������
	// TODO �}�W�b�N�i���o�[������
	cv::Mat depth2dWorldAxisImage(DEPTH2DWORLD_HEIGHT, DEPTH2DWORLD_WIDTH, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < width; ++i) {
		double projection_ratio = (double)i / (double)(width/2) - 1;			// �䗦
		double max_x = (depth2dBuffer[i] / 10) * tan(PI/180*KINECT_FOV_HORI);	// ����z�ɂ�����x�����̍ő咷��(cm�P��)
		INT16 worldX = projection_ratio * max_x;								// x�����̍��W(cm�P��)
		INT16 worldXimg = worldX * 0.3 + (DEPTH2DWORLD_WIDTH / 2);				// �\���p�ɉ摜���W�ɕϊ�
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
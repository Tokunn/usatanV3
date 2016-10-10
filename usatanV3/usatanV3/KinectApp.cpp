#include <iostream>
#include <sstream>

#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include <atlbase.h>

#include "error_check.h"

class KinectApp
{
private:
public:

	// Kinect SDK
	CComPtr<IKinectSensor> kinect = nullptr;

	CComPtr<IDepthFrameReader> depthFrameReader = nullptr;

	// 表示部分
	int depthWidth;
	int depthHeight;

	std::vector<UINT16> depthBuffer;

	const char* DepthWindowName = "Depth Image";


	~KinectApp()
	{
		// Kinectの動作を終了する
		if (kinect != nullptr) {
			kinect->Close();
		}
	}

	// 初期化
	void initialize()
	{
		// デフォルトのKinectを取得する
		ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
		ERROR_CHECK(kinect->Open());

		// Depthリーダーを取得する
		CComPtr<IDepthFrameSource> depthFrameSource;
		ERROR_CHECK(kinect->get_DepthFrameSource(&depthFrameSource));
		ERROR_CHECK(depthFrameSource->OpenReader(&depthFrameReader));

		// Depth画像のサイズを取得する
		CComPtr<IFrameDescription> depthFrameDescription;
		ERROR_CHECK(depthFrameSource->get_FrameDescription(
			&depthFrameDescription));
		ERROR_CHECK(depthFrameDescription->get_Width(&depthWidth));
		ERROR_CHECK(depthFrameDescription->get_Height(&depthHeight));

		// Depthの最大値、最小値を取得する
		UINT16 minDepthReliableDistance;
		UINT16 maxDepthReliableDistance;
		ERROR_CHECK(depthFrameSource->get_DepthMinReliableDistance(
			&minDepthReliableDistance));
		ERROR_CHECK(depthFrameSource->get_DepthMaxReliableDistance(
			&maxDepthReliableDistance));
		std::cout << "Depth最小値       : " << minDepthReliableDistance << std::endl;
		std::cout << "Depth最大値       : " << maxDepthReliableDistance << std::endl;

		// バッファーを作成する
		depthBuffer.resize(depthWidth * depthHeight);

	}

	// データの更新処理
	void update()
	{
		updateDepthFrame();
	}

	void updateDepthFrame()
	{
		// Depthフレームを取得する
		CComPtr<IDepthFrame> depthFrame;
		auto ret = depthFrameReader->AcquireLatestFrame(&depthFrame);
		if (ret != S_OK) {
			return;
		}

		// データを取得する
		ERROR_CHECK(depthFrame->CopyFrameDataToArray(
			depthBuffer.size(), &depthBuffer[0]));
	}

	void draw()
	{
		drawDepthFrame();
	}

	void drawDepthFrame()
	{
		// Depthデータを表示する
		cv::Mat depthImage(depthHeight, depthWidth, CV_8UC1);

		// Depthデータを0-255のグレーデータにする
		for (int i = 0; i < depthImage.total(); ++i) {
			depthImage.data[i] = depthBuffer[i] % 255;
		}


		// cv::imshow(DepthWindowName, depthImage);

		// Depthデータの中心を上面図にする
		std::vector<UINT16> depth2dBuffer(depthWidth, 0);
		for (int i = 0; i < depthWidth; ++i) {
			depth2dBuffer[i] = depthBuffer[depthWidth*int(depthHeight / 2) + i];
		}
		cv::Mat depth2dImage(depthWidth, 800, CV_8UC1, cv::Scalar(0));
		for (int i = 0; i < depthWidth; ++i) {
			depth2dImage.data[(i * 800) + depth2dBuffer[i] / 10] = 255;
		}
		cv::imshow("2D Depth", depth2dImage);

	}

	std::vector<UINT16> *get_depth() {
		return &depthBuffer;
	}
};
#include <iostream>
#include <sstream>
#include <vector>

#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include <atlbase.h>

#include "error_check.h"

//#include "DepthMap.h"

class KinectApp
{
private:
public:

	// Kinect SDK
	CComPtr<IKinectSensor> kinect = nullptr;

	CComPtr<IDepthFrameReader> depthFrameReader = nullptr;

	CComPtr<ICoordinateMapper> mapper = nullptr;

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

		// self
		ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));

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


		cv::imshow(DepthWindowName, depthImage);
	}

	void draw2dMap() {
		// 2次元距離情報を抽出
		std::vector<UINT16> depth2dBuffer(depthWidth, 0);
		for (int i = 0; i < depthWidth; ++i) {
			depth2dBuffer[i] = depthBuffer[depthWidth*int(depthHeight / 2) + i];
		}

		cv::Mat depth2dWorldAxisImage(1000, 1000, CV_8UC1, cv::Scalar(0));
		for (int x = 0; x < depthWidth; ++x) {
			DepthSpacePoint depthSpacePoint = { static_cast<float>(x), static_cast<float>(depthHeight / 2) };
			UINT16 depth = depth2dBuffer[x];
			CameraSpacePoint cameraSpacePoint = { 0.0f, 0.0f, 0.0f };
			mapper->MapDepthPointToCameraSpace(depthSpacePoint, depth, &cameraSpacePoint);
			std::cout << cameraSpacePoint.X << "\t" << cameraSpacePoint.Z << "\n" << std::endl;
			//depth2dWorldAxisImage.data[(int)(500+500/2*1000)] = 255;
			depth2dWorldAxisImage.data[(int)(cameraSpacePoint.X*100+500) + (int)(cameraSpacePoint.Z*100 * 1000)] = 255;
		}
		cv::imshow("Kinect2DMap", depth2dWorldAxisImage);
	}

	std::vector<UINT16> *get_depth() {
		return &depthBuffer;
	}
};
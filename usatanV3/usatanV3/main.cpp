#include <iostream>
#include <sstream>

#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include <atlbase.h>

// ���̂悤�Ɏg���܂�
// ERROR_CHECK( ::GetDefaultKinectSensor( &kinect ) );
// ���Ђł̉���̂��߂Ƀ}�N���ɂ��Ă��܂��B���ۂɂ͓W�J�����`�Ŏg�����Ƃ��������Ă��������B
#define ERROR_CHECK( ret )  \
    if ( (ret) != S_OK ) {    \
        std::stringstream ss;	\
        ss << "failed " #ret " " << std::hex << ret << std::endl;			\
        throw std::runtime_error( ss.str().c_str() );			\
    }

class KinectApp
{
private:

	// Kinect SDK
	CComPtr<IKinectSensor> kinect = nullptr;

	CComPtr<IDepthFrameReader> depthFrameReader = nullptr;

	// �\������
	int depthWidth;
	int depthHeight;

	std::vector<UINT16> depthBuffer;

	/*int depthPointX;
	int depthPointY;*/

	const char* DepthWindowName = "Depth Image";

public:

	~KinectApp()
	{
		// Kinect�̓�����I������
		if (kinect != nullptr) {
			kinect->Close();
		}
	}

	// ������
	void initialize()
	{
		// �f�t�H���g��Kinect���擾����
		ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
		ERROR_CHECK(kinect->Open());

		// Depth���[�_�[���擾����
		CComPtr<IDepthFrameSource> depthFrameSource;
		ERROR_CHECK(kinect->get_DepthFrameSource(&depthFrameSource));
		ERROR_CHECK(depthFrameSource->OpenReader(&depthFrameReader));

		// Depth�摜�̃T�C�Y���擾����
		CComPtr<IFrameDescription> depthFrameDescription;
		ERROR_CHECK(depthFrameSource->get_FrameDescription(
			&depthFrameDescription));
		ERROR_CHECK(depthFrameDescription->get_Width(&depthWidth));
		ERROR_CHECK(depthFrameDescription->get_Height(&depthHeight));

		/*depthPointX = depthWidth / 2;
		depthPointY = depthHeight / 2;*/

		// Depth�̍ő�l�A�ŏ��l���擾����
		UINT16 minDepthReliableDistance;
		UINT16 maxDepthReliableDistance;
		ERROR_CHECK(depthFrameSource->get_DepthMinReliableDistance(
			&minDepthReliableDistance));
		ERROR_CHECK(depthFrameSource->get_DepthMaxReliableDistance(
			&maxDepthReliableDistance));
		std::cout << "Depth�ŏ��l       : " << minDepthReliableDistance << std::endl;
		std::cout << "Depth�ő�l       : " << maxDepthReliableDistance << std::endl;

		// �o�b�t�@�[���쐬����
		depthBuffer.resize(depthWidth * depthHeight);

		// �}�E�X�N���b�N�̃C�x���g��o�^����
		/*cv::namedWindow(DepthWindowName);
		cv::setMouseCallback(DepthWindowName, &KinectApp::mouseCallback, this);*/
	}

	// �}�E�X�C�x���g�̃R�[���o�b�N
	/*static void mouseCallback(int event, int x, int y, int flags, void* userdata)
	{
		// �����ɓn����this�|�C���^���o�R���ă����o�֐��ɓn��
		auto pThis = (KinectApp*)userdata;
		pThis->mouseCallback(event, x, y, flags);
	}

	// �}�E�X�C�x���g�̃R�[���o�b�N(������)
	void mouseCallback(int event, int x, int y, int flags)
	{
		if (event == CV_EVENT_LBUTTONDOWN) {
			depthPointX = x;
			depthPointY = y;
		}
	}*/

	void run()
	{
		while (1) {
			update();
			draw();

			auto key = cv::waitKey(10);
			if (key == 27) {
				break;
			}
		}
	}

private:

	// �f�[�^�̍X�V����
	void update()
	{
		updateDepthFrame();
	}

	void updateDepthFrame()
	{
		// Depth�t���[�����擾����
		CComPtr<IDepthFrame> depthFrame;
		auto ret = depthFrameReader->AcquireLatestFrame(&depthFrame);
		if (ret != S_OK) {
			return;
		}

		// �f�[�^���擾����
		ERROR_CHECK(depthFrame->CopyFrameDataToArray(
			depthBuffer.size(), &depthBuffer[0]));
	}

	void draw()
	{
		drawDepthFrame();
	}

	void drawDepthFrame()
	{
		// Depth�f�[�^��\������
		cv::Mat depthImage(depthHeight, depthWidth, CV_8UC1);

		// Depth�f�[�^��0-255�̃O���[�f�[�^�ɂ���
		for (int i = 0; i < depthImage.total(); ++i) {
			depthImage.data[i] = depthBuffer[i] % 255;
		}


		// Depth�f�[�^�̃C���f�b�N�X���擾���āA���̏ꏊ�̋�����\������
		/*int index = (depthPointY * depthWidth) + depthPointX;
		std::stringstream ss;
		ss << depthBuffer[index] << "mm";

		cv::circle(depthImage, cv::Point(depthPointX, depthPointY), 10,
			cv::Scalar(0, 0, 255), 2);
		cv::putText(depthImage, ss.str(), cv::Point(depthPointX, depthPointY),
			0, 1, cv::Scalar(0, 255, 255));*/

		cv::imshow(DepthWindowName, depthImage);

		// Depth�f�[�^�̒��S����ʐ}�ɂ���
		std::vector<UINT16> depth2dBuffer(depthWidth, 0);
		for (int i = 0; i < depthWidth; ++i) {
			depth2dBuffer[i] = depthBuffer[depthWidth*int(depthHeight/2) + i];
		}
		cv::Mat depth2dImage(depthWidth, 800, CV_8UC1, cv::Scalar(0));
		for (int i = 0; i < depthWidth; ++i) {
			//depth2dImage.data[(i * 800) + (depthBuffer[int(depthHeight / 2) + i]/10)] = 255;
			//depth2dImage.data[(depth2dBuffer[i]/10) * depthWidth + i] = 255;
			depth2dImage.data[(i * 800) + depth2dBuffer[i] / 10] = 255;
		}
		cv::imshow("2D Depth", depth2dImage);

	}
};

void main()
{
	try {
		KinectApp app;
		app.initialize();
		app.run();
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
}
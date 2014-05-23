#pragma once
#include "opencv2/core.hpp"
#include "opencv2/cuda.hpp"
#include "opencv2/cudawarping.hpp"
#include <opencv2/photo/photo.hpp>
//#include "FrameProcessor.h"
#include <vector>
#include "Geometry.h"
#include "opencv2/highgui.hpp"

struct transformParameters{
	float rotationAngle;
	int shiftX;
	int shiftY;
	bool verticalFlip;
	bool horizontalFlip;
};

class VideoProcessor
{
public: 
	//FrameProcessor left, right;
	cv::cuda::Stream stream;
	cv::cuda::GpuMat srcLeftFrame, srcRightFrame;
	cv::cuda::GpuMat tmpGpuLeftFrame, tmpGpuRightFrame;
	cv::cuda::GpuMat mapXLeft, mapYLeft;
	cv::cuda::GpuMat mapXRight, mapYRight;
	cv::Mat dstLeftFrame16, dstRightFrame16;
	cv::Mat dstLeftFrame8, dstRightFrame8;
	cv::Mat flipMapXLeft, flipMapXRight;
	cv::Mat flipMapYLeft, flipMapYRight;
	cv::Mat resultImage;
	cv::Mat rotMatLeft, rotMatRight;
	cv::Size size;
	cv::Size newSize;
	int top, left;
	cv::Ptr<cv::TonemapDurand> tonemap;	
	std::vector<void*> res;
	transformParameters params, leftParam, rightParam;
	enum _Side {Left,Right};
public:
	VideoProcessor(void);
	~VideoProcessor(void);
	VideoProcessor(int width, int height, transformParameters left,  transformParameters right);
	std::vector<void*> processFrames(void* leftFrame, void* rightFrame);
	std::pair<int,int> getNewSize();
private:
	void generateMapLeft();
	void generateMapRight();
	void rotateAndShift(int side);
	void remap(int side);
	void generateRotationMatrix();
	void process(int side);
	void cropFrame(int side);
	void mergeFramesSideBySide();
};


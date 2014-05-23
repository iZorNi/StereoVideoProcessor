#include "stdafx.h"
#include "VideoProcessor.h"



VideoProcessor::VideoProcessor(void)
{
}

VideoProcessor::~VideoProcessor(void)
{
}

VideoProcessor::VideoProcessor(int width, int height, transformParameters leftParam,  transformParameters rightParam)
{
	int num_devices = cv::cuda::getCudaEnabledDeviceCount();
	if(num_devices==0)
	{
		printf("No cuda devices\n");
		exit(-1);
	}
	else
	{
		cv::cuda::setDevice(0);
		size.height = height;
		size.width = width;
		stream = cv::cuda::Stream();
		this->leftParam = leftParam;
		this->rightParam = rightParam;
		generateMapLeft();
		generateMapRight();
		generateRotationMatrix();
		tonemap = cv::createTonemap(2.2f);
		int w,h;
		std::pair<int,int> tl = Geometry::getNewRect(0,0, size.height, size.width, leftParam.rotationAngle, rightParam.rotationAngle, h, w);
		newSize = cv::Size(w,h);
		top = tl.first;
		left = tl.second;
	}

}

void VideoProcessor::generateMapLeft()
{

	if(leftParam.horizontalFlip && leftParam.verticalFlip)
	{
		flipMapXLeft.create(size, CV_32FC1);
		flipMapYLeft.create(size, CV_32FC1);
		for(int j=0; j<size.height; j++)
			for(int i=0; i<size.width; i++)
			{
				flipMapXLeft.at<float>(j,i) = size.width - i;
				flipMapYLeft.at<float>(j,i) = size.height - j;
			}
	}
	else if(leftParam.horizontalFlip)
	{
		flipMapXLeft.create(size, CV_32FC1);
		flipMapYLeft.create(size, CV_32FC1);
		for(int j=0; j<size.height; j++)
			for(int i=0; i<size.width; i++)
			{
				flipMapXLeft.at<float>(j,i) = size.width - i ;
				flipMapYLeft.at<float>(j,i) = j;
			}
	}
	else if(leftParam.verticalFlip)
	{
		flipMapXLeft.create(size, CV_32FC1);
		flipMapYLeft.create(size, CV_32FC1);
		for(int j=0; j<size.height; j++)
			for(int i=0; i<size.width; i++)
			{
				flipMapXLeft.at<float>(j,i) = i ;
				flipMapYLeft.at<float>(j,i) = size.height - j;
			}
	}
	mapXLeft.upload(flipMapXLeft, stream);
	mapYLeft.upload(flipMapYLeft, stream);
}

void VideoProcessor::generateMapRight()
{

	if(rightParam.horizontalFlip && rightParam.verticalFlip)
	{
		flipMapXRight.create(size, CV_32FC1);
		flipMapYRight.create(size, CV_32FC1);
		for(int j=0; j<size.height; j++)
			for(int i=0; i<size.width; i++)
			{
				flipMapXRight.at<float>(j,i) = size.width - i;
				flipMapYRight.at<float>(j,i) = size.height - j;
			}
	}
	else if(rightParam.horizontalFlip)
	{
		flipMapXRight.create(size, CV_32FC1);
		flipMapYRight.create(size, CV_32FC1);
		for(int j=0; j<size.height; j++)
			for(int i=0; i<size.width; i++)
			{
				flipMapXRight.at<float>(j,i) = size.width - i ;
				flipMapYRight.at<float>(j,i) = j;
			}
	}
	else if(rightParam.verticalFlip)
	{
		flipMapXRight.create(size, CV_32FC1);
		flipMapYRight.create(size, CV_32FC1);
		for(int j=0; j<size.height; j++)
			for(int i=0; i<size.width; i++)
			{
				flipMapXRight.at<float>(j,i) = i ;
				flipMapYRight.at<float>(j,i) = size.height - j;
			}
	}
	mapXRight.upload(flipMapXRight, stream);
	mapYRight.upload(flipMapYRight, stream);
}

void VideoProcessor::generateRotationMatrix()
{
	rotMatLeft = cv::getRotationMatrix2D (cv::Point2f(size.width/2.f,size.height/2.f),leftParam.rotationAngle, 1.0);
	rotMatRight = cv::getRotationMatrix2D (cv::Point2f(size.width/2.f,size.height/2.f),rightParam.rotationAngle, 1.0);
	return;
}

std::vector<void*> VideoProcessor::processFrames(void* leftFrame, void* rightFrame)
{
	//left frame
	srcLeftFrame.upload(cv::Mat(size, CV_16UC3, leftFrame));
	process(Left);
	stream.waitForCompletion();
	//right frame
	srcRightFrame.upload(cv::Mat(size, CV_16UC3, rightFrame));
	process(Right);
	tonemap->process(dstLeftFrame16, dstLeftFrame8);
	//dstLeftFrame16.convertTo(dstLeftFrame8, CV_8UC3);
	cropFrame(Left);
	res.push_back(dstLeftFrame8.data);
	stream.waitForCompletion();
	tonemap->process(dstRightFrame16, dstRightFrame8);
	//dstRightFrame16.convertTo(dstRightFrame8, CV_8UC3);
	cropFrame(Right);
	res.push_back(dstRightFrame8.data);
	//mergeFramesSideBySide();
	//cv::imwrite("resultimage.JPG",resultImage);
	return res;
}

void VideoProcessor::process(int side)
{
	remap(side);
	rotateAndShift(side);
	switch(side)
	{
	case Left:
		{
			srcLeftFrame.convertTo(tmpGpuLeftFrame, CV_8UC3,stream);
			//tmpGpuLeftFrame.download(dstLeftFrame8,stream);
			srcLeftFrame.download(dstLeftFrame16,stream);
			break;
		}
	case Right:
		{
			srcRightFrame.convertTo(tmpGpuRightFrame, CV_8UC3,stream);
			//tmpGpuRightFrame.download(dstRightFrame8,stream);
			srcRightFrame.download(dstRightFrame16,stream);
			break;
		}
	}
	return;
	//tonemap->process(srcLeftFrame, dstLeftFrame);
	//tonemap->process(srcRightFrame, dstRightFrame);
	
}

void VideoProcessor::remap(int side)
{
	switch(side)
	{
	case Left:
		{
			if(leftParam.horizontalFlip || leftParam.verticalFlip)
				cv::cuda::remap(srcLeftFrame,tmpGpuLeftFrame,mapXLeft,mapYLeft,cv::INTER_NEAREST, 0,cv::Scalar(),stream);
			else
				srcLeftFrame.copyTo(tmpGpuLeftFrame,stream);
			break;
		}
	case Right:
		{
			if(rightParam.horizontalFlip || rightParam.verticalFlip)	
				cv::cuda::remap(srcRightFrame,tmpGpuRightFrame,mapXRight,mapYRight,cv::INTER_NEAREST, 0,cv::Scalar(),stream);
			else
				srcRightFrame.copyTo(tmpGpuRightFrame,stream);
			break;
		}
	}
	return;
}

void VideoProcessor::rotateAndShift(int side)
{
	switch(side)
	{
	case Left:
		{
			cv::cuda::warpAffine(tmpGpuLeftFrame, srcLeftFrame,rotMatLeft,size,1,0,cv::Scalar(), stream);
			break;
		}
	case Right:
		{
			cv::cuda::warpAffine(tmpGpuRightFrame, srcRightFrame,rotMatRight,size,1,0,cv::Scalar(), stream);
			break;
		}
	}

	return;
}

void VideoProcessor::cropFrame(int side)
{
	switch(side)
	{
	case Left:
		{
			dstLeftFrame8= dstLeftFrame8(cv::Range(top, top+newSize.height), cv::Range(left, left+newSize.width));
			break;
		}
	case Right:
		{
			dstRightFrame8= dstRightFrame8(cv::Range(top, top+newSize.height), cv::Range(left, left+newSize.width));
			break;
		}
	}
}

void VideoProcessor::mergeFramesSideBySide()
{
	resultImage = cv::Mat(newSize.height, 2*newSize.width, CV_16UC3);
	dstLeftFrame8.copyTo( resultImage (cv::Range::all(),cv::Range(0,newSize.width)) );
	dstRightFrame8.copyTo( resultImage (cv::Range::all(),cv::Range(newSize.width,newSize.width*2)) );
	return;
}

std::pair<int,int> VideoProcessor::getNewSize()
{
	return std::pair<int,int>(newSize.height, newSize.width);
}



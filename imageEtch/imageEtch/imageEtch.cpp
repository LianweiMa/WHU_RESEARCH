#include <stdio.h>
#include <opencv_2_4_7/opencv/cv.h>
#include "opencv_2_4_7/opencv/highgui.h"
#include <opencv_2_4_7/opencv/cvaux.h> //必须引此头文件

int main(int argc, char** argv)
{
	char strInputFile[512]; strcpy(strInputFile, argv[1]);
	IplImage *imgInput = cvLoadImage(strInputFile, 0);	
	if (!imgInput){printf("image load failed!\n");return false;}

	////Binary
	//IplImage* imgBinary = cvCreateImage(cvGetSize(imgInput), 8, 1);
	//cvThreshold(imgInput, imgBinary, 2, 1, CV_THRESH_BINARY);
	//cvNamedWindow("BinaryImageShow", CV_WINDOW_AUTOSIZE);
	//cvShowImage("BinaryImageShow", imgBinary);
	//cvWaitKey(-1);
	//Dilate
	IplImage *imgErode = cvCreateImage(cvGetSize(imgInput), 8, 1);
	IplImage *imgDliate = cvCreateImage(cvGetSize(imgInput), 8, 1);
	cvErode(imgInput, imgErode, NULL, 2);
	cvDilate(imgErode, imgDliate, NULL, 2);
	
	//char strDilateFile[512]; strcpy(strDilateFile, strInputFile);
	//sprintf(strrchr(strDilateFile, '.'), "%s", "_Erode.tif");
	//cvSaveImage(strDilateFile, imgDliate);
	/*cvNamedWindow("DilateImageShow", CV_WINDOW_AUTOSIZE);
	cvShowImage("DilateImageShow", imgDliate);
	cvWaitKey(-1);*/
	///*cvErode(imgInput, imgOutput, NULL, 1); 
	//cvDilate(imgInput, imgOutput, NULL, 1);*/
	
	//搜索轮廓
	IplImage* imgOutput = cvCloneImage(imgDliate);
	CvSeq* contour = NULL;
	CvMemStorage* storage = cvCreateMemStorage(0);
	int contourNum = cvFindContours(imgDliate, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	////显示轮廓
	IplImage* imgContours = cvCreateImage(cvGetSize(imgInput), 8, 1);    
	cvRectangle(imgContours, cvPoint(0, 0), cvPoint(imgContours->width, imgContours->height), CV_RGB(255, 255, 255), CV_FILLED);// 填充成白色
	cvDrawContours(imgContours, contour, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), 5, 2);
	cvNamedWindow("imageShowContours", CV_WINDOW_AUTOSIZE);
	cvShowImage("imageShowContours", imgContours);
	cvWaitKey(-1);
	//开始遍历轮廓树
	
	double minarea = 100.0, tmparea = 0.0;
	uchar *pp;
	for (; contour; contour = contour->h_next)
	{
		CvRect rect;
		tmparea = fabs(cvContourArea(contour));

		rect = cvBoundingRect(contour, 0);
		if (tmparea < minarea/*||tmparea>4900*/)
		{

			//当连通域的中心点为黑色时，而且面积较小则用白色进行填充     
			pp = (uchar*)(imgOutput->imageData + imgOutput->widthStep*(rect.y + rect.height / 2) + rect.x + rect.width / 2);
			if (pp[0] == 0)
			{
				for (int y = rect.y; y < rect.y + rect.height; y++)
				{
					for (int x = rect.x; x < rect.x + rect.width; x++)
					{
						pp = (uchar*)(imgOutput->imageData + imgOutput->widthStep*y + x);

						if (pp[0] == 0)
						{
							pp[0] = 255;
						}
					}
				}
			}

		}
	}
	//cvNamedWindow("imageShow",CV_WINDOW_AUTOSIZE);
	//cvShowImage("imageShow", imgOutput);
	//cvWaitKey(-1);
	/*cvDilate(imgOutput, imgDliate, NULL, 1);
	cvNamedWindow("DilateImageShow", CV_WINDOW_AUTOSIZE);
	cvShowImage("DilateImageShow", imgDliate);
	cvWaitKey(-1);*/
	char strOutputFile[512]; strcpy(strOutputFile, strInputFile);
	sprintf(strrchr(strOutputFile, '.'), "%s", "_Plane.tif");
	cvSaveImage(strOutputFile, imgOutput);


	cvReleaseImage(&imgInput);
	cvReleaseImage(&imgOutput);
	cvDestroyAllWindows();
	return 0;
}
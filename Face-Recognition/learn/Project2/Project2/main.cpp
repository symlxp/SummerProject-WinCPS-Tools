#include <cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <opencv2/face/facerec.hpp>
#include <facedetect-dll.h>
#include <iostream>
#include <string>
#include <cstring>
#include <windows.h>
#define DETECT_BUFFER_SIZE 0x20000
using namespace cv;
using namespace std;
using namespace face;
void recognize(Mat src, string path)
{
	//分配空间失败返回-1,中断识别返回0,识别到返回1,正在检测返回2,存储完成返回3.
	int * pResults = NULL;
	//pBuffer is used in the detection functions.
	//If you call functions in multiple threads, please create one buffer for each thread!
	unsigned char * pBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);
	if (!pBuffer)
	{
		fprintf(stderr, "Can not alloc buffer.\n");
		return;
	}
	Mat face;
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);//转为灰度图
    equalizeHist(gray, gray);
	int doLandmark = 1;// do landmark detection
					   ///////////////////////////////////////////
					   // frontal face detection / 68 landmark detection
					   //正面人脸检测/ 68标志性检测
					   // it's fast, but cannot detect side view faces
					   //它很快，但不能检测侧面的脸
					   //////////////////////////////////////////
	pResults = facedetect_multiview_reinforce(pBuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
		1.2f, 2, 48, 0, doLandmark);
	//print the detection results
	if (pResults != NULL)
	{
		for (int i = 0; i < (pResults ? *pResults : 0); i++)
		{
			short * p = ((short*)(pResults + 1)) + 142 * i;
			Rect_<float> face_rect = Rect_<float>(p[0], p[1], p[2], p[3]);
			face = src(face_rect);
			//cv::rectangle(src, Rect(p[0], p[1], p[2], p[3]), Scalar(230, 255, 0), 2);
			//rectangle(src, Rect(p[0], p[1], p[2], p[3]), Scalar(0, 255, 0), 2);

			/*if (doLandmark)
			{
			for (int j = 0; j < 68; j++)
			circle(src, Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]), 1, Scalar(0, 0, 255), 2);
			}*/
		}
		if (face.empty() || !face.data)
		{
			free(pBuffer);
			return;
		}
		else
		{
			cv::resize(face, face, Size(120, 120));
			imwrite(path, face);
			//imshow("ss", face);
			free(pBuffer);
			return;
		}

	}
	else
	{
		//face is moving, and reset the detect count
		free(pBuffer);
		return;
	}
}

void read_csv(const string& filename, char separator = ';')
{


	std::ifstream file(filename.c_str(), ifstream::in);//c_str()函数可用可不用，无需返回一个标准C类型的字符串

													   /*if (!file) {

													   string error_message = "No valid input file was given, please check the given filename.";
													   //CV_Error(CV_StsBadArg, error_message);

													   }*/

	string line, path, classlabel;
	Mat in;
	while (getline(file, line))//从文本文件中读取一行字符，未指定限定符默认限定符为“/n”

	{

		stringstream liness(line);//这里采用stringstream主要作用是做字符串的分割

		getline(liness, path, separator);//读入图片文件路径以分好作为限定符

		getline(liness, classlabel);//读入图片标签，默认限定符

		if (!path.empty() && !classlabel.empty())//如果读取成功，则将图片和对应标签压入对应容器中 

		{

			//qDebug() << path<<" "<< classlabel<<endl;
			in=imread(path, 1);
			recognize(in, path);
		}
	}
}

void w_csv()
{
	system("dir /b/s d:\\faces\\*.jpg > d:/faces/a.csv");
	//csvfile.close();
	std::ifstream file("d:/faces/a.csv", ifstream::in);

	if (!file) {

		string error_message = "No valid input file was given, please check the given filename.";
		cout << error_message;
		return;

	}
	string line, tmp1, tmp2, out;
	int linegroup = 0;
	char fujia[5] = { 0 };
	while (getline(file, line))//从文本文件中读取一行字符，未指定限定符默认限定符为“/n”
	{
		tmp2 = tmp1;
		std::size_t found = line.find_last_of('\\');
		//std::cout << line.substr(0, found) << endl;
		tmp1 = line.substr(0, found);
		if (tmp1 != tmp2)
			linegroup++;
		sprintf_s(fujia, ";%d", linegroup);
		out += line + fujia + '\n';
	}
	file.close();
	ofstream outfile;
	outfile.open("d:/faces/a.csv");
	outfile.flush();
	outfile << out;
	outfile.close();

}
int main()
{
	vector<Mat> images;
	vector<int> labels;
	w_csv();
	read_csv("d:/faces/a.csv");
	system("pause");
	return 0;
}
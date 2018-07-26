
#include <cv.h>
#include <opencv2/opencv.hpp>
//#include <opencv2/core.hpp>
#include <opencv2/face.hpp>
#include <opencv2/face/facerec.hpp>
#include <iostream>
#include <string>
#include <cstring>
#include <windows.h>
using namespace cv;
using namespace std;
using namespace face;
void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';')
{


	std::ifstream file(filename.c_str(), ifstream::in);//c_str()函数可用可不用，无需返回一个标准C类型的字符串

													   /*if (!file) {

													   string error_message = "No valid input file was given, please check the given filename.";
													   //CV_Error(CV_StsBadArg, error_message);

													   }*/

	string line, path, classlabel;
	Mat tmp;

	while (getline(file, line))//从文本文件中读取一行字符，未指定限定符默认限定符为“/n”

	{

		stringstream liness(line);//这里采用stringstream主要作用是做字符串的分割

		getline(liness, path, separator);//读入图片文件路径以分好作为限定符

		getline(liness, classlabel);//读入图片标签，默认限定符

		if (!path.empty() && !classlabel.empty())//如果读取成功，则将图片和对应标签压入对应容器中 

		{

			//qDebug() << path<<" "<< classlabel<<endl;
			tmp=imread(path, CV_LOAD_IMAGE_GRAYSCALE);
			equalizeHist(tmp, tmp);
			images.push_back(tmp);

			labels.push_back(atoi(classlabel.c_str()));

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
		if (tmp1 == "d:\\faces\\dreacter_faces") {
			out += line + ";66\n";
			continue;
		}

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
	cout<<"开始学习"<<endl;
	w_csv();
	read_csv("d:/faces/a.csv", images, labels);
	//Ptr<FaceRecognizer> model = LBPHFaceRecognizer::create();
	//Ptr<FaceRecognizer> model = FisherFaceRecognizer::create();
	Ptr<FaceRecognizer> model = EigenFaceRecognizer::create();
	model->train(images, labels);
	CreateDirectory("d:/faces/model", NULL);
	model->write("d:/faces/model/Face_Model.xml");
	system("del d:\\faces\\a.csv /Q");

	cout<<"学习完毕"<<endl;
	//system("pause");
	return 0;
}
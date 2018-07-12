#if _MSC_VER >= 1600  
#pragma execution_character_set("utf-8")
#endif
#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <cstdio>
//#include <qdebug.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->open->setEnabled(true);
	ui->close->setEnabled(false);
	ui->label->setScaledContents(true);
	connect(this, SIGNAL(finishgetfaces()), this, SLOT(learning()));
	timer = NULL;
	TCHAR strBuffer[256] = { 0 };
	DWORD dwSize = 256;
	GetUserName(strBuffer, &dwSize);
	//wcout<<strBuffer<<endl;
	//wsprintf(username,"%s",strBuffer);
	QString name = QString::fromUtf16((const ushort*)strBuffer);
	output_folder = "d:/faces/dreacter_faces";
	//output_folder = "d:/faces/" + name + "_faces";
	//qDebug() << output_folder;
	QDir dir;
	if (!dir.exists(output_folder))
		dir.mkpath(output_folder);
	ui->statuslabel->setText("请点击打开摄像头录入人脸！");
}

MainWindow::~MainWindow()
{
    delete ui;
}
/*void MainWindow::read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';')

{

	std::ifstream file(filename.c_str(), ifstream::in);//c_str()函数可用可不用，无需返回一个标准C类型的字符串

	if (!file) {

		string error_message = "No valid input file was given, please check the given filename.";

		CV_Error(CV_StsBadArg, error_message);

	}

	string line, path, classlabel;

	while (getline(file, line))//从文本文件中读取一行字符，未指定限定符默认限定符为“/n”

	{

		stringstream liness(line);//这里采用stringstream主要作用是做字符串的分割

		getline(liness, path, separator);//读入图片文件路径以分好作为限定符

		getline(liness, classlabel);//读入图片标签，默认限定符

		if (!path.empty() && !classlabel.empty())//如果读取成功，则将图片和对应标签压入对应容器中 

		{

			images.push_back(imread(path, 0));

			labels.push_back(atoi(classlabel.c_str()));

		}

	}

}*/
void  MainWindow::learning()
{
	on_close_clicked();
	vector<Mat> images;
	vector<int> labels;
	ui->statuslabel->setText("人脸数据录入成功，正在学习特征！");
	// images for me
	for (int i = 0; i < 10; i++)
	{
		Mat a;
		a = imread(format("%s/%d.jpg", output_folder.toStdString().c_str(), i), CV_LOAD_IMAGE_GRAYSCALE);
		//cv::resize(a, a, Size(120, 120));
		images.push_back(a);
		labels.push_back(1);
	}
	// images for second person
	for (int i = 0; i < 10; i++)
	{
		Mat a;
		a = imread(format("d:/faces/Bill_Gates/%d.jpg", i), CV_LOAD_IMAGE_GRAYSCALE);
		//cv::resize(a, a, Size(120, 120));
		images.push_back(a);
		labels.push_back(3);
	}
	// images for third person
	for (int i = 0; i < 10; i++)
	{
		Mat a;
		a = imread(format("d:/faces/Amelie_Mauresmo/%d.jpg", i), CV_LOAD_IMAGE_GRAYSCALE);
		//cv::resize(a, a, Size(120, 120));
		images.push_back(a);
		labels.push_back(5);
	}
	for (int i = 0; i < 10; i++)
	{
		Mat a;
		a = imread(format("d:/faces/Abdullah_Gul/%d.jpg", i), CV_LOAD_IMAGE_GRAYSCALE);
		//cv::resize(a, a, Size(120, 120));
		images.push_back(a);
		labels.push_back(7);
	}

	//Ptr<FaceRecognizer> model = LBPHFaceRecognizer::create();
	Ptr<FaceRecognizer> model = FisherFaceRecognizer::create();
	model->train(images, labels);
	QDir outdir(output_folder+ "/model");
	if (!outdir.exists())
		outdir.mkdir(output_folder + "/model");
	model->write(format("%s/model/Face_Model.xml", output_folder.toStdString().c_str()));
	/*Mat a = imread("d:/Bill_Gates/%1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	try
	{
		if (model->predict(a) == 1)
			qDebug() << "jeskon";
	}
	catch (Excep ex)
	{

	}*/


	//Mat img = imread("person1/2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//int predicted = model->predict(img);
	ui->statuslabel->setText("特征学习完毕，请关闭程序！");

}
/*QImage  MainWindow::Mat2Qimage(const Mat src)
{
	Mat mat;
	//flip(src, mat,1);
	QImage qImg;
	if (mat.channels() == 3)                             //3 channels color image
	{

		cv::cvtColor(mat, mat, CV_BGR2RGB);
		qImg = QImage((const unsigned char*)(mat.data),
			mat.cols, mat.rows,
			mat.cols*mat.channels(),
			QImage::Format_RGB888);
	}
	else if (mat.channels() == 1)                    //grayscale image
	{
		qImg = QImage((const unsigned char*)(mat.data),
			mat.cols, mat.rows,
			mat.cols*mat.channels(),
			QImage::Format_Indexed8);
	}
	else
	{
		qImg = QImage((const unsigned char*)(mat.data),
			mat.cols, mat.rows,
			mat.cols*mat.channels(),
			QImage::Format_RGB888);
	}

	return qImg;

}

Mat MainWindow::relight(Mat src, double alpha, int beta)
{
	//qDebug() << alpha<<endl;
	//qDebug() << beta << endl;
	/// Do the operation new_image(i,j) = alpha*image(i,j) + beta
	Mat new_image = Mat::zeros(src.size(), src.type());
	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{
			for (int c = 0; c < 3; c++)
			{
				new_image.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha*(src.at<Vec3b>(y, x)[c]) + beta);
			}
		}
	}
	imshow("ss", new_image);
	//string output= output_folder.toStdString();
	//imwrite(format("%s/%d.png", face_detect_count-30,output.c_str()), new_image);
	return new_image;
}
*/
QImage  MainWindow::Mat2Qimage(const Mat src)
{
	Mat mat;
	flip(src, mat, 1);
	// 8-bits unsigned, NO. OF CHANNELS = 1
	if(mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)
		image.setColorCount(256);
		for(int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat
		uchar *pSrc = mat.data;
		for(int row = 0; row < mat.rows; row ++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	// 8-bits unsigned, NO. OF CHANNELS = 3
	else if(mat.type() == CV_8UC3)
	{
		// Copy input Mat
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if(mat.type() == CV_8UC4)
	{
		// Copy input Mat
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		return QImage();
	}
}
/*
void  MainWindow::TakePhoto()
{
    static int i =1;
    ui.pushButton_3->setEnabled(false);
    ui.label_1->setPixmap(QPixmap::fromImage(TakeP));

    sprintf(file,"../Images/Pic_%d.jpg",i);
    i++;
    imwrite(file,frame);
    waitKey(10);
    ui.pushButton_3->setEnabled(true);
    //cout<<"jjjjj"<<endl;

}*/




void MainWindow::on_open_clicked()
{//打开摄像头并初始化计时器捕获第一张图
    ui->open->setEnabled(false);
	ui->close->setEnabled(true);
	ui->statuslabel->setText("摄像头开启成功，请将人脸置入摄像头可见区域！");
	face_detect_count = 0;
    if (cap.isOpened())
        cap.release();     //decide if capture is already opened; if so,close it
    cap.open(0);           //open the default camera
    if (cap.isOpened())
    {
        //int rate= cap.get(CV_CAP_PROP_FPS);
        cap >> frame;
        if (!frame.empty())
        {
            image = Mat2Qimage(frame);
            ui->label->setPixmap(QPixmap::fromImage(image));
            timer = new QTimer(this);
            timer->setInterval(1000/30);   //set timer match with FPS
            connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
            timer->start();
        }
    }
}
void MainWindow::nextFrame()
{//反复调用此函数，获得视频
    cap >> frame;
	int result;
    if (!frame.empty())
    {
		result = recognize(frame);
		if (result == -1|| result == -2)
			ui->statuslabel->setText("系统错误，请不要剧烈晃动！");
		if (result == 0)
			ui->statuslabel->setText("检测不到人脸，请将人脸置入摄像头可见区域！");
		if (result == 1)
			ui->statuslabel->setText("检测到人脸，请略微移动，等待数据录入！");
		if (result == 3)
			emit finishgetfaces();
        image = Mat2Qimage(frame);
        ui->label->setPixmap(QPixmap::fromImage(image));
        //this->update();
    }
}
void MainWindow::on_close_clicked()
{
	ui->open->setEnabled(true);
	if (cap.isOpened())
	   cap.release();
	if(timer!=NULL)
	    if(timer->isActive())
		    timer->stop();
	timer=NULL;
    ui->label->clear();
	ui->statuslabel->setText("摄像头关闭成功！");
}
int MainWindow::recognize(Mat &src)
{
	//分配空间失败返回-1,中断识别返回0,识别到返回1,正在检测返回2,存储完成返回3.
	int * pResults = NULL;
	//pBuffer is used in the detection functions.
	//If you call functions in multiple threads, please create one buffer for each thread!
	unsigned char * pBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);
	if (!pBuffer)
	{
		fprintf(stderr, "Can not alloc buffer.\n");
		face_detect_count = 0;
		return -1;
	}
	Mat face;
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);//转为灰度图
	//equalizeHist(gray, gray);
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
			//Point left(p[0], p[1]);
			//Point right(p[0]+p[2], p[1] + p[3]);
			cv::rectangle(src, Rect(p[0], p[1], p[2], p[3]), Scalar(230, 255, 0), 2);
			//rectangle(src, Rect(p[0], p[1], p[2], p[3]), Scalar(0, 255, 0), 2);

			/*if (doLandmark)
			{
				for (int j = 0; j < 68; j++)
					circle(src, Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]), 1, Scalar(0, 0, 255), 2);
			}*/
		}
		if (face.empty() || !face.data)
		{
			face_detect_count = 0;
			free(pBuffer);
			return 0;
		}
		else
		{
			face_detect_count++;
			if (face_detect_count > 10 && face_detect_count < 21)
			{
				//qsrand(QTime(0, 0, 0).msecsTo(QTime::currentTime()));
				//int a = qrand() % 101;
				//double b = 1.0+ qrand()*2.0 / (double)RAND_MAX;
				//if (face_detect_count > 25)
				//	face=relight(face, b, a);
				cv::resize(face,face, Size(250, 250));
				imwrite(format("%s/%d.jpg", output_folder.toStdString().c_str(), face_detect_count - 11), face);
				//imshow("ss", face);
				free(pBuffer);
				return 2;
			}
			if(face_detect_count >= 21)
				return 3;
		}
	}
	else
	{
		//face is moving, and reset the detect count
		face_detect_count = 0;
		free(pBuffer);
		return -2;
	}
	free(pBuffer);
	return 1;
	

	//imshow("ss", frame);
/*
	///////////////////////////////////////////
	// frontal face detection designed for video surveillance / 68 landmark detection
	//正面人脸检测专为视频监控/ 68标志性检测而设计
	// it can detect faces with bad illumination.
	//它可以检测到不良照明的面部。
	//////////////////////////////////////////
	//!!! The input image must be a gray one (single-channel)
	//!!! DO NOT RELEASE pResults !!!
	pResults = facedetect_frontal_surveillance(pBuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
		1.2f, 2, 48, 0, doLandmark);
	printf("%d faces detected.\n", (pResults ? *pResults : 0));
	Mat result_frontal_surveillance = image.clone();;
	//print the detection results
	for (int i = 0; i < (pResults ? *pResults : 0); i++)
	{
		short * p = ((short*)(pResults + 1)) + 142 * i;
		int x = p[0];
		int y = p[1];
		int w = p[2];
		int h = p[3];
		int neighbors = p[4];
		int angle = p[5];

		printf("face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n", x, y, w, h, neighbors, angle);
		rectangle(result_frontal_surveillance, Rect(x, y, w, h), Scalar(0, 255, 0), 2);
		if (doLandmark)
		{
			for (int j = 0; j < 68; j++)
				circle(result_frontal_surveillance, Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]), 1, Scalar(0, 255, 0));
		}
	}
	imshow("Results_frontal_surveillance", result_frontal_surveillance);


    ///////////////////////////////////////////
    // frontal face detection designed for video surveillance / 68 landmark detection
    //正面人脸检测专为视频监控/ 68标志性检测而设计
    // it can detect faces with bad illumination.
    //它可以检测到不良照明的面部。
    //////////////////////////////////////////
    //!!! The input image must be a gray one (single-channel)
    //!!! DO NOT RELEASE pResults !!!
    pResults = facedetect_frontal_surveillance(pBuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
                                                1.2f, 2, 48, 0, doLandmark);
    printf("%d faces detected.\n", (pResults ? *pResults : 0));
    Mat result_frontal_surveillance = image.clone();;
    //print the detection results
    for(int i = 0; i < (pResults ? *pResults : 0); i++)
    {
        short * p = ((short*)(pResults+1))+142*i;
        int x = p[0];
        int y = p[1];
        int w = p[2];
        int h = p[3];
        int neighbors = p[4];
        int angle = p[5];

        printf("face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n", x, y, w, h, neighbors, angle);
        rectangle(result_frontal_surveillance, Rect(x, y, w, h), Scalar(0, 255, 0), 2);
        if (doLandmark)
        {
            for (int j = 0; j < 68; j++)
                circle(result_frontal_surveillance, Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]), 1, Scalar(0, 255, 0));
        }
    }
    imshow("Results_frontal_surveillance", result_frontal_surveillance);

	//imshow("Show", src);
	///////////////////////////////////////////
	// multiview face detection / 68 landmark detection
	//多视角人脸检测/ 68标志性检测
	// it can detect side view faces, but slower than facedetect_frontal().
	//它可以检测侧视面，但比facedetect_frontal（）慢。
	//////////////////////////////////////////
	//!!! The input image must be a gray one (single-channel)
	//!!! DO NOT RELEASE pResults !!!
	pResults = facedetect_multiview(pBuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
		1.2f, 2, 48, 0, doLandmark);

	printf("%d faces detected.\n", (pResults ? *pResults : 0));
	Mat result_multiview = image.clone();;
	//print the detection results
	for (int i = 0; i < (pResults ? *pResults : 0); i++)
	{
		short * p = ((short*)(pResults + 1)) + 142 * i;
		int x = p[0];
		int y = p[1];
		int w = p[2];
		int h = p[3];
		int neighbors = p[4];
		int angle = p[5];

		printf("face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n", x, y, w, h, neighbors, angle);
		rectangle(result_multiview, Rect(x, y, w, h), Scalar(0, 255, 0), 2);
		if (doLandmark)
		{
			for (int j = 0; j < 68; j++)
				circle(result_multiview, Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]), 1, Scalar(0, 255, 0));
		}
	}
	imshow("Results_multiview", result_multiview);


	///////////////////////////////////////////
	// reinforced multiview face detection / 68 landmark detection
	//加强多视图人脸检测/ 68标志性检测
	// it can detect side view faces, better but slower than facedetect_multiview().
	//它可以检测侧面视图，比facedetect_multiview（）更好但更慢。
	//////////////////////////////////////////
	//!!! The input image must be a gray one (single-channel)
	//!!! DO NOT RELEASE pResults !!!
	pResults = facedetect_multiview_reinforce(pBuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
		1.2f, 3, 48, 0, doLandmark);

	printf("%d faces detected.\n", (pResults ? *pResults : 0));
	Mat result_multiview_reinforce = image.clone();;
	//print the detection results
	for (int i = 0; i < (pResults ? *pResults : 0); i++)
	{
		short * p = ((short*)(pResults + 1)) + 142 * i;
		int x = p[0];
		int y = p[1];
		int w = p[2];
		int h = p[3];
		int neighbors = p[4];
		int angle = p[5];

		printf("face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n", x, y, w, h, neighbors, angle);
		rectangle(result_multiview_reinforce, Rect(x, y, w, h), Scalar(0, 255, 0), 2);
		if (doLandmark)
		{
			for (int j = 0; j < 68; j++)
				circle(result_multiview_reinforce, Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]), 1, Scalar(0, 255, 0));
		}
	}
	imshow("Results_multiview_reinforce", result_multiview_reinforce);*/

	//release the buffer
}
/*void MainWindow::on_test_clicked()
{
	//modelPCA = LBPHFaceRecognizer::create();
	modelPCA = FisherFaceRecognizer::create();
	modelPCA->read(format("%s/model/Face_Model.xml", output_folder.toStdString().c_str()));
	if (cap.isOpened())
		cap.release();     //decide if capture is already opened; if so,close it
	cap.open(0);           //open the default camera
	if (cap.isOpened())
	{
		//int rate= cap.get(CV_CAP_PROP_FPS);
		cap >> frame;
		if (!frame.empty())
		{
			image = Mat2Qimage(frame);
			ui->label->setPixmap(QPixmap::fromImage(image));
			timer = new QTimer(this);
			timer->setInterval(1000 / 30);   //set timer match with FPS
			connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame1()));
			timer->start();

		}
	}
}
void MainWindow::nextFrame1()
{//反复调用此函数，获得视频
	cap >> frame;
	int predictPCA = -1;
	Point text_lb;
	if (!frame.empty())
	{
		//this->update();
		//分配空间失败返回-1,中断识别返回0,识别到返回1,正在检测返回2,存储完成返回3.
		int * pResults = NULL;
		//pBuffer is used in the detection functions.
		//If you call functions in multiple threads, please create one buffer for each thread!
		unsigned char * pBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);
		if (!pBuffer)
		{
			fprintf(stderr, "Can not alloc buffer.\n");
		}
		Mat face;
		Mat gray;
		cvtColor(frame, gray, CV_BGR2GRAY);//转为灰度图
		//equalizeHist(gray, gray);
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
				//face = gray(face_rect);
				face = frame(face_rect);
				cv::rectangle(frame, Rect(p[0], p[1], p[2], p[3]), Scalar(230, 255, 0), 2);
				text_lb = Point(p[0]+ p[2], p[1]+ p[3]);
			}
            if (!face.empty())
            {
                //测试图像应该是灰度图
                cvtColor(face, face, CV_BGR2GRAY);
                equalizeHist(face, face);
                cv::resize(face, face, Size(250, 250));
                //imshow("ma", face);
                try
                {
                    predictPCA = modelPCA->predict(face);
                    if (predictPCA == 1)
                    {
                        //qDebug() << "jeskon";
                        putText(frame, "jeskon", text_lb, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
                    }
                    if (predictPCA == 3)
                    {
                        //qDebug() << "jeskon";
                        putText(frame, "Bill", text_lb, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
                    }
                }
                catch (cv::Exception a)
                {
                    qDebug()<<QString::fromStdString(a.err);
                }
                timer->stop();
                cap.release();
            }
		}
        free(pBuffer);
        image = Mat2Qimage(frame);
        ui->label->setPixmap(QPixmap::fromImage(image));
	}
}*/

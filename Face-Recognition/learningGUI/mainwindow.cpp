#if _MSC_VER >= 1600  
#pragma execution_character_set("utf-8")
#endif
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "voice.h"
//#include <cstdio>
#include <qdebug.h>

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

void  MainWindow::learning()
{
	on_close_clicked();
	vector<Mat> images;
	vector<int> labels;
	ui->statuslabel->setText("人脸数据录入成功，正在学习特征！");
	//QFileInfo fileInfo("d:/faces/model/Face_Model.xml");
	//if (!fileInfo.exists())
	//{
		//学习特征
		system("d:/release/learn.exe");
	//}
	//Ptr<FaceRecognizer> model = LBPHFaceRecognizer::create();

	//model->read("d:/faces/model/Face_Model.xml");
	// images for me
	/*for (int i = 1; i < 11; i++)
	{
		Mat a;
		a = imread(format("%s/%d.jpg", output_folder.toStdString().c_str(), i), CV_LOAD_IMAGE_GRAYSCALE);
		equalizeHist(a, a);
		//cv::resize(a, a, Size(120, 120));
		images.push_back(a);
		labels.push_back(66);
	}


	//Ptr<FaceRecognizer> model = FisherFaceRecognizer::create();
	
	model->update(images, labels);
	model->write("d:/faces/model/Face_Model.xml");*/
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
	system("rd /s/q d:\\faces\\dreacter_faces");
	ui->statuslabel->setText("特征学习完毕，请关闭程序！");

}
/*
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




void MainWindow::on_open_clicked()
{//打开摄像头并初始化计时器捕获第一张图
    voice *voicerec=new voice();
	connect(voicerec, SIGNAL(sendData()), this, SLOT(paizhao()));
    voicerec->exec();
}
void MainWindow::paizhao()
{
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
			timer->setInterval(1000 / 30);   //set timer match with FPS
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
				cv::resize(face,face, Size(120, 120));
				imwrite(format("%s/%d.jpg", output_folder.toStdString().c_str(), face_detect_count - 10), face);
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

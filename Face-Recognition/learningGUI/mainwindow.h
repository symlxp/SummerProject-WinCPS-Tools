#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cv.h>
#include <opencv2/opencv.hpp>
//#include <opencv2/core.hpp>
#include <opencv2/face.hpp>
#include <opencv2/face/facerec.hpp>
#include <facedetect-dll.h>
#include <iostream>
#include <QString>
#include <Qtimer>
//#include <QTime>
#include <QDir>
#include <windows.h>
#pragma comment(lib,"Advapi32.lib")
#define DETECT_BUFFER_SIZE 0x20000
using namespace cv;
using namespace std;
using namespace face;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
	QImage Mat2Qimage(const Mat src);

private:
    Ui::MainWindow *ui;
    Mat frame;
    //Mat CannyImg;
    QImage image;
	//Ptr<FaceRecognizer> modelPCA;

    //char file[20];
    //人脸识别
    //Mat edges;
    //Mat gray;
    //QString tempStr;
	int face_detect_count;
    VideoCapture cap;
    QTimer *timer;
	QString output_folder;
    //int AA;
private slots:
    //void OpenVideoCap();
    //void CloseVideoCap();
    //void TakePhoto();
    //人脸识别
    //void FaceRecognition();
    //初始化
    //void Quit();
    //void Slider(int);
    //void checkBox();
    //auto get next frame
    void nextFrame();
    void on_close_clicked();
	int recognize(Mat & src);
    void on_open_clicked();
	void learning();
    void paizhao();
    //void on_test_clicked();
	//void nextFrame1();

signals:
	void finishgetfaces();


};

#endif // MAINWINDOW_H

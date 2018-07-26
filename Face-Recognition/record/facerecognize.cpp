#include "FaceRecognize.h"


//人脸识别主函数
//本函数应于3-5秒内返回，或者使用多线程，以免（过于）阻塞主界面
//@return -1人脸识别失败；0人脸不匹配；1识别成功；
VideoCapture cap;
Ptr<FaceRecognizer> modelPCA;
Mat frame;
//int number=0;
int StartFaceRecognize() {
    //cv::destroyWindow("Face");
    cv::destroyAllWindows();
    //number++;
    //modelPCA = LBPHFaceRecognizer::create();
    //modelPCA = FisherFaceRecognizer::create();
    modelPCA = EigenFaceRecognizer::create();
    int time_all=0;
    //int count = 0;
    double predicted_confidence = 0.0;
    int predictPCA = -1;
    Point text_lb;
    ifstream faceFile;
    faceFile.open("d:/faces/model/Face_Model.xml");
    if (!faceFile.is_open())
    {
        return -1;
    }
    else
        faceFile.close();
    modelPCA->read("d:/faces/model/Face_Model.xml");
    if (cap.isOpened())
        cap.release();     //decide if capture is already opened; if so,close it
    cap.open(0);           //open the default camera
    if (cap.isOpened())
    {
        //if(getWindowProperty("Face", CV_WINDOW_AUTOSIZE)!=-1)


        namedWindow("Face", CV_WINDOW_AUTOSIZE);

        while (1)
        {
            cap >> frame;
            if (!frame.empty())
            {
                time_all++;
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
                Mat show;
                Point tishi(0, 20);
                cvtColor(frame, gray, CV_BGR2GRAY);//转为灰度图
                equalizeHist(gray, gray);
                int doLandmark = 1;// do landmark detection
                                   ///////////////////////////////////////////
                                   // frontal face detection / 68 landmark detection
                                   //正面人脸检测/ 68标志性检测
                                   // it's fast, but cannot detect side view faces
                                   //它很快，但不能检测侧面的脸
                                   //////////////////////////////////////////
                if(time_all>10)
                    pResults = facedetect_multiview_reinforce(pBuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
                    1.2f, 2, 48, 0, doLandmark);
                flip(frame, show, 1);
                putText(show, "Please put you face into the camera!",tishi , FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
                //imshow(format("Face%d",number), show);
                imshow("Face", show);
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
                    }
                    if (!face.empty()&& face.data)
                    {
                        flip(frame, show, 1);
                        putText(show, "Got face,waiting for recognize!", tishi, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
                        imshow("Face", show);
                        waitKey(1000);
                        //测试图像应该是灰度图
                        cvtColor(face, face, CV_BGR2GRAY);
                        equalizeHist(face, face);
                        cv::resize(face, face, Size(120, 120));
                        try
                        {
                            modelPCA->predict(face, predictPCA, predicted_confidence);
                            if (predictPCA == 66 && predicted_confidence<5000.0) {
                                //EigenFaceRecognizer一般3000越大差距越大
                                //count++;
                                cap.release();
                                free(pBuffer);
                                flip(frame, show, 1);
                                putText(show, format("Success! %lf", predicted_confidence), tishi, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
                                imshow("Face", show);
                                waitKey(1000);
                                cv::destroyWindow("Face");

                                return 1;
                                //putText(show, format("Holding,don't move! %lf", predicted_confidence), tishi, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
                                //imshow("Face", show);
                            }
                            else {
                                cap.release();
                                free(pBuffer);
                                flip(frame, show, 1);
                                putText(show, format("Fail! %lf %d", predicted_confidence, predictPCA), tishi, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
                                imshow("Face", show);
                                waitKey(1000);
                                cv::destroyWindow("Face");
                                return 0;
                            }
                            //if (count > 4)
                            //{

                            //}
                        }
                        catch (cv::Exception a)
                        {
                            cout << a.err;
                        }
                    }
                }
                free(pBuffer);
                waitKey(20);

                if (time_all > 80) {
                    cap.release();
                    cv::destroyWindow("Face");
                    return -1;
                }
            }
        }
    }
    else
        return -1;
}

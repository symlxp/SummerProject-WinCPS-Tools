#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QFile>
#include <QBuffer>
#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>
#include <algorithm>
using namespace std;
// 采样频率
#define SAMPLE_FREQUENCY 16000


namespace Ui {
class MainWindow;
}
struct WAVFILEHEADER
{
    // RIFF 头
    char RiffName[4];
    unsigned long nRiffLength;

    // 数据类型标识符
    char WavName[4];

    // 格式块中的块头
    char FmtName[4];
    unsigned long nFmtLength;

    // 格式块中的块数据
    unsigned short nAudioFormat;
    unsigned short nChannleNumber;
    unsigned long nSampleRate;
    unsigned long nBytesPerSecond;
    unsigned short nBytesPerSample;
    unsigned short nBitsPerSample;

    // 数据块中的块头
    char    DATANAME[4];
    unsigned long   nDataLength;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    size_t ReadWav(const string& filePath, short int voiceData[], size_t sizeData, size_t seek);
	void train();
    void recognize();
    qint64 addWavHeader(QBuffer *buf, QString wavFileName);
    ~MainWindow();


private slots:
    void on_get_clicked();
    void stopRecording();
    void on_go_clicked();
    void finishedPlaying();


private:
    Ui::MainWindow *ui; 
    QByteArray voiceData;
    QBuffer bufDevice;
    QFile inputFile;
    QAudioInput* audio;
    QAudioOutput* audio_out;
};

#endif // MAINWINDOW_H

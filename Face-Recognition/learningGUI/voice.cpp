#if _MSC_VER >= 1600  
#pragma execution_character_set("utf-8")
#endif
#include "voice.h"
#include "ui_voice.h"
#include "mfcc.h"
#include "gmm.h"
#include <QTimer>
#include <iostream>
// 采样频率
#define TRAINSIZE SAMPLE_FREQUENCY*4        //4 secondes of voice for trainning
                                // --- you can increase this value to improve the recognition rate
#define RECOGSIZE SAMPLE_FREQUENCY*2 //1 seconde of voice for recognition
voice::voice(QWidget *parent) :
    QDialog(parent),
    uid(new Ui::voice)
{
    uid->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    uid->edit->append("请按下提取声纹按钮");
    uid->edit->append("并在5秒内读出数字：12345678");
}

voice::~voice()
{
    delete uid;
}
void voice::on_go_clicked()
{
	uid->go->setEnabled(false);
    QAudioFormat format;
    // set up the format you want, eg.
    format.setSampleRate(SAMPLE_FREQUENCY);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        cout << "default format not supported try to use nearest"<<endl;
        format = info.nearestFormat(format);
    }
    audio = new QAudioInput(format, this);
    bufDevice.setBuffer(&voiceData);
    bufDevice.open(QIODevice::WriteOnly | QIODevice::Truncate);

    // Records audio for 3000ms
    QTimer::singleShot(5000, this, SLOT(finishedPlaying()));
    audio->start(&bufDevice);
    uid->edit->append("开始录制...");
    //qDebug() << "record begin!" << endl;
}

void voice::finishedPlaying()
{
   audio->stop();
   bufDevice.close();
   delete audio;
   uid->edit->append("停止录制...");
   bufDevice.setBuffer(&voiceData);

   //qDebug() <<"record end!" << endl;
   if (addWavHeader(&bufDevice, "nb.wav") > 0) {
       uid->edit->append("开始识别...");
       recognize();
   }
   uid->go->setEnabled(true);
}
// 将生成的.raw文件转成.wav格式文件;
qint64 voice::addWavHeader(QBuffer *buf, QString wavFileName)
{
    QIODevice *device=buf;
    // 开始设置WAV的文件头
    // 这里具体的数据代表什么含义请看上一篇文章（Qt之WAV文件解析）中对wav文件头的介绍
    WAVFILEHEADER WavFileHeader;
    qstrcpy(WavFileHeader.RiffName, "RIFF");
    qstrcpy(WavFileHeader.WavName, "WAVE");
    qstrcpy(WavFileHeader.FmtName, "fmt ");
    qstrcpy(WavFileHeader.DATANAME, "data");

    // 表示 FMT块 的长度
    WavFileHeader.nFmtLength = 16;
    // 表示 按照PCM 编码;
    WavFileHeader.nAudioFormat = 1;
    // 声道数目;
    WavFileHeader.nChannleNumber = 1;
    // 采样频率;
    WavFileHeader.nSampleRate = SAMPLE_FREQUENCY;

    // 每次采样得到的样本数据位数;
    WavFileHeader.nBitsPerSample = 16;

    // nBytesPerSample 和 nBytesPerSecond这两个值通过设置的参数计算得到;
    // 数据块对齐单位(每个采样需要的字节数 = 通道数 × 每次采样得到的样本数据位数 / 8 )
    WavFileHeader.nBytesPerSample = WavFileHeader.nChannleNumber*WavFileHeader.nBitsPerSample/8;
    // 波形数据传输速率
    // (每秒平均字节数 = 采样频率 × 通道数 × 每次采样得到的样本数据位数 / 8  = 采样频率 × 每个采样需要的字节数 )
    WavFileHeader.nBytesPerSecond = WavFileHeader.nSampleRate*WavFileHeader.nBytesPerSample;
    if (!device->open(QIODevice::ReadOnly))
    {
            return -1;
    }


    int nSize = sizeof(WavFileHeader);

    WavFileHeader.nRiffLength = device->size() - 8 + nSize;
    WavFileHeader.nDataLength = device->size();
    //qDebug()<<device->size();

    QFile wavFile(wavFileName);

    if (!wavFile.open(QIODevice::WriteOnly))
    {
        return -2;
    }

    // 先将wav文件头信息写入，再将音频数据写入;
    wavFile.write((char *)&WavFileHeader, nSize);
    wavFile.write(device->readAll());

    device->close();
    wavFile.close();
    //qDebug()<<device->size()<<endl<<device->bytesAvailable();
    return device->size();
}
void voice::recognize()
{
    Gmm gmm;
    Mfcc mfcc(16000, 16, 8, Mfcc::Hamming, 24, 12);
    string filePath;
    size_t realSize;
    size_t frameCount;
    vector<vector<double>> melCepData;
    short int bigVoiceBuffer[TRAINSIZE];
    filePath = "d:/a.gmm";
    /*for(int personId=0; personId<=9; personId++)
    {
        filePath = GetFilePath(personId, 0, 2, "gmm");
        gmm.AddModel(filePath, GetPersonName(personId));
    }*/
    gmm.AddModel("d:/0.gmm","wrong");
    gmm.AddModel(filePath, "real");

    filePath="nb.wav";//无声double -15329.3//敲击double -56727.9
    //filePath="F04_1-16000.wav";//double -16938.6
    //filePath="haha.wav";//double -16938.6
    //filePath="F00-16000.wav";//double -20063.4
    realSize = ReadWav(filePath, bigVoiceBuffer, RECOGSIZE, 0);
    if(realSize<1) return;
    frameCount = mfcc.Analyse(bigVoiceBuffer,realSize);
    melCepData = mfcc.GetMFCCData();
    QFile file("nb.wav");
    file.remove();
    if(gmm.Recogniser(melCepData, frameCount))
    {
        uid->edit->append("识别成功");
        emit sendData();
        this->close();
        //cout << "recognize correctly" << endl;
        //return true;
    }
    else
    {
        uid->edit->append("识别失败");
        //cout << "recognize wrong" << endl;
        //return false;
    }
}
size_t voice::ReadWav(const string& filePath, short int voiceData[], size_t sizeData, size_t seek)
{
    ifstream inFile(filePath, ifstream::in|ifstream::binary);
    size_t ret;

    if(!inFile.is_open())
    {
        cout << endl << "Can not open the WAV file !!" << endl;
        return -1;
    }

    char waveheader[44];
    inFile.read(waveheader, 44);

    if(seek!=0) inFile.seekg (seek*sizeof(short int), ifstream::cur);

    inFile.read(reinterpret_cast<char *>(voiceData), sizeof(short int)*sizeData);
    ret = (size_t)inFile.gcount()/sizeof(short int);

    inFile.close();
    return ret;
}



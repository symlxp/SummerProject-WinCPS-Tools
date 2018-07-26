#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->append("请按下提取声纹按钮");
    ui->textEdit->append("并在5秒内读出数字：12345678");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_get_clicked()
{
    /** @function
    ********************************************************************************
    <PRE>
    [IN] pWaveform: WAVEFORMATEX结构指针
    [IN] ChannelCount: 声道 [1（单声道）or 2（立体声）]
    [IN] setSampleRate: 采样频率
    [IN] setSampleSize: 单样本比特数8/16
    </PRE>
    *******************************************************************************/
    ui->get->setEnabled(false);
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
        qWarning() << "default format not supported try to use nearest";
        format = info.nearestFormat(format);
    }
    audio = new QAudioInput(format, this);
    bufDevice.setBuffer(&voiceData);
    bufDevice.open(QIODevice::WriteOnly | QIODevice::Truncate);

    // Records audio for 3000ms
    QTimer::singleShot(5000, this, SLOT(stopRecording()));
    audio->start(&bufDevice);
    ui->textEdit->append("开始录制...");
}
void MainWindow::stopRecording()
{
   audio->stop();
   bufDevice.close();
   delete audio;
   ui->textEdit->append("停止录制...");
   bufDevice.setBuffer(&voiceData);
   if (addWavHeader(&bufDevice, "d:/haha.wav") > 0) {
       QMessageBox::information(NULL, tr("恭喜"), tr("录制完毕"));
       qApp->exit(0);
   }

}
// 将生成的.raw文件转成.wav格式文件;
qint64 MainWindow::addWavHeader(QBuffer *buf, QString wavFileName)
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



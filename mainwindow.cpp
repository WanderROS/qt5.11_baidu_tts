#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    player=new QMediaPlayer(this);
    m_apiKey="Q8LQyw208UWnbRGMmf3dTYIQ";
    m_secretKey="XSq685g4RjciXDsR9WhWc9p0TE0CV5j1";
    refreshToken();
    deviceFind();
}

MainWindow::~MainWindow()
{
    delete ui;
}
//合成语音并播放
void MainWindow::on_pushButton_clicked()
{
        QByteArray url="https://tsn.baidu.com/text2audio?";
        url.append(QString("&lan=zh&cuid=14347490&ctp=1&tok=%1&pit=6&per=3").arg(m_token).toLatin1());
        url.append("&tex=");
        url.append(QUrl::toPercentEncoding(ui->textEdit->toPlainText()));
        //qDebug()<<url;
        player->setMedia(QUrl(url));//"/Users/wander/Music/网易云音乐/刘允乐 - 太早.mp3"
        player->play();
}

//获取百度的鉴定认证
void MainWindow::refreshToken(void)
{
    QNetworkRequest request(QUrl(QString("https://aip.baidubce.com/oauth/2.0/token?")));
    QString append = QString("grant_type=client_credentials&client_id=%1&client_secret=%2&").arg(m_apiKey, m_secretKey).toLatin1();

    request.setRawHeader("Content-Type", "application/json");

    reply=manager.post(request,append.toUtf8());
    connect(reply, &QNetworkReply::finished, this, &MainWindow::finished);


}
void MainWindow::finished()
{
    QByteArray bytes = reply->readAll();
    reply->deleteLater();
   // qDebug()<<bytes;
    const auto &&data = QJsonDocument::fromJson(bytes).object();
    if(data.isEmpty() || !data.contains("access_token"))
        return;

    m_token = data["access_token"].toString();
   // qDebug()<<m_token;
}
void MainWindow::deviceFind()
{
    const auto &&availableDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
       if(!availableDevices.isEmpty())
       {
           m_currentDevice = availableDevices.first();

           QAudioFormat format;
           format.setSampleRate(8000);
           format.setChannelCount(1);
           format.setSampleSize(16);
           format.setSampleType(QAudioFormat::SignedInt);
           format.setByteOrder(QAudioFormat::LittleEndian);
           format.setCodec("audio/pcm");

           m_audioInput = new QAudioInput(m_currentDevice, format, this);
       }
}

void MainWindow::on_start_clicked()
{
       m_buffer = new QBuffer;
       m_buffer->open(QIODevice::ReadWrite);
       m_audioInput->start(m_buffer);
}

void MainWindow::on_stop_clicked()
{
       m_audioInput->stop();

       const auto &sendData = m_buffer->data();
       m_buffer->deleteLater();

       QNetworkRequest request(QUrl("http://vop.baidu.com/server_api"));
       QJsonObject append;

       request.setRawHeader("Content-Type", "application/json");

       append["format"] = "pcm";
       append["rate"] = 8000;
       append["channel"] = 1;
       append["token"] = m_token;
       append["lan"] = "zh";
       append["cuid"] = "JasonQt";
       append["speech"] = QString(sendData.toBase64());
       append["len"] = sendData.size();

       reply=manager.post(request,QJsonDocument(append).toJson());
       connect(reply, &QNetworkReply::finished, this, &MainWindow::voicefinished);


}
void MainWindow::voicefinished()
{
    QByteArray buf=reply->readAll();
     reply->deleteLater();
     qDebug()<<buf;
     QJsonObject acceptedData(QJsonDocument::fromJson(buf).object());

     if(buf.isEmpty() || acceptedData.isEmpty() || !acceptedData.contains("result"))
         return;

     const auto &&message = acceptedData["result"].toArray()[0].toString();
     QString mes=message.mid(0, message.size() - 1);
     qDebug()<<mes;
     ui->textEdit->setText(mes);
}

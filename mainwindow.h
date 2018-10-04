#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMediaPlayer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioFormat>
#include <QBuffer>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void refreshToken();
    void finished();
    void deviceFind();
    void voicefinished();

private slots:
    void on_pushButton_clicked();

    void on_start_clicked();

    void on_stop_clicked();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QNetworkReply *reply;
    QString m_apiKey;
    QString m_secretKey;
    QString m_token;
    QNetworkAccessManager manager;
    QAudioDeviceInfo m_currentDevice;
    QAudioInput *m_audioInput = NULL;
    QBuffer *m_buffer = NULL;
};

#endif // MAINWINDOW_H

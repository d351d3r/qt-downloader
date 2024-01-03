#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "filedownloader.h"

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDownloadButtonClicked();
    void onCancelButtonClicked();
    void downloadFinished(QNetworkReply *reply);
    void updateProgress(qint64 bytesRead, qint64 totalBytes);
    void enableDownloadUI(bool enable);

private:
    FileDownloader *fileDownloader;
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    QNetworkReply *currentReply;
    QFile *downloadedFile;
    QString downloadedFilePath;

    void onDownloadComplete();
    void onDownloadError(const QString &errorString);
    void startDownload(const QUrl &url);
    void openDownloadedFile();
};

#endif // MAINWINDOW_H

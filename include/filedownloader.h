#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class FileDownloader : public QObject {
    Q_OBJECT

public:
    explicit FileDownloader(QObject *parent = nullptr);
    void downloadFile(const QUrl &url, const QString &savePath);
    void cancelDownload();
    bool isDownloading() const;

signals:
    void downloadProgress(qint64 bytesRead, qint64 totalBytes);
    void downloaded(const QString &filePath); // Изменен для передачи пути к файлу
    void downloadError(const QString &errorString);

private slots:
    void onDownloadProgress(qint64 bytesRead, qint64 totalBytes);
    void onFinished();
    void onErrorOccurred(QNetworkReply::NetworkError error);

private:
    QNetworkAccessManager *networkManager;
    QNetworkReply *currentReply;
    QFile *downloadedFile;
    QString savePath;
    bool downloading;
};

#endif // FILEDOWNLOADER_H

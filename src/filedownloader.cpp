#include "filedownloader.h"
#include <QFile>
#include <QFileInfo>

FileDownloader::FileDownloader(QObject *parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this)), currentReply(nullptr), downloading(false) {
}

void FileDownloader::cancelDownload() {
    if (currentReply) {
        currentReply->disconnect(this);
        currentReply->abort();
        currentReply->deleteLater();
        currentReply = nullptr;
    }
}

bool FileDownloader::isDownloading() const {
    return downloading;
}

void FileDownloader::downloadFile(const QUrl &url, const QString &savePath) {
    this->savePath = savePath;
    QNetworkRequest request(url);
    currentReply = networkManager->get(request);

    connect(currentReply, &QNetworkReply::downloadProgress, this, &FileDownloader::onDownloadProgress);
    connect(currentReply, &QNetworkReply::finished, this, &FileDownloader::onFinished);
    connect(currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &FileDownloader::onErrorOccurred);
    downloading = true;
}

void FileDownloader::onDownloadProgress(qint64 bytesRead, qint64 totalBytes) {
    emit downloadProgress(bytesRead, totalBytes);
}

void FileDownloader::onFinished() {
    if (!currentReply) {
        emit downloadError("Download finished unexpectedly.");
        downloading = false;
        return;
    }

    if (currentReply->error() == QNetworkReply::NoError) {
        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(currentReply->readAll());
            file.close();
            emit downloaded(savePath);
        } else {
            emit downloadError("Error writing to file.");
        }
    } else {
        emit downloadError(currentReply->errorString());
    }

    currentReply->deleteLater();
    currentReply = nullptr;
    downloading = false;
}


void FileDownloader::onErrorOccurred(QNetworkReply::NetworkError error) {
    (void)error; // Используем эту строку, чтобы избежать предупреждения о неиспользуемой переменной
    emit downloadError(currentReply->errorString());
    currentReply->deleteLater();
    currentReply = nullptr;
}

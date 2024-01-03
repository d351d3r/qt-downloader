#include "filedownloader.h"
#include <QFile>
#include <QFileInfo>

FileDownloader::FileDownloader(QObject *parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this)), currentReply(nullptr), downloading(false) {
}

// void FileDownloader::cancelDownload() {
//     if (currentReply && downloading) {
//         currentReply->abort();
//         currentReply->deleteLater();
//         currentReply = nullptr;
//         downloading = false;
//     }
// }
void FileDownloader::cancelDownload() {
    if (currentReply) {
        currentReply->disconnect(this); // Отключение всех сигналов
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
    if (currentReply->error() == QNetworkReply::NoError) {
        QScopedPointer<QFile> downloadedFile(new QFile(savePath));
        if (downloadedFile->open(QIODevice::WriteOnly)) {
            downloadedFile->write(currentReply->readAll());
            downloadedFile->close();
        }
        emit downloaded();
    }
    downloading = false;
    currentReply->deleteLater();
    currentReply = nullptr;
}

void FileDownloader::onErrorOccurred(QNetworkReply::NetworkError error) {
    emit downloadError(currentReply->errorString());
    currentReply->deleteLater();
    currentReply = nullptr;
}

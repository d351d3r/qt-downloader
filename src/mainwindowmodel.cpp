#include "mainwindowmodel.h"
#include "filedownloader.h"

MainWindowModel::MainWindowModel(QObject *parent)
    : QObject(parent), fileDownloader(new FileDownloader(this)) {

    connect(fileDownloader, &FileDownloader::downloadProgress,
            this, &MainWindowModel::downloadProgress);
    connect(fileDownloader, &FileDownloader::downloaded,
            this, [this](const QString &filePath) {
                emit downloadCompleted(filePath);
            });
    connect(fileDownloader, &FileDownloader::downloadError,
            this, &MainWindowModel::downloadError);
}

MainWindowModel::~MainWindowModel() {
    delete fileDownloader;
}

void MainWindowModel::downloadFile(const QUrl &url, const QString &savePath) {
    fileDownloader->downloadFile(url, savePath);
}

void MainWindowModel::cancelDownload() {
    fileDownloader->cancelDownload();
}

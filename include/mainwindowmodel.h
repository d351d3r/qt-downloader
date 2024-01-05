#ifndef MAINWINDOWMODEL_H
#define MAINWINDOWMODEL_H

#include <QObject>
#include <QUrl>

class FileDownloader;

class MainWindowModel : public QObject {
    Q_OBJECT

public:
    explicit MainWindowModel(QObject *parent = nullptr);
    ~MainWindowModel();

    void downloadFile(const QUrl &url, const QString &savePath);
    void cancelDownload();

signals:
    void downloadProgress(qint64 bytesRead, qint64 totalBytes);
    void downloadCompleted(const QString &filePath);
    void downloadError(const QString &errorString);

private:
    FileDownloader *fileDownloader;
};

#endif // MAINWINDOWMODEL_H

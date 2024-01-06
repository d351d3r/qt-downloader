#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "downloadhistory.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindowModel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDownloadButtonClicked();
    void onCancelButtonClicked();
    void updateProgress(qint64 bytesRead, qint64 totalBytes);
    void onDownloadComplete(const QString &filePath);
    void onDownloadError(const QString &errorString);

signals:
    void initiateDownload(const QUrl &url, const QString &savePath);

private:
    Ui::MainWindow *ui;
    MainWindowModel *model;
    DownloadHistory downloadHistory;
    void openDownloadedFile(const QString &filePath);
    void enableDownloadUI(bool enable);
    void populateDownloadTable();
    QString downloadedFileName;
    QString downloadedUrl;


};

#endif // MAINWINDOW_H

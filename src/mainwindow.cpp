// #include "mainwindow.h"
// #include "ui_mainwindow.h"
// #include "filedownloader.h"
// #include <QNetworkRequest>
// #include <QDesktopServices>
// #include <QUrl>
// #include <QFileInfo>
// #include <QStandardPaths>
// #include <QMessageBox>

// MainWindow::MainWindow(QWidget *parent)
//     : QMainWindow(parent), ui(new Ui::MainWindow) {
//     ui->setupUi(this);
//     this->setFixedSize(600, 200);

//     // Фиксируем размер кнопок
//     ui->downloadButton->setFixedHeight(30);
//     ui->cancelButton->setFixedHeight(30);
//     ui->cancelButton->setFixedWidth(80);
//     ui->downloadButton->setDefault(true);

//     // Скрытие кнопки "Cancel" при инициализации
//     ui->cancelButton->setVisible(false);

//     fileDownloader = new FileDownloader(nullptr);
//     QThread *downloadThread = new QThread(this);

//     fileDownloader->moveToThread(downloadThread);

//     connect(ui->downloadButton, &QPushButton::clicked, this, &MainWindow::onDownloadButtonClicked);
//     connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelButtonClicked);

//     // Подключение сигналов от FileDownloader к слотам в MainWindow
//     connect(this, &MainWindow::initiateDownload, fileDownloader, &FileDownloader::downloadFile);
//     connect(fileDownloader, &FileDownloader::downloadProgress, this, &MainWindow::updateProgress);
//     connect(fileDownloader, &FileDownloader::downloaded, this, &MainWindow::onDownloadComplete);
//     connect(fileDownloader, &FileDownloader::downloadError, this, &MainWindow::onDownloadError);
//     connect(downloadThread, &QThread::finished, downloadThread, &QThread::deleteLater);

//     downloadThread->start();

//     ui->progressBar->setValue(0);
//     ui->progressBar->setVisible(false);
// }

// MainWindow::~MainWindow() {
//     delete ui;
//     delete networkManager;
// }

// void MainWindow::onDownloadButtonClicked() {
//     QUrl url(ui->lineEdit->text().trimmed());
//     if (!url.isValid() || url.scheme().isEmpty()) {
//         QMessageBox::warning(this, tr("Invalid URL"), tr("Please enter a valid URL with a scheme (e.g., http://)."));
//         return;
//     }

//     QString savePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + QFileInfo(url.path()).fileName();

//     emit initiateDownload(url, savePath);

//     // Обновление UI
//     ui->lineEdit->setEnabled(false);
//     ui->downloadButton->setVisible(false);
//     ui->cancelButton->setVisible(true);
//     ui->progressBar->setVisible(true);
// }

// void MainWindow::onDownloadComplete() {
//     ui->progressBar->setVisible(false);
//     ui->downloadButton->setVisible(true);
//     ui->cancelButton->setVisible(false);
//     // Открыть папку с загруженным файлом
//     openDownloadedFile();
//     ui->lineEdit->setEnabled(true); // Разблокировать ввод URL
//     ui->lineEdit->clear(); // Очистить поле ввода
// }

// void MainWindow::onDownloadError(const QString &errorString) {
//     QMessageBox::critical(this, tr("Download Error"), errorString);
//     ui->progressBar->setVisible(false);
//     ui->downloadButton->setVisible(true);
//     ui->cancelButton->setVisible(false);
//     ui->lineEdit->setEnabled(true); // Разблокировать lineEdit для нового ввода
//     ui->lineEdit->clear(); // Очистить текущий ввод, если нужно
// }

// void MainWindow::onCancelButtonClicked() {
//     if (fileDownloader && fileDownloader->isDownloading()) {
//         fileDownloader->cancelDownload();
//         ui->lineEdit->setEnabled(true);
//         ui->downloadButton->setVisible(true);
//         ui->cancelButton->setVisible(false);
//         ui->progressBar->setVisible(false);
//     }
// }


// void MainWindow::startDownload(const QUrl &url) {
//     QNetworkRequest request(url);
//     currentReply = networkManager->get(request);

//     connect(currentReply, &QNetworkReply::downloadProgress, this, &MainWindow::updateProgress);
//     connect(currentReply, &QNetworkReply::finished, this, [=](){
//         this->downloadFinished(currentReply);
//     });
// }

// void MainWindow::updateProgress(qint64 bytesRead, qint64 totalBytes) {
//     ui->progressBar->setVisible(true);
//     ui->progressBar->setMaximum(static_cast<int>(totalBytes));
//     ui->progressBar->setValue(static_cast<int>(bytesRead));
// }

// void MainWindow::downloadFinished(QNetworkReply *reply) {
//     if (!reply) return;
//     if (reply->error()) {
//         qDebug() << "Download error: " << reply->errorString();
//         enableDownloadUI(true);
//     } else {

//     QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
//     QString fileName = QFileInfo(reply->url().path()).fileName();
//     downloadedFilePath = desktopPath + "/" + fileName;

//     downloadedFile = new QFile(downloadedFilePath);
//     if (downloadedFile->open(QIODevice::WriteOnly)) {
//         downloadedFile->write(reply->readAll());
//         downloadedFile->close();
//     }

//     openDownloadedFile();
//     enableDownloadUI(true);
//     reply->deleteLater();
//     }
// }

// void MainWindow::enableDownloadUI(bool enable) {
//     ui->lineEdit->setEnabled(enable);
//     ui->downloadButton->setVisible(enable);
//     ui->cancelButton->setVisible(!enable);
//     ui->progressBar->setVisible(!enable && currentReply && currentReply->isRunning());
//     ui->downloadButton->setText(enable ? tr("Download") : tr("Cancel"));
// }

// void MainWindow::openDownloadedFile() {
// // Проверяем операционную систему
// #ifdef Q_OS_WIN
//     QString command = "explorer.exe /select, \"" + downloadedFilePath + "\"";
//     QProcess::startDetached(command);
// #else
//     // Для других ОС просто открываем папку
//     QDesktopServices::openUrl(QUrl::fromLocalFile(downloadedFilePath));
// #endif
// }
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindowmodel.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>
#include <QFileInfo>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new MainWindowModel(this)) {

    ui->setupUi(this);
    this->setFixedSize(600, 200);
    ui->downloadButton->setDefault(true);
    ui->cancelButton->setVisible(false);

    // Фиксируем размер кнопок
    ui->downloadButton->setFixedHeight(30);
    ui->cancelButton->setFixedHeight(30);
    ui->cancelButton->setFixedWidth(80);

    connect(ui->downloadButton, &QPushButton::clicked, this, &MainWindow::onDownloadButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelButtonClicked);

    connect(this, &MainWindow::initiateDownload, model, &MainWindowModel::downloadFile);
    connect(model, &MainWindowModel::downloadProgress, this, &MainWindow::updateProgress);
    connect(model, &MainWindowModel::downloadCompleted, this, &MainWindow::onDownloadComplete);
    connect(model, &MainWindowModel::downloadError, this, &MainWindow::onDownloadError);

    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(false);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onDownloadButtonClicked() {
    QUrl url(ui->lineEdit->text().trimmed());
    if (!url.isValid() || url.scheme().isEmpty()) {
        QMessageBox::warning(this, tr("Invalid URL"), tr("Please enter a valid URL with a scheme (e.g., http://)."));
        return;
    }

    QString savePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + QFileInfo(url.path()).fileName();
    emit initiateDownload(url, savePath);

    enableDownloadUI(false);
}

void MainWindow::onCancelButtonClicked() {
    model->cancelDownload();
    enableDownloadUI(true);
}

void MainWindow::updateProgress(qint64 bytesRead, qint64 totalBytes) {
    ui->progressBar->setMaximum(static_cast<int>(totalBytes));
    ui->progressBar->setValue(static_cast<int>(bytesRead));
}

void MainWindow::onDownloadComplete(const QString &filePath) {
    openDownloadedFile(filePath);
    enableDownloadUI(true);
    ui->lineEdit->clear();
}

void MainWindow::onDownloadError(const QString &errorString) {
    QMessageBox::critical(this, tr("Download Error"), errorString);
    enableDownloadUI(true);
    ui->lineEdit->clear();
}

void MainWindow::openDownloadedFile(const QString &filePath) {
#ifdef Q_OS_WIN
    QString command = "explorer.exe /select, \"" + filePath + "\"";
    QProcess::startDetached(command);
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filePath).absolutePath()));
#endif
}

void MainWindow::enableDownloadUI(bool enable) {
    ui->lineEdit->setEnabled(enable);
    ui->downloadButton->setVisible(enable);
    ui->cancelButton->setVisible(!enable);
    ui->progressBar->setVisible(!enable);
    if (enable) {
        ui->downloadButton->setText(tr("Download"));
    } else {
        ui->downloadButton->setText(tr("Cancel"));
    }
}

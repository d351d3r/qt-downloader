#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filedownloader.h"
#include <QNetworkRequest>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QStandardPaths>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setFixedSize(600, 200);

    // Фиксируем размер кнопок
    ui->downloadButton->setFixedHeight(30);
    ui->cancelButton->setFixedHeight(30);
    ui->cancelButton->setFixedWidth(80);

    // Скрытие кнопки "Cancel" при инициализации
    ui->cancelButton->setVisible(false);

    fileDownloader = new FileDownloader(this); // Создание экземпляра FileDownloader

    connect(ui->downloadButton, &QPushButton::clicked, this, &MainWindow::onDownloadButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelButtonClicked);

    // Подключение сигналов от FileDownloader к слотам в MainWindow
    connect(fileDownloader, &FileDownloader::downloadProgress, this, &MainWindow::updateProgress);
    connect(fileDownloader, &FileDownloader::downloaded, this, &MainWindow::onDownloadComplete);
    connect(fileDownloader, &FileDownloader::downloadError, this, &MainWindow::onDownloadError);

    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(false);
}

MainWindow::~MainWindow() {
    delete ui;
    delete networkManager;
}

void MainWindow::onDownloadButtonClicked() {
    QUrl url(ui->lineEdit->text().trimmed());
    if (!url.isValid() || url.scheme().isEmpty()) {
        QMessageBox::warning(this, tr("Invalid URL"), tr("Please enter a valid URL with a scheme (e.g., http://)."));
        return;
    }

    QString fileName = QFileInfo(url.path()).fileName();
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + fileName;
    downloadedFilePath = savePath;

    fileDownloader->downloadFile(url, savePath);
    ui->lineEdit->setEnabled(false);
    ui->downloadButton->setVisible(false);
    ui->cancelButton->setVisible(true);
    ui->progressBar->setVisible(true);
}


void MainWindow::onDownloadComplete() {
    ui->progressBar->setVisible(false);
    ui->downloadButton->setVisible(true);
    ui->cancelButton->setVisible(false);
    // Открыть папку с загруженным файлом
    openDownloadedFile();
    ui->lineEdit->setEnabled(true); // Разблокировать ввод URL
    ui->lineEdit->clear(); // Очистить поле ввода
}

void MainWindow::onDownloadError(const QString &errorString) {
    QMessageBox::critical(this, tr("Download Error"), errorString);
    ui->progressBar->setVisible(false);
    ui->downloadButton->setVisible(true);
    ui->cancelButton->setVisible(false);
    ui->lineEdit->setEnabled(true); // Разблокировать lineEdit для нового ввода
    ui->lineEdit->clear(); // Очистить текущий ввод, если нужно
}

void MainWindow::onCancelButtonClicked() {
    if (fileDownloader && fileDownloader->isDownloading()) {
        fileDownloader->cancelDownload();
        ui->lineEdit->setEnabled(true);
        ui->downloadButton->setVisible(true);
        ui->cancelButton->setVisible(false);
        ui->progressBar->setVisible(false);
    }
}


void MainWindow::startDownload(const QUrl &url) {
    QNetworkRequest request(url);
    currentReply = networkManager->get(request);

    connect(currentReply, &QNetworkReply::downloadProgress, this, &MainWindow::updateProgress);
    connect(currentReply, &QNetworkReply::finished, this, [=](){
        this->downloadFinished(currentReply);
    });
}

void MainWindow::updateProgress(qint64 bytesRead, qint64 totalBytes) {
    ui->progressBar->setVisible(true);
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);
}

void MainWindow::downloadFinished(QNetworkReply *reply) {
    if (!reply) return;
    if (reply->error()) {
        qDebug() << "Download error: " << reply->errorString();
        enableDownloadUI(true);
    } else {

    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString fileName = QFileInfo(reply->url().path()).fileName();
    downloadedFilePath = desktopPath + "/" + fileName;

    downloadedFile = new QFile(downloadedFilePath);
    if (downloadedFile->open(QIODevice::WriteOnly)) {
        downloadedFile->write(reply->readAll());
        downloadedFile->close();
    }

    openDownloadedFile();
    enableDownloadUI(true);
    reply->deleteLater();
    }
}

void MainWindow::enableDownloadUI(bool enable) {
    ui->lineEdit->setEnabled(enable);
    ui->downloadButton->setVisible(enable);
    ui->cancelButton->setVisible(!enable);
    ui->progressBar->setVisible(!enable && currentReply && currentReply->isRunning());
    ui->downloadButton->setText(enable ? tr("Download") : tr("Cancel"));
}

void MainWindow::openDownloadedFile() {
// Проверяем операционную систему
#ifdef Q_OS_WIN
    QString command = "explorer.exe /select, \"" + downloadedFilePath + "\"";
    QProcess::startDetached(command);
#else
    // Для других ОС просто открываем папку
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(downloadedFilePath).absolutePath()));
#endif
}

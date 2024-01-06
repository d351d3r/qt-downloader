#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindowmodel.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStandardItemModel>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new MainWindowModel(this)) {

    ui->setupUi(this);
    this->setFixedSize(800, 600);
    ui->downloadButton->setDefault(true);
    ui->cancelButton->setVisible(false);

    // Фиксируем размер кнопок
    ui->downloadButton->setFixedHeight(30);
    ui->cancelButton->setFixedHeight(30);
    ui->cancelButton->setFixedWidth(80);

    downloadHistory.loadFromSettings();
    populateDownloadTable();

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

void MainWindow::populateDownloadTable() {
  // Создаем модель для таблицы
  auto *model = new QStandardItemModel(this);
  model->setHorizontalHeaderLabels({"File Name", "URL", "Timestamp", "Status"});

  // Заполняем модель данными из истории загрузок
  for (const auto &record : downloadHistory.getHistory()) {
    QList<QStandardItem *> row;
    row << new QStandardItem(record.fileName)
        << new QStandardItem(record.url)
        << new QStandardItem(record.timestamp.toString("yyyy-MM-dd HH:mm:ss"))
        << new QStandardItem(record.success ? "Success" : "Failed");

    model->appendRow(row);
  }

  // Устанавливаем модель для таблицы
  ui->tableView->setModel(model);
  // Настраиваем отображение таблицы
  ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
// void MainWindow::onDownloadButtonClicked() {
//     QUrl url(ui->lineEdit->text().trimmed());
//     if (!url.isValid() || url.scheme().isEmpty()) {
//         QMessageBox::warning(this, tr("Invalid URL"), tr("Please enter a valid URL with a scheme (e.g., http://)."));
//         return;
//     }

//     QString savePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + QFileInfo(url.path()).fileName();
//     emit initiateDownload(url, savePath);

//     enableDownloadUI(false);
// }


void MainWindow::onDownloadButtonClicked() {
  QUrl url(ui->lineEdit->text().trimmed());
  if (!url.isValid() || url.scheme().isEmpty()) {
    QMessageBox::warning(this, tr("Invalid URL"), tr("Please enter a valid URL with a scheme (e.g., http://)."));
    return;
  }

  QString fileName = QFileInfo(url.path()).fileName();
  QString savePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + fileName;

  // Store file name and URL for history logging
  downloadedFileName = fileName;
  downloadedUrl = url.toString();

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
  DownloadRecord record;
  record.fileName = downloadedFileName;
  record.url = downloadedUrl;
  record.timestamp = QDateTime::currentDateTime();
  record.success = true;

  downloadHistory.addRecord(record);
  openDownloadedFile(filePath);
  enableDownloadUI(true);
  ui->lineEdit->clear();
}




void MainWindow::onDownloadError(const QString &errorString) {
  DownloadRecord record;
  record.fileName = downloadedFileName;
  record.url = downloadedUrl;
  record.timestamp = QDateTime::currentDateTime();
  record.success = false;  // This should be false for errors

  downloadHistory.addRecord(record);
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

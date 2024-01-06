#include "downloadhistory.h"
#include <QSettings>

void DownloadHistory::addRecord(const DownloadRecord &record) {
  history.append(record);
  saveToSettings();
}

QList<DownloadRecord> DownloadHistory::getHistory() const {
  return history;
}

void DownloadHistory::loadFromSettings() {
  QSettings settings;
  int size = settings.beginReadArray("downloadHistory");
  for (int i = 0; i < size; ++i) {
    settings.setArrayIndex(i);
    DownloadRecord record;
    record.fileName = settings.value("fileName").toString();
    record.url = settings.value("url").toString();
    record.timestamp = settings.value("timestamp").toDateTime();
    record.success = settings.value("success").toBool();
    history.append(record);
  }
  settings.endArray();
}

void DownloadHistory::saveToSettings() {
  QSettings settings;
  settings.beginWriteArray("downloadHistory");
  for (int i = 0; i < history.size(); ++i) {
    settings.setArrayIndex(i);
    settings.setValue("fileName", history[i].fileName);
    settings.setValue("url", history[i].url);
    settings.setValue("timestamp", history[i].timestamp);
    settings.setValue("success", history[i].success);
  }
  settings.endArray();
}

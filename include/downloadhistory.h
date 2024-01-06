#ifndef DOWNLOADHISTORY_H
#define DOWNLOADHISTORY_H

#include <QList>
#include <QString>
#include <QDateTime>

struct DownloadRecord {
  QString fileName;
  QString url;
  QDateTime timestamp;
  bool success;
};

class DownloadHistory {
 public:
  void addRecord(const DownloadRecord &record);
  QList<DownloadRecord> getHistory() const;
  void loadFromSettings();
  void saveToSettings();

 private:
  QList<DownloadRecord> history;
};

#endif // DOWNLOADHISTORY_H

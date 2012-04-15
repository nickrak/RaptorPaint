#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QStringList>
#include <QImage>
#include <QMap>

class HistoryManager
{
public:
    HistoryManager(QImage* image);

    QStringList listSnapshots();
    void makeSnapshot(QString name);
    void rollbackToSnapshot(QString name);

private:
    QImage* base;
    QMap<QString, QImage> snapshots;
};

#endif // HISTORYMANAGER_H

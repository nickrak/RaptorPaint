#include "historymanager.h"

HistoryManager::HistoryManager(QImage* image)
{
    this->base = image;
}

QStringList HistoryManager::listSnapshots()
{
    QStringList items;
    foreach (QString k, this->snapshots.keys())
    {
        items << k;
    }
    return items;
}

void HistoryManager::makeSnapshot(QString name)
{
    this->snapshots[name] = *this->base;
}

void HistoryManager::rollbackToSnapshot(QString name)
{
    *this->base = this->snapshots[name];
}

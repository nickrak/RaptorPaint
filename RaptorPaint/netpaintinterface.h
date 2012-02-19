#ifndef NETPAINTINTERFACE_H
#define NETPAINTINTERFACE_H

#include <QList>
#include <QImage>

class NetPaintInterface
{
public:
    NetPaintInterface();
    ~NetPaintInterface();
    bool setUsername(QString name);
    bool isParticipant(QString name);
    QList<QString> getParticipantList();

public slots:
    void updateImage(QImage image);
    void sendTextMessage(QString message);

signals:
    void updatedParticipantList(QList<QString> participants);
    void gotUpdatedLayer(QString layerName, QImage image);
    void gotTextMessage(QString participant, QString message);
};

#endif // NETPAINTINTERFACE_H

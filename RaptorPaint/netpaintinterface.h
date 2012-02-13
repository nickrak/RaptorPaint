#ifndef NETPAINTINTERFACE_H
#define NETPAINTINTERFACE_H

#include <QList>
#include <QImage>

class NetPaintInterface
{
public:
    virtual bool setUsername(QString name) = 0;
    virtual bool isParticipant(QString name) = 0;
    virtual QList<QString> getParticipantList() = 0;

public slots:
    virtual void updateImage(QImage image) = 0;
    virtual void sendTextMessage(QString message) = 0;

signals:
    void updatedParticipantList(QList<QString> participants);
    void gotUpdatedLayer(QString layerName, QImage image);
    void gotTextMessage(QString participant, QString message);
};

#endif // NETPAINTINTERFACE_H

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QString>
#include <QQueue>
#include <QByteArray>
#include <QThread>
#include <QMap>
#include <QImage>
#include <QMutex>

class ConnectionManager : public QThread
{
    Q_OBJECT
public:
  ConnectionManager();
  void openConnectionWindow();
  void disconnect();
  void sendTextMessage(QString message);
  bool toggleMute(QString name);
  QString getName();

private slots:
  void connectionWindowResponce(QString username, QString hostname);

private:
  void run();
  QMap<QString, QImage> layers;
  QMap<QString, bool> mutes;
  QMutex txtQueue;
  QQueue<QString> outboundMessages;
  QTcpSocket socket;
  bool keepAlive;
  QString name;


signals:
  void gotTextMessage(QString message);
  void gotImageUpdate();
  void userJoined(QString user);
  void userLeft(QString user);

public slots:
    
};

#endif // CONNECTIONMANAGER_H

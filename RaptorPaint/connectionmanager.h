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

class ConnectionManager : public QThread
{
    Q_OBJECT
public:
  ConnectionManager();
  void openConnectionWindow();
  void disconnect();
  void sendTextMessage(QString message);
  void setMute(QString name, bool mute);
  QString getName();

private slots:
  void connectionWindowResponce(QString username, QString hostname);

private:
  void run();
  QMap<QString, QImage> layers;
  QMap<QString, bool> mutes;
  QTcpSocket socket;
  bool keepAlive;
  QString name;


signals:
  void gotTextMessage(QString message);
  void gotImageUpdate();
public slots:
    
};

#endif // CONNECTIONMANAGER_H

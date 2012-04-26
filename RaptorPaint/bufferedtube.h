// Raptor Paint
// Copyright (C) 2012 Nick Rakoczy, Jessica Randall
//
// This program is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.

#ifndef BUFFEREDTUBE_H
#define BUFFEREDTUBE_H

#include <QTcpSocket>
#include <QDataStream>
#include <QBuffer>
#include <QImage>
#include <QMutex>
#include <QByteArray>

class BufferedTube : public QObject
{
    Q_OBJECT
public:
    BufferedTube(QTcpSocket* socket);

    void sendBuffer(QBuffer &buffer);

private slots:
    void readyRead();

signals:
    void gotBuffer(QByteArray buffer);

private:
    QTcpSocket* sock;
    QDataStream ds;
    QMutex m_r;
    QMutex m_w;
};

#endif // BUFFEREDTUBE_H

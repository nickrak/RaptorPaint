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

#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H
#include <QMainWindow>
#include <QKeyEvent>

namespace Ui {
class ConnectionWindow;
}

class ConnectionWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConnectionWindow(QWidget *parent = 0);
    ~ConnectionWindow();

signals:
    void connectionReady(QString username, QString hostname);

private slots:
    void cancelClicked();
    void connectClicked();

private:
    Ui::ConnectionWindow *ui;

protected:
    void keyPressEvent(QKeyEvent *);
};

#endif // CONNECTIONWINDOW_H

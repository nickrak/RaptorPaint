#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H
#include <QMainWindow>

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
};

#endif // CONNECTIONWINDOW_H

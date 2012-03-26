#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "connectionwindow.h"

#include <QDebug>
#include <QRgb>
#include <iostream>

#include <qmath.h>

#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QPainter>
#include <QInputDialog>
#include <QLineEdit>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cm(new ConnectionManager),
    muted(QIcon(":/userIcons/muted.png")),
    unmuted(QIcon(":/userIcons/unmuted.png")),
    qcd(this)
{
    ui->setupUi(this);

    this->cm->myImage()->fill(0x00FF0000);

    this->connect(this->cm, SIGNAL(gotTextMessage(QString)), this, SLOT(gotTextMessage(QString)));
    this->connect(ui->actionConnect_Host, SIGNAL(triggered()), this, SLOT(mnuConnect()));
    this->connect(ui->input, SIGNAL(returnPressed()), this, SLOT(txtInputReturnPressed()));
    this->connect(ui->actionSave, SIGNAL(triggered()),this, SLOT(saveToFile()));

    this->connect(this->cm, SIGNAL(userJoined(QString)), this, SLOT(userJoined(QString)));
    this->connect(this->cm, SIGNAL(userLeft(QString)), this, SLOT(userLeft(QString)));

    this->connect(ui->userList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(userMuteToggle(QListWidgetItem*)));

    this->connect(ui->paintArea, SIGNAL(mouseRelease()), this, SLOT(mouseRelease()));

    ui->mnuIn->setShortcut(QKeySequence("Ctrl+="));
    ui->mnuOut->setShortcut(QKeySequence("Ctrl+-"));
    ui->mnuActual->setShortcut(QKeySequence("Ctrl+0"));
    ui->actionSave->setShortcut(QKeySequence("Ctrl+S"));
    ui->actionClose->setShortcut(QKeySequence("Ctrl+W"));
    ui->actionConnect_Host->setShortcut(QKeySequence("Ctrl+N"));

    this->connect(ui->mnuIn, SIGNAL(triggered()), ui->paintArea, SLOT(zoomIn()));
    this->connect(ui->mnuOut, SIGNAL(triggered()), ui->paintArea, SLOT(zoomOut()));
    this->connect(ui->mnuActual, SIGNAL(triggered()), ui->paintArea, SLOT(zoomActual()));

    this->connect(ui->paintbrush,SIGNAL(clicked()), this, SLOT(setToBrush()));
    this->connect(ui->pencil, SIGNAL(clicked()), this, SLOT(setToPencil()));
    this->connect(ui->eraser, SIGNAL(clicked()), this, SLOT(setToEraser()));
    this->connect(ui->type, SIGNAL(clicked()), this, SLOT(setToType()));
    this->connect(ui->color, SIGNAL(clicked()), this, SLOT(openColorPicker()));

    this->connect(&this->qcd, SIGNAL(colorSelected(QColor)), this, SLOT(colorSelected(QColor)));
    this->connect(&this->qcd, SIGNAL(currentColorChanged(QColor)), this, SLOT(colorSelected(QColor)));

    ui->paintArea->setImageStack(this->cm->getLayerPtr());

    this->connect(ui->paintArea, SIGNAL(drawHere(double,double)), this, SLOT(drawHere(double,double)));

    this->wasDragging = false;

    this->selectedTool = BRUSH;
    this->toolSize = 10.0;
    this->colorSelected(QColor::fromRgb(0, 0, 0));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mouseRelease()
{
    this->wasDragging = false;
}

void MainWindow::drawHere(double x, double y)
{
    static double lx, ly;

    QImage* img = this->cm->myImage();
    if (x < 0 || x > WIDTH || y < 0 || y > HEIGHT)
    {
        return;
    }
    if (img->isNull())
    {
        return;
    }

    QPainter painter(img);

    if (wasDragging)
    {
        double cx = x - lx;
        double cy = y - ly;
        double cc = qSqrt(cx * cx + cy * cy); // total distance between where we are now and where we were
        double dx = cx / cc;
        double dy = cy / cc;

        for (double cp = 0; cp < cc; cp++)
        {
            if (changeCanvas(x - dx * cp, y - dy * cp, &painter)) goto exiting;
        }
    }
    else
    {
        this->wasDragging = true;
        if (changeCanvas(x, y, &painter)) goto exiting;
    }

exiting:
    lx = x;
    ly = y;
}

bool MainWindow::changeCanvas(double x, double y, QPainter* painter)
{
    double radius = this->toolSize / 2.0;
    QRectF r(x - radius, y - radius, radius * 2, radius * 2);

    switch (this->selectedTool)
    {
    case BRUSH:
        //painter->setRenderHints((QPainter::RenderHint) 0xF, true);
        painter->setBrush(QBrush(this->selectedColor));
        painter->setPen(this->selectedColor);
        painter->drawEllipse(r);
        break;
    case PENCIL:
        //painter->setRenderHints((QPainter::RenderHint) 0xF, false);
        painter->setBrush(QBrush(this->selectedColor));
        painter->setPen(this->selectedColor);
        painter->drawEllipse(QRect(x, y, 1, 1));
        break;
    case ERASER:
        //painter->setRenderHints((QPainter::RenderHint) 0xF, false);
        painter->setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter->setBrush(QBrush(QColor::fromRgb(0, 0, 0, 255)));
        painter->setPen(QColor::fromRgb(0, 0, 0, 255));
        painter->drawEllipse(r);
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        break;
    case TYPE:
        bool ok = false;
        QString textToPrint = QInputDialog::getText(this, "Text Tool", "What would you like to type?", QLineEdit::Normal, "", &ok);
        if (ok)
        {
            QFont f("Helvetica", 25);
            f.setStyleStrategy(QFont::NoAntialias);
            painter->setFont(f);
            painter->drawText(x, y, WIDTH - x, HEIGHT - y, 0x1000, textToPrint);
        }
        return true;
    }
    return false;
}

void MainWindow::openColorPicker()
{
    this->qcd.show();
}

void MainWindow::colorSelected(QColor color)
{
    this->selectedColor = color;
    QPixmap pix(ui->color->width() * 0.65, ui->color->height() * 0.65);
    pix.fill(color);
    QIcon icon(pix);
    ui->color->setIcon(icon);
    ui->color->setIconSize(QSize(ui->color->width(), ui->color->height()));
}

void MainWindow::txtInputReturnPressed()
{
    this->cm->sendTextMessage(ui->input->text());
    ui->input->setText("");
}

void MainWindow::userMuteToggle(QListWidgetItem *item)
{
    QString user = item->text();
    bool mute = this->cm->toggleMute(user);
    item->setIcon(mute ? this->muted : this->unmuted);
}

void MainWindow::mnuConnect()
{
    this->cm->openConnectionWindow();
}

void MainWindow::userJoined(QString name)
{
    qDebug("Join");
    qDebug(name.toAscii().data());
    if (this->listItems.contains(name)) return;
    QListWidgetItem* item = new QListWidgetItem(this->unmuted, name, ui->userList);
    this->listItems[name] = item;
}

void MainWindow::userLeft(QString name)
{
    qDebug("Leave");
    qDebug(name.toAscii().data());
    if (this->listItems.contains(name))
    {
        QListWidgetItem* item = this->listItems[name];
        this->listItems.remove(name);
        delete item;
    }
}

// Got message from server
void MainWindow::gotTextMessage(QString msg)
{
    if(msg.startsWith(QString("[%1]").arg(this->cm->getName())))
    {
        msg = msg.prepend("<p class=\"me\">").append("</p>");
    }
    ui->chatLog->append(msg);
}

//Save File
void MainWindow::saveToFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", QDir::homePath(), "Image Files (*.png *.jpg);; All Files (*)");
    QImage saveImage(WIDTH,HEIGHT,QImage::Format_ARGB32_Premultiplied);
    ImageStack* layers = this->cm->getLayerPtr();
    {
        QPainter totalImage(&saveImage);
        totalImage.fillRect(0,0,WIDTH,HEIGHT,QColor::fromRgb(255,255,255));
        foreach(QString userName, layers->keys())
        {
            totalImage.drawImage(0,0,(*(*layers)[userName]));
        }
    }
    saveImage.save(fileName);
}

inline void MainWindow::resetToolbox()
{
    ui->type->setChecked(false);
    ui->paintbrush->setChecked(false);
    ui->eraser->setChecked(false);
    ui->pencil->setChecked(false);
}

void MainWindow::setToBrush()
{
    this->resetToolbox();
    this->selectedTool = BRUSH;
    ui->paintbrush->setChecked(true);
}

void MainWindow::setToPencil()
{
    this->resetToolbox();
    this->selectedTool = PENCIL;
    ui->pencil->setChecked(true);
}

void MainWindow::setToEraser()
{
    this->resetToolbox();
    this->selectedTool = ERASER;
    ui->eraser->setChecked(true);
}

void MainWindow::setToType()
{
    this->resetToolbox();
    this->selectedTool = TYPE;
    ui->type->setChecked(true);
}

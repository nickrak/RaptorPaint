#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "connectionwindow.h"


#include <iostream>
#include <qmath.h>

#include <QDebug>
#include <QRgb>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QPainter>
#include <QInputDialog>
#include <QLineEdit>
#include <QColorDialog>
#include <QBitmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cm(new ConnectionManager),
    muted(QIcon(":/userIcons/muted.png")),
    unmuted(QIcon(":/userIcons/unmuted.png")),
    qcd(this), manager(HistoryManager(cm->myImage()))
{
    ui->setupUi(this);

    this->cm->myImage()->fill(0x00FF0000);

    this->connect(this->cm, SIGNAL(gotTextMessage(QString)), this, SLOT(gotTextMessage(QString)));
    this->connect(ui->actionConnect_Host, SIGNAL(triggered()), this, SLOT(mnuConnect()));
    this->connect(ui->input, SIGNAL(returnPressed()), this, SLOT(txtInputReturnPressed()));
    this->connect(ui->actionSave, SIGNAL(triggered()),this, SLOT(saveToFile()));
    this->connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(clearCanvas()));
    this->connect(ui->actionIncrease_Size, SIGNAL(triggered()), this, SLOT(increaseBrush()));
    this->connect(ui->actionDecrease_Size, SIGNAL(triggered()), this, SLOT(decreaseBrush()));
    this->connect(ui->actionDefault_Size, SIGNAL(triggered()), this, SLOT(resetBrush()));

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
    ui->actionIncrease_Size->setShortcut(QKeySequence("Ctrl+]"));
    ui->actionDecrease_Size->setShortcut(QKeySequence("Ctrl+["));
    ui->actionUndo->setShortcut(QKeySequence("Ctrl+z"));
    ui->actionRedo->setShortcut(QKeySequence("Ctrl+shift+z"));

    this->connect(ui->cmbBlendingModes, SIGNAL(currentIndexChanged(int)), this, SLOT(blendingModeChanged(int)));

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

    this->blendMode = QPainter::CompositionMode_SourceOver;
}

void MainWindow::initialize()
{
    this->resetBrush();
    this->colorSelected(QColor::fromRgb(0, 0, 0));
    this->setToBrush();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mouseRelease()
{
    this->wasDragging = false;
    if (this->cm != 0)
    {
        this->cm->sendImageUpdate();
    }
}

void MainWindow::drawHere(double x, double y)
{
    static double lx, ly;

    QImage* img = this->cm->myImage();
    if (x < 0 || x > WIDTH || y < 0 || y > HEIGHT)
    {
        this->wasDragging = false;
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

    painter->setCompositionMode(this->blendMode);

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
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        this->cm->sendImageUpdate();
        return true;
    }
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
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
    this->setPaintCursor();
}

void MainWindow::setToPencil()
{
    this->resetToolbox();
    this->selectedTool = PENCIL;
    ui->pencil->setChecked(true);
    this->setPaintCursor();
}

void MainWindow::setToEraser()
{
    this->resetToolbox();
    this->selectedTool = ERASER;
    ui->eraser->setChecked(true);
    this->setPaintCursor();
}

void MainWindow::setToType()
{
    this->resetToolbox();
    this->selectedTool = TYPE;
    ui->type->setChecked(true);
    this->setPaintCursor();
}

void MainWindow::increaseBrush()
{
    toolSize *= 1.25;
    this->setPaintCursor();
}

void MainWindow::decreaseBrush()
{
    toolSize *= 0.75;
    this->setPaintCursor();
}

void MainWindow::resetBrush()
{
    toolSize = 10;
    this->setPaintCursor();
}

void MainWindow::setPaintCursor()
{
    switch (this->selectedTool)
    {
    case BRUSH:
    case ERASER:
        this->setPaintCursorSize(this->toolSize);
        break;
    case TYPE:
        ui->paintArea->setCursor(Qt::IBeamCursor);
        break;
    case PENCIL:
        ui->paintArea->setCursor(Qt::CrossCursor);
        break;
    }
}

void MainWindow::setPaintCursorSize(double size)
{
    if (size > 0)
    {
        double rad = size / 2.0;
        QPixmap pix(size, size);
        pix.fill(QColor::fromRgb(0, 0, 0, 0));

        QPainter p(&pix);
        QPen pen(QColor::fromRgb(128, 128, 128, 128));
        p.setPen(pen);
        p.setBrush(QBrush(QColor::fromRgb(0, 0, 0, 0)));
        p.drawEllipse(0, 0, size - 1, size - 1);

        this->currentSizeCursor = QCursor(pix, rad, rad);
        ui->paintArea->setCursor(this->currentSizeCursor);
    }
}

void MainWindow::clearCanvas()
{
    QPainter p(this->cm->myImage());
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.fillRect(0,0,WIDTH,HEIGHT,Qt::SolidPattern);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    ui->paintArea->repaint();
}

void MainWindow::blendingModeChanged(int index)
{
    switch (index)
    {
    case 0: //Normal
        this->blendMode =  QPainter::CompositionMode_SourceOver;
        break;
    case 1: //Darken
        this->blendMode = QPainter::CompositionMode_Darken;
        break;
    case 2: //Multiply
        this->blendMode = QPainter::CompositionMode_Multiply;
        break;
    case 3: //Lighten
        this->blendMode = QPainter::CompositionMode_Lighten;
        break;
    case 4: //Screen
        this->blendMode = QPainter::CompositionMode_Screen;
        break;
    case 5: //Overlay
        this->blendMode = QPainter::CompositionMode_Overlay;
        break;
    case 6: //Soft Light
        this->blendMode = QPainter::CompositionMode_SoftLight;
        break;
    case 7: //Hard Light
        this->blendMode = QPainter::CompositionMode_HardLight;
        break;
    case 8: //Difference
        this->blendMode = QPainter::CompositionMode_Difference;
        break;
    case 9: //Exclustion
        this->blendMode = QPainter::CompositionMode_Exclusion;
        break;
    default:    //Back to normal something went wrong.
        ui->cmbBlendingModes->setCurrentIndex(0);
        break;
    }
}

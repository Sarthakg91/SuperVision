/*
* This class creates a Qt GUI for displaying ART tracker data.
* (c) Eric Lecolinet - Telecom ParisTech 2014.
*/

#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "DTracker.h"
#include <iostream>
#include <QtGui>
#include <QTextEdit>
#include <QLineEdit>
#include <QFileDialog>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QTextStream>
#include <QCloseEvent>
#include<QPainter>
#include<qpalette.h>
using namespace std;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const char* DEFAULT_HOST = "pixls.enst.fr";
const int DEFAULT_PORT = 5000;
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 500;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

MainWindow::~MainWindow() {
    delete ui;
    delete dtracker;
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow),
    textEdit(NULL), trackerAct(NULL), serverAct(NULL),
    hostDialog(NULL),
    hostname(DEFAULT_HOST),
    port(DEFAULT_PORT),
    dtracker(NULL),
    trackerState(DTracker::Disconnected)
{
    //const char* c_hostname = hostname.toLocal8Bit();
    dtracker = new DTracker(hostname.toStdString(), port, port);

    connect(dtracker, SIGNAL(onStateChange(int)), this, SLOT(showState(int)));
    connect(dtracker, SIGNAL(onMessage(const QString&)),
            this, SLOT(showMessage(const QString&)));

    //----------------------------------------------------------
    w=new Widget();


    QPalette Pal(palette());
    // set black background
    Pal.setColor(QPalette::Background, Qt::black);
    w->setAutoFillBackground(true);
   w->setPalette(Pal);
w->setWindowState(Qt::WindowFullScreen);
   w->show();
    connect(dtracker, SIGNAL(bodyTracked(QPointF)), w, SLOT(updateCenter(QPointF)));
    connect(dtracker, SIGNAL(flyStickButtonReleased(QPointF)), w, SLOT(selectElement(QPointF)));



    createGUI();


}


void MainWindow::createGUI() {
    // ui->setupUi(this);

    hostDialog = new HostDialog(this);
//------------------------------------
//    widget=new QWidget();
//QSlider *slider=new QSlider();
//slider->setMaximum(800);
//slider->setMinimum(0);
//slider->setValue(0);
//slider->show();
//widget->show();

//-----------------------------------------
    this->setMinimumSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    setCentralWidget(textEdit);

    QMenuBar* menuBar = this->menuBar();
    QToolBar* toolBar = this->addToolBar(tr("File"));
    /*QStatusBar* statusBar = */ this->statusBar();

    QMenu* fileMenu = menuBar->addMenu(tr("File"));
    QMenu* trackerMenu = menuBar->addMenu(tr("Tracker"));
    QAction* act = 0;

    act = toolBar->addAction(QIcon(":open.png"), tr("&Open..."), this, SLOT(openFile()));
    act->setShortcut(tr("Ctrl+O"));
    act->setStatusTip(tr("Open file"));
    fileMenu->addAction(act);

    act = toolBar->addAction(QIcon(":save.png"), tr("&Save..."), this, SLOT(saveFile()));
    act->setShortcut(tr("Ctrl+S"));
    act->setStatusTip(tr("Save File"));
    fileMenu->addAction(act);

    act = toolBar->addAction(QIcon(":quit.png"), tr("&Quit..."), this, SLOT(quitApp()));
    act->setShortcut(tr("Ctrl+Q"));
    act->setStatusTip(tr("Quit Application"));
    fileMenu->addAction(act);

    act = toolBar->addAction(tr("Clean"), this, SLOT(cleanText()));
    act->setShortcut(tr("Ctrl+C"));
    act->setStatusTip(tr("Clean Console"));

    act = toolBar->addAction(tr(" + "), textEdit, SLOT(zoomIn()));
    act->setShortcut(tr("Ctrl++"));
    act->setStatusTip(tr("Zoom Text"));

    act = toolBar->addAction(tr(" - "), textEdit, SLOT(zoomOut()));
    act->setShortcut(tr("Ctrl+-"));
    act->setStatusTip(tr("Dezoom Text"));

    trackerAct = toolBar->addAction(tr("Start Tracker"), this, SLOT(startStopTracker()));
    trackerAct->setShortcut(tr("Ctrl+T"));
    trackerAct->setStatusTip(tr("Start/Stop ART Tracker"));
    trackerMenu->addAction(trackerAct);

    serverAct = toolBar->addAction("Host", this, SLOT(openHostDialog()));
    serverAct->setStatusTip(tr("ART Tracker Host"));
    trackerMenu->addAction(serverAct);
    updateHostAddress();
    hostDialog->hostField->setText(hostname);
    hostDialog->portField->setText(QString::number(port));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

HostDialog::HostDialog(QWidget *parent) : QDialog(parent) {
    hostField = new QLineEdit();
    portField = new QLineEdit();

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("Host:"), hostField);
    formLayout->addRow(tr("Port:"), portField);

    QDialogButtonBox*
            buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                             | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addLayout(formLayout);
    vLayout->addWidget(buttonBox);
    this->setLayout(vLayout);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void MainWindow::openHostDialog() {
    int status = hostDialog->exec();
    if (status == QDialog::Accepted) {
        hostname = hostDialog->hostField->text();
        port = hostDialog->portField->text().toInt();
        updateHostAddress();
    }
}


void MainWindow::updateHostAddress() {
    QString hostAddress = QString("Host: %1:%2").arg(hostname).arg(port);
    serverAct->setText(hostAddress);
    dtracker->setHost(hostname.toStdString(), port, port);
}


void MainWindow::startStopTracker() {
    if (trackerState == DTracker::Disconnected) dtracker->start();
    else if (trackerState == DTracker::Running) dtracker->stop();
}


void MainWindow::showState(int state) {
    trackerState = state;
    switch(state) {
    case DTracker::Disconnected:
        trackerAct->setText("Start Tracker");
        break;
    case DTracker::Connecting:
        trackerAct->setText("Connecting...");
        break;
    case DTracker::Disconnecting:
        trackerAct->setText("Disconnecting...");
        break;
    case DTracker::Running:
        trackerAct->setText("Stop Tracker");
        break;
    }
}

void MainWindow::showMessage(const QString& text) {
    textEdit->append(text);
}

void MainWindow::cleanText() {
    textEdit->setPlainText("");
}


void MainWindow::quitApp() {
    int ret =
    QMessageBox::critical(this, tr("Quit Application"),
                          tr("The document has been modified.\n""Do you want to quit?"),
                          QMessageBox::Yes | QMessageBox::No);
    //cout << ret << endl;
    if (ret == QMessageBox::Yes) {

        w->file->close();
        w->player.Stop();
        QApplication::quit();
    }
}


void MainWindow::closeEvent(QCloseEvent *event) {
    event->ignore();
    quitApp();
}


void MainWindow::openFile() {
    QString fileName =
            QFileDialog::getOpenFileName (this,
                                          tr("Open Text"),
                                          "",
                                          tr("Text Files (*.txt *.cpp *.h *.html)")
                                          );
    //cout << qPrintable(fileName) << endl;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        //textEdit->setHtml(in.readAll());
        textEdit->setPlainText(in.readAll());
        file.close();
    }
}


void MainWindow::saveFile() {
    QString fileName =
            QFileDialog::getSaveFileName (this,
                                          tr("SaveText"),
                                          "",
                                          tr("Text Files (*.txt *.cpp *.h *.html)")
                                          );
    //cout << qPrintable(fileName) << endl;
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out.setCodec("UTF-8"); // ISO 8859-1", "UTF-8", and "UTF-16"
        //out << textEdit->toHtml();
        out << textEdit->toPlainText();
        file.close();
    }
}
//--------------------------------------------------------------


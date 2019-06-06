/*
* This class creates a Qt GUI for displaying ART tracker data.
* (c) Eric Lecolinet - Telecom ParisTech 2014.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include"widget.h"


namespace Ui {
class MainWindow;
}

/**
* This class creates a Qt GUI for displaying ART tracker data.
*/
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
//-------------------------------------------------
   // void paintEvent(QPaintEvent *);
//-------------------------------------------------
public slots:
    void showState(int type);
    void showMessage(const QString& text);

private slots:
    void openFile();
    void saveFile();
    void quitApp();
    void cleanText();
    void startStopTracker();
    void openHostDialog();

private:
    Ui::MainWindow *ui;
    class QTextEdit *textEdit;
    class QAction *trackerAct, *serverAct;
    class HostDialog* hostDialog;
    QString hostname;
    int port;
    class DTracker *dtracker;
    int trackerState;
//---------------------
   Widget *w;
//----------------------------
    void createGUI();
    void updateHostAddress();
    void closeEvent(QCloseEvent*);
};


class HostDialog : public QDialog {
    Q_OBJECT
public:
    HostDialog(QWidget *parent = 0);
    class QLineEdit *hostField;
    class QLineEdit *portField;
};


#endif // MAINWINDOW_H

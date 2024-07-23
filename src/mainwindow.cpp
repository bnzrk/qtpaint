#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_session = new SessionManager(this);

    ui->viewport->setSessionManager(m_session);
    ui->layerPanel->setSessionManager(m_session);
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_session = new SessionManager(this);

    m_sourceImage = new LayerImage();

    ui->viewport->setSourceImage(m_sourceImage);
    ui->layerPanel->setSourceImage(m_sourceImage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

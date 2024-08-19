#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_canvas = new Canvas(800, 600, this);

    ui->viewport->setCanvas(m_canvas);
    ui->rightPanel->init(m_canvas);
}

MainWindow::~MainWindow()
{
    delete ui;
}

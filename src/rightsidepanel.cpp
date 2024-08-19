#include "rightsidepanel.h"
#include "ui_rightsidepanel.h"

RightSidePanel::RightSidePanel(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::RightSidePanel)
{
    ui->setupUi(this);
    ui->verticalSplitter->setStretchFactor(0, 3);
    ui->verticalSplitter->setStretchFactor(1, 5);
}

RightSidePanel::~RightSidePanel()
{
    delete ui;
}

void RightSidePanel::init(Canvas *canvas)
{
    ui->layerPanel->setCanvas(canvas);
}

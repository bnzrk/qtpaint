#include "layerpanelitem.h"
#include "ui_layerpanelitem.h"

LayerPanelItem::LayerPanelItem(QString displayName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayerPanelItem)
{
    ui->setupUi(this);

    QObject::connect(ui->visibleToggle, &QPushButton::toggled, this, &LayerPanelItem::onVisibleButtonToggled);
    QObject::connect(ui->upButton, &QPushButton::clicked, this, &LayerPanelItem::onUpButtonClicked);
    QObject::connect(ui->downButton, &QPushButton::clicked, this, &LayerPanelItem::onDownButtonClicked);

    setDisplayName(displayName);
    ui->draggableRegion->setStyleSheet(QString(""));
}

LayerPanelItem::LayerPanelItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayerPanelItem)
{
    ui->setupUi(this);

    QObject::connect(ui->visibleToggle, &QPushButton::toggled, this, &LayerPanelItem::onVisibleButtonToggled);
    QObject::connect(ui->upButton, &QPushButton::clicked, this, &LayerPanelItem::onUpButtonClicked);
    QObject::connect(ui->downButton, &QPushButton::clicked, this, &LayerPanelItem::onDownButtonClicked);

    setDisplayName("");
    deselect();
}

LayerPanelItem::~LayerPanelItem()
{
    delete ui;
}

void LayerPanelItem::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
}

void LayerPanelItem::select()
{
    ui->draggableRegion->setStyleSheet(SELECTED_STYLE);
}

void LayerPanelItem::deselect()
{
    ui->draggableRegion->setStyleSheet(UNSELECTED_STYLE);
}

void LayerPanelItem::onVisibleButtonToggled()
{
    emit visibleButtonToggled();
}

void LayerPanelItem::onUpButtonClicked()
{
    emit upButtonClicked();
}

void LayerPanelItem::onDownButtonClicked()
{
    emit downButtonClicked();
}

QString LayerPanelItem::getDisplayName()
{
    return ui->nameText->text();
}

void LayerPanelItem::setDisplayName(QString displayName)
{
    ui->nameText->setText(displayName);
}

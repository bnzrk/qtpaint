#include "layerpanel.h"
#include "ui_layerpanel.h"

LayerPanel::LayerPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LayerPanel)
{
    ui->setupUi(this);
    m_items = QVector<LayerPanelItem*>();
}

LayerPanel::~LayerPanel()
{
    delete ui;
}

void LayerPanel::onVisibilityToggled(LayerPanelItem* sender)
{
    int senderIndex = getIndexOf(sender);
    toggleLayerVisibility(senderIndex);
}

void LayerPanel::onMoveUpRequested(LayerPanelItem* sender)
{
    int senderIndex = getIndexOf(sender);
    if (senderIndex < 1)
        return;
    swapLayers(senderIndex, senderIndex - 1);
    updateItemDisplay(senderIndex);
    updateItemDisplay(senderIndex - 1);
}

void LayerPanel::onMoveDownRequested(LayerPanelItem* sender)
{
    int senderIndex = getIndexOf(sender);
    if (senderIndex >= m_sourceImage->getLayerCount() - 1)
        return;
    swapLayers(senderIndex, senderIndex + 1);
    updateItemDisplay(senderIndex);
    updateItemDisplay(senderIndex + 1);
}

void LayerPanel::updateItemDisplay(int index)
{
    Layer* reference = m_sourceImage->getLayerAtIndex(index);
    LayerPanelItem* item = m_items[index];
    item->setDisplayName(reference->getName());
}

void LayerPanel::toggleLayerVisibility(int index)
{
    m_sourceImage->getLayerAtIndex(index)->toggleVisible();
}

void LayerPanel::swapLayers(int a, int b)
{
    m_sourceImage->swapLayers(a, b);
}

void LayerPanel::addItem(const Layer& reference)
{
    LayerPanelItem* item = new LayerPanelItem(reference.getName());
    m_items.push_back(item);

    QObject::connect(item, &LayerPanelItem::upButtonClicked, this, &LayerPanel::onMoveUpRequested);
    QObject::connect(item, &LayerPanelItem::downButtonClicked, this, &LayerPanel::onMoveDownRequested);
    QObject::connect(item, &LayerPanelItem::visibleButtonToggled, this, &LayerPanel::onVisibilityToggled);

    ui->verticalLayout->addWidget(item);
}

int LayerPanel::getIndexOf(const LayerPanelItem* item) const
{
    for (int i = 0; i < m_items.size(); i++)
    {
        if (item == m_items[i])
            return i;
    }
    return -1;
}

void LayerPanel::setSourceImage(LayerImage *sourceImage)
{
    m_sourceImage = sourceImage;
    populateItems();
}

void LayerPanel::populateItems()
{
    for (int i = 0; i < m_sourceImage->getLayerCount(); i++)
    {
        addItem(*m_sourceImage->getLayerAtIndex(i));
    }
    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->verticalLayout->addSpacerItem(spacer);
}

#include "layerpanel.h"
#include "ui_layerpanel.h"

LayerPanel::LayerPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LayerPanel)
{
    ui->setupUi(this);
    m_items = QVector<LayerPanelItem*>();

    QObject::connect(ui->newLayerButton, &QPushButton::clicked, this, &LayerPanel::onNewLayerClicked);
    QObject::connect(ui->removeLayerButton, &QPushButton::clicked, this, &LayerPanel::onRemoveLayerClicked);
}

LayerPanel::~LayerPanel()
{
    delete ui;
}

void LayerPanel::setCanvas(Canvas* canvas)
{
    m_canvas = canvas;

    if (!m_canvas)
    {
        cleanup();
        return;
    }

    populateItems();
    ui->newLayerButton->setEnabled(true);
}

void LayerPanel::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    ui->scrollArea->setMinimumWidth(ui->scrollArea->verticalScrollBar()->sizeHint().width() + 142);
    setMinimumWidth(ui->scrollArea->minimumWidth() + ui->horizontalSpacer1->minimumSize().width() + ui->horizontalSpacer2->minimumSize().width());
}

void LayerPanel::cleanup()
{
    m_items.clear();
    while(auto item = ui->itemLayout->takeAt(0))
    {
        delete item->widget();
    }
    ui->newLayerButton->setEnabled(false);
    ui->removeLayerButton->setEnabled(false);
}

void LayerPanel::onNewLayerClicked()
{
    if (m_canvas->layerCount() < 2)
    {
        ui->removeLayerButton->setEnabled(true);
    }

    int index = m_canvas->activeLayer() + 1;
    m_items[m_canvas->activeLayer()]->deselect();
    m_canvas->setActiveLayer(index);
    m_canvas->createNewLayer(index);
    addItem(*m_canvas->layerAt(index));
    m_items[index]->select();
}

void LayerPanel::onRemoveLayerClicked()
{
    int index = m_canvas->activeLayer();
    int newActive = index;

    if (index == m_canvas->layerCount() - 1)
        newActive = index - 1;

    m_canvas->setActiveLayer(newActive);

    m_canvas->removeLayer(index);
    removeItem(index);

    m_items[newActive]->select();

    if (m_canvas->layerCount() < 2)
    {
        ui->removeLayerButton->setEnabled(false);
    }
}

void LayerPanel::onVisibilityToggled(LayerPanelItem* sender)
{
    int senderIndex = getIndexOf(sender);
    toggleLayerVisibility(senderIndex);
}

void LayerPanel::onMoveUpRequested(LayerPanelItem* sender)
{
    int senderIndex = getIndexOf(sender);
    if (senderIndex >= m_canvas->layerCount() - 1)
        return;
    swapLayers(senderIndex, senderIndex + 1);
}

void LayerPanel::onMoveDownRequested(LayerPanelItem* sender)
{
    int senderIndex = getIndexOf(sender);
    if (senderIndex < 1)
        return;
    swapLayers(senderIndex, senderIndex - 1);
}

void LayerPanel::onItemClicked(LayerPanelItem* sender)
{
    int senderIndex = getIndexOf(sender);
    if (m_canvas->activeLayer() != senderIndex)
        m_items[m_canvas->activeLayer()]->deselect();
    m_canvas->setActiveLayer(senderIndex);
    m_items[senderIndex]->select();
}

void LayerPanel::updateItemDisplay(int index)
{
    Layer* reference = m_canvas->layerAt(index);
    LayerPanelItem* item = m_items[index];
    item->setDisplayName(reference->name());
    if (m_canvas->activeLayer() == index)
        m_items[index]->select();
    else
        m_items[index]->deselect();
}

void LayerPanel::toggleLayerVisibility(int index)
{
    m_canvas->layerAt(index)->toggleVisible();
}

void LayerPanel::swapLayers(int a, int b)
{
    if (m_canvas->activeLayer() == a)
        m_canvas->setActiveLayer(b);
    else if (m_canvas->activeLayer() == b)
        m_canvas->setActiveLayer(a);

    m_canvas->swapLayers(a, b);
    updateItemDisplay(a);
    updateItemDisplay(b);
}

void LayerPanel::addItem(const Layer& reference)
{
    LayerPanelItem* item = new LayerPanelItem(reference.name(), this);
    m_items.push_back(item);

    QObject::connect(item, &LayerPanelItem::upButtonClicked, this, &LayerPanel::onMoveUpRequested);
    QObject::connect(item, &LayerPanelItem::downButtonClicked, this, &LayerPanel::onMoveDownRequested);
    QObject::connect(item, &LayerPanelItem::visibleButtonToggled, this, &LayerPanel::onVisibilityToggled);
    QObject::connect(item, &LayerPanelItem::clicked, this, &LayerPanel::onItemClicked);

    ui->itemLayout->insertWidget(0, item);
}

void LayerPanel::removeItem(int index)
{
    auto item = m_items.takeAt(index);
    ui->itemLayout->removeWidget(item);
    delete item;
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

void LayerPanel::populateItems()
{
    for (int i = 0; i < m_canvas->layerCount(); i++)
    {
        addItem(*(m_canvas->layerAt(i)));
    }

    m_canvas->setActiveLayer(0);
    m_items[0]->select();
}

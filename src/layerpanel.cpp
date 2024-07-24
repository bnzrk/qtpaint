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

void LayerPanel::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    ui->scrollArea->setMinimumWidth(ui->scrollArea->verticalScrollBar()->sizeHint().width() + 142);
    setMinimumWidth(ui->scrollArea->minimumWidth() + ui->horizontalSpacer1->minimumSize().width() + ui->horizontalSpacer2->minimumSize().width());
}

void LayerPanel::onSessionDeleted()
{
    m_session = nullptr;
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
    if (sourceImage()->layerCount() < 2)
    {
        ui->removeLayerButton->setEnabled(true);
    }

    int index = m_session->activeLayer() + 1;
    m_items[m_session->activeLayer()]->deselect();
    m_session->setActiveLayer(index);
    sourceImage()->createNewLayer(index);
    addItem(*sourceImage()->layerAtIndex(index));
    m_items[index]->select();
}

void LayerPanel::onRemoveLayerClicked()
{
    int index = m_session->activeLayer();
    int newActive = index;

    if (index == sourceImage()->layerCount() - 1)
        newActive = index - 1;

    m_session->setActiveLayer(newActive);

    sourceImage()->removeLayer(index);
    removeItem(index);

    m_items[newActive]->select();

    if (sourceImage()->layerCount() < 2)
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
    if (senderIndex >= sourceImage()->layerCount() - 1)
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
    if (m_session->activeLayer() != senderIndex)
        m_items[m_session->activeLayer()]->deselect();
    m_session->setActiveLayer(senderIndex);
    m_items[senderIndex]->select();
}

void LayerPanel::setSessionManager(SessionManager* session)
{
    m_session = session;
    QObject::connect(m_session, &SessionManager::sessionDeleted, this, &LayerPanel::onSessionDeleted);
    populateItems();
    ui->newLayerButton->setEnabled(true);
}

void LayerPanel::updateItemDisplay(int index)
{
    Layer* reference = sourceImage()->layerAtIndex(index);
    LayerPanelItem* item = m_items[index];
    item->setDisplayName(reference->name());
    if (m_session->activeLayer() == index)
        m_items[index]->select();
    else
        m_items[index]->deselect();
}

void LayerPanel::toggleLayerVisibility(int index)
{
    sourceImage()->layerAtIndex(index)->toggleVisible();
}

void LayerPanel::swapLayers(int a, int b)
{
    if (m_session->activeLayer() == a)
        m_session->setActiveLayer(b);
    else if (m_session->activeLayer() == b)
        m_session->setActiveLayer(a);

    sourceImage()->swapLayers(a, b);
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
    for (int i = 0; i < sourceImage()->layerCount(); i++)
    {
        addItem(*(sourceImage()->layerAtIndex(i)));
    }

    m_session->addSelectedLayer(0);
    m_session->setActiveLayer(0);
    m_items[0]->select();
}

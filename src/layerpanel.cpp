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

void LayerPanel::onSessionDeleted()
{
    m_session = nullptr;
    m_items.clear();
    while(auto item = ui->verticalLayout->takeAt(0))
    {
        if (item->widget())
            delete item->widget();
        else
            delete item->spacerItem();
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
    if (senderIndex < 1)
        return;
    swapLayers(senderIndex, senderIndex - 1);
}

void LayerPanel::onMoveDownRequested(LayerPanelItem* sender)
{
    int senderIndex = getIndexOf(sender);
    if (senderIndex >= sourceImage()->layerCount() - 1)
        return;
    swapLayers(senderIndex, senderIndex + 1);
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

void LayerPanel::populateItems()
{
    for (int i = 0; i < sourceImage()->layerCount(); i++)
    {
        addItem(*(sourceImage()->layerAtIndex(i)));
    }
    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->verticalLayout->addSpacerItem(spacer);

    m_session->addSelectedLayer(0);
    m_session->setActiveLayer(0);
    m_items[0]->select();
}

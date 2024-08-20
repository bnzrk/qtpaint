#include "layerpanel.h"
#include "ui_layerpanel.h"

LayerPanel::LayerPanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LayerPanel)
{
    ui->setupUi(this);

    QFile styleFile(":/layerpanel.qss");
    if(styleFile.open(QIODevice::ReadOnly))
    {
        QString styleSheet = styleFile.readAll();
        styleFile.close();
        setStyleSheet(styleSheet);
    }

    ui->blendModeComboBox->addItems(BLEND_MODE_STRINGS);
    ui->blendModeComboBox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->blendModeComboBox->setFrame(false);

    m_items = QVector<LayerPanelItem*>();

    QObject::connect(ui->newLayerButton, &QPushButton::clicked, this, &LayerPanel::onNewLayerClicked);
    QObject::connect(ui->removeLayerButton, &QPushButton::clicked, this, &LayerPanel::onRemoveLayerClicked);

    QObject::connect(ui->blendModeComboBox, &QComboBox::currentTextChanged, this, &LayerPanel::changeLayerBlendMode);
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

void LayerPanel::changeLayerBlendMode(const QString &text)
{
    Layer* layer = m_canvas->layerAt(m_canvas->activeLayer());
    Layer::BlendMode newBlendMode;
    if (text == "Normal")
    {
        newBlendMode = Layer::BlendMode::Normal;
    }
    else if (text == "Multiply")
    {
        newBlendMode = Layer::BlendMode::Multiply;
    }
    else if (text == "Add")
    {
        newBlendMode = Layer::BlendMode::Add;
    }
    else if (text == "Color Burn")
    {
        newBlendMode = Layer::BlendMode::ColorBurn;
    }
    else if (text == "Color Dodge")
    {
        newBlendMode = Layer::BlendMode::ColorDodge;
    }
    else if (text == "Screen")
    {
        newBlendMode = Layer::BlendMode::Screen;
    }
    else if (text == "Overlay")
    {
        newBlendMode = Layer::BlendMode::Overlay;
    }
    else if (text == "Lighten")
    {
        newBlendMode = Layer::BlendMode::Lighten;
    }
    else if (text == "Darken")
    {
        newBlendMode = Layer::BlendMode::Darken;
    }
    else if (text == "Hard Light")
    {
        newBlendMode = Layer::BlendMode::HardLight;
    }
    else if (text == "Soft Light")
    {
        newBlendMode = Layer::BlendMode::SoftLight;
    }
    else if (text == "Difference")
    {
        newBlendMode = Layer::BlendMode::Difference;
    }
    else if (text == "Exclusion")
    {
        newBlendMode = Layer::BlendMode::Exclusion;
    }
    else if (text == "Xor")
    {
        newBlendMode = Layer::BlendMode::Xor;
    }
    else
    {
        Q_ASSERT_X(false, "changeLayerBlendMode", "Layer blend mode does not exist!");
    }
    layer->setBlendMode(newBlendMode);
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

    Layer::BlendMode activeBlendMode = m_canvas->layerAt(m_canvas->activeLayer())->blendMode();
    ui->blendModeComboBox->setCurrentText(blendModeName(activeBlendMode));
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

    emit m_canvas->canvasImageChanged(index, m_canvas->layerAt(index)->image()->rect());
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

QString LayerPanel::blendModeName(Layer::BlendMode blendMode) const
{
    switch (blendMode)
    {
    case Layer::BlendMode::Normal:
        return BLEND_MODE_STRINGS[0];
    case Layer::BlendMode::Multiply:
        return BLEND_MODE_STRINGS[1];
    case Layer::BlendMode::Add:
        return BLEND_MODE_STRINGS[2];
    case Layer::BlendMode::ColorBurn:
        return BLEND_MODE_STRINGS[3];
    case Layer::BlendMode::ColorDodge:
        return BLEND_MODE_STRINGS[4];
    case Layer::BlendMode::Screen:
        return BLEND_MODE_STRINGS[5];
    case Layer::BlendMode::Overlay:
        return BLEND_MODE_STRINGS[6];
    case Layer::BlendMode::Lighten:
        return BLEND_MODE_STRINGS[7];
    case Layer::BlendMode::Darken:
        return BLEND_MODE_STRINGS[8];
    case Layer::BlendMode::HardLight:
        return BLEND_MODE_STRINGS[9];
    case Layer::BlendMode::SoftLight:
        return BLEND_MODE_STRINGS[10];
    case Layer::BlendMode::Difference:
        return BLEND_MODE_STRINGS[11];
    case Layer::BlendMode::Exclusion:
        return BLEND_MODE_STRINGS[12];
    case Layer::BlendMode::Xor:
        return BLEND_MODE_STRINGS[14];
    default:
        Q_ASSERT_X(false, "blendModeName", "Name not found!");
    }
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

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

    ui->itemLayout->setDirection(QVBoxLayout::BottomToTop);

    m_items = QVector<LayerPanelItem*>();

    QObject::connect(ui->newLayerButton, &QPushButton::clicked, this, &LayerPanel::createNewLayer);
    QObject::connect(ui->removeLayerButton, &QPushButton::clicked, this, &LayerPanel::removeActiveLayer);
    QObject::connect(ui->blendModeComboBox, &QComboBox::currentTextChanged, this, &LayerPanel::changeLayerBlendMode);
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

    connect(m_canvas, &Canvas::activeLayerChanged, this, &LayerPanel::changeActiveItem);
    connect(m_canvas, &Canvas::layerInserted, this, &LayerPanel::insertItem);
    connect(m_canvas, &Canvas::layerRemoved, this, &LayerPanel::removeItem);
    connect(m_canvas, &Canvas::layersSwapped, this, &LayerPanel::swapItems);
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

void LayerPanel::updateItem(int index)
{
    Layer* reference = m_canvas->layerAt(index);
    LayerPanelItem* item = m_items[index];
    item->setDisplayName(reference->name());
    if (m_canvas->activeLayer() == index)
        m_items[index]->select();
    else
        m_items[index]->deselect();
}

void LayerPanel::updateItems()
{
    if (m_items.size() > 0)
        updateItemsInRange(0, m_items.size() - 1);
}

void LayerPanel::updateItemsInRange(int start, int end)
{
    for (int i = start; i <= end; i++)
    {
        updateItem(i);
    }
}

void LayerPanel::swapItems(int a, int b)
{
    updateItem(a);
    updateItem(b);
}

void LayerPanel::insertItem(int index)
{
    LayerPanelItem* item = new LayerPanelItem(this);
    QObject::connect(item, &LayerPanelItem::upButtonClicked, this, &LayerPanel::moveLayerUp);
    QObject::connect(item, &LayerPanelItem::downButtonClicked, this, &LayerPanel::moveLayerDown);
    QObject::connect(item, &LayerPanelItem::visibleButtonToggled, this, &LayerPanel::toggleLayerVisibility);
    QObject::connect(item, &LayerPanelItem::clicked, this, &LayerPanel::changeActiveLayer);

    if (index >= m_items.size())
    {
        m_items.push_back(item);
        ui->itemLayout->insertWidget(-1, item);
    }
    else
    {
        m_items.insert(index, item);
        ui->itemLayout->insertWidget(index, item);
    }

    updateItem(index);
    m_canvas->setActiveLayer(index);
}

void LayerPanel::removeItem(int index)
{
    auto item = m_items.takeAt(index);
    ui->itemLayout->removeWidget(item);
    delete item;

    if (index == m_canvas->activeLayer() && m_items.size() > 0)
    {
        updateItem(index);
    }
}

void LayerPanel::changeActiveItem(int oldIndex, int newIndex)
{
    if (oldIndex != newIndex)
        updateItem(oldIndex);
    updateItem(newIndex);

    Layer::BlendMode activeBlendMode = m_canvas->layerAt(m_canvas->activeLayer())->blendMode();
    ui->blendModeComboBox->setCurrentText(blendModeName(activeBlendMode));
}

void LayerPanel::createNewLayer()
{
    if (m_canvas->layerCount() < 2)
    {
        ui->removeLayerButton->setEnabled(true);
    }

    int index = m_canvas->activeLayer() + 1;
    m_canvas->createLayer(index);
}

void LayerPanel::removeActiveLayer()
{
    int index = m_canvas->activeLayer();
    m_canvas->removeLayer(index);

    if (m_canvas->layerCount() < 2)
    {
        ui->removeLayerButton->setEnabled(false);
    }
}

void LayerPanel::toggleLayerVisibility()
{
    LayerPanelItem* layer = qobject_cast<LayerPanelItem*>(QObject::sender());
    Q_ASSERT(layer);

    int senderIndex = m_items.indexOf(layer);
    m_canvas->layerAt(senderIndex)->toggleVisible();
}

void LayerPanel::moveLayerUp()
{
    LayerPanelItem* layer = qobject_cast<LayerPanelItem*>(QObject::sender());
    Q_ASSERT(layer);

    int senderIndex = m_items.indexOf(layer);
    if (senderIndex >= m_canvas->layerCount() - 1)
        return;
    m_canvas->swapLayers(senderIndex, senderIndex + 1);
}

void LayerPanel::moveLayerDown()
{
    LayerPanelItem* layer = qobject_cast<LayerPanelItem*>(QObject::sender());
    Q_ASSERT(layer);

    int senderIndex = m_items.indexOf(layer);
    if (senderIndex < 1)
        return;
    m_canvas->swapLayers(senderIndex, senderIndex - 1);
}

void LayerPanel::changeActiveLayer()
{
    LayerPanelItem* layer = qobject_cast<LayerPanelItem*>(QObject::sender());
    Q_ASSERT(layer);

    int senderIndex = m_items.indexOf(layer);
    int currrentActive = m_canvas->activeLayer();
    if (currrentActive != senderIndex)
    {
        m_canvas->setActiveLayer(senderIndex);
    }
}

int LayerPanel::indexOfItem(const LayerPanelItem* item) const
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
        insertItem(i);
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

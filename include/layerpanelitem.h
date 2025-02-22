#ifndef LAYERPANELITEM_H
#define LAYERPANELITEM_H

#include <QWidget>
#include <QLayoutItem>

namespace Ui {
class LayerPanelItem;
}

class LayerPanelItem : public QWidget
{
    Q_OBJECT

public:
    explicit LayerPanelItem(QString displayName, QWidget* parent = nullptr);
    explicit LayerPanelItem(QWidget* parent = nullptr);
    ~LayerPanelItem();

    void select();
    void deselect();

    QString getDisplayName();
    void setDisplayName(QString displayName);
public:
    const QString SELECTED_STYLE = "QWidget#draggableRegion { background-color: #464646; }";
    const QString UNSELECTED_STYLE = "QWidget#draggableRegion { background-color: #2B2B2B; }";
public slots:
    void onVisibleButtonToggled();
    void onUpButtonClicked();
    void onDownButtonClicked();
signals:
    void visibleButtonToggled();
    void upButtonClicked();
    void downButtonClicked();
    void clicked();
    void shiftClicked();
private:
    virtual void mousePressEvent(QMouseEvent* event) override;
private:
    Ui::LayerPanelItem *ui;
};

#endif // LAYERPANELITEM_H

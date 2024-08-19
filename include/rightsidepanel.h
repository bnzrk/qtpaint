#ifndef RIGHTSIDEPANEL_H
#define RIGHTSIDEPANEL_H

#include <QWidget>
#include "canvas.h"

namespace Ui
{
class RightSidePanel;
}

class RightSidePanel : public QWidget
{
    Q_OBJECT

public:
    explicit RightSidePanel(QWidget *parent = nullptr);
    ~RightSidePanel();

    void init(Canvas* canvas);
private:
    Ui::RightSidePanel *ui;
};

#endif // RIGHTSIDEPANEL_H

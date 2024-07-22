#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "viewport.h"
#include "layerimage.h"
#include "sessionmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    SessionManager* m_session = nullptr;
    LayerImage* m_sourceImage = nullptr;
    Viewport* m_viewport = nullptr;
};

#endif // MAINWINDOW_H

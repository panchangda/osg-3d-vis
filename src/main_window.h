#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>
#include "Selection.h"


namespace Ui {
class main_window;
}

class main_window : public QWidget
{
    Q_OBJECT

public:
    explicit main_window(QWidget *parent = nullptr);
    ~main_window();


    void SetUIButtonConnections();
private:

    Ui::main_window *ui;
    void createStreamLine();
    void createStreamlineGPU();
    void createLIC();
};

#endif // MAIN_WINDOW_H

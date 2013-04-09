#ifndef PALETTE_H
#define PALETTE_H

#include <QWidget>

class MainWindow;

namespace Ui {
class Palette;
}

class Palette : public QWidget
{
    Q_OBJECT
    
public:
    explicit Palette(QWidget *parent, MainWindow *wnd);
    ~Palette();
    
private:
    Ui::Palette *ui;
};

#endif // PALETTE_H

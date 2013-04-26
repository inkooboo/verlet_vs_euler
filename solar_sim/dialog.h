#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    
    void step_simulation(double dt);
    void draw();

private slots:
    void on_timer();

private:
    Ui::Dialog *ui;

    QTimer m_timer;
};

#endif // DIALOG_H

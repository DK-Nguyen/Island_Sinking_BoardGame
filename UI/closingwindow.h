#ifndef CLOSINGWINDOW_H
#define CLOSINGWINDOW_H

#include <QDialog>

class QLabel;
class QPushButton;

class CloseWindow: public QDialog

{
    Q_OBJECT
public:
    CloseWindow(QWidget* parent=nullptr);

    QLabel* prompt;
    QPushButton *save_button, *quit_button;

signals:
    void save();
    void quit();

private slots:
    void save_clicked();
    void quit_clicked();
};

#endif // CLOSINGWINDOW_H

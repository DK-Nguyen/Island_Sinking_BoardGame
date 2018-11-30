#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <QDialog>

class QLabel;
class QPushButton;

class GameOverWindow: public QDialog

{
    Q_OBJECT
public:
    GameOverWindow(std::string winner, QWidget* parent=nullptr);

    QLabel* prompt;
    QPushButton *play_again_button, *quit_button;

signals:
    void play_again();
    void quit();

private slots:
    void play_again_clicked();
    void quit_clicked();
};


#endif // GAMEOVER_H

#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <QDialog>

class QLabel;
class QPushButton;

namespace UI{
class GameOverWindow: public QDialog

{
    Q_OBJECT
public:
    GameOverWindow(std::string winner, QWidget* parent=nullptr);

    QLabel* prompt_;
    QPushButton *playAgainButton_, *quitButton_;

signals:
    void playAgain();
    void quit();

private slots:
    void playAgainClicked();
    void quitClicked();
};
}

#endif // GAMEOVER_H

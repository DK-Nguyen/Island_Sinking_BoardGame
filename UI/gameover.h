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
    // emit signal to play again in mainWindow
    void playAgain();

    // emit signal to quit in mainWindow
    void quit();

private slots:
    // handle play_again button
    void playAgainClicked();

    // handle quit button
    void quitClicked();
};
}

#endif // GAMEOVER_H

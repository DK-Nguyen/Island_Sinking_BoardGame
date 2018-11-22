#include <gameover.h>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

GameOverWindow::GameOverWindow(QWidget *parent): QDialog(parent)
{
    prompt = new QLabel("Do you want to play again?");
    play_again_button = new QPushButton("Play Again");

    connect(play_again_button, SIGNAL(clicked()), this, SLOT(play_again_clicked()));

    quit_button = new QPushButton("Quit");
    quit_button->setToolTip("Quit game anyway");
    connect(quit_button, SIGNAL(clicked()), this, SLOT(quit_clicked()));

    QHBoxLayout *button_layout = new QHBoxLayout();
    button_layout->addWidget(play_again_button);
    button_layout->addWidget(quit_button);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addWidget(prompt);
    main_layout->addLayout(button_layout);
    setLayout(main_layout);

    setWindowTitle("Quit");
    setFixedHeight(sizeHint().height());
}

void GameOverWindow::play_again_clicked()
{
    emit play_again();
    close();
}


void GameOverWindow::quit_clicked()
{
    emit quit();
    close();
}

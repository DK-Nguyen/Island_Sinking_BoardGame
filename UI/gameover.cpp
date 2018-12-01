#include <gameover.h>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

UI::GameOverWindow::GameOverWindow(std::string winner, QWidget *parent): QDialog(parent)
{
    std::string msg;
    if (winner.compare("")==0)
    {
        msg = "No one wins. Game's Over. Do you want to play again?";
    }
    else
    {
        msg = winner + " wins. Game's Over. Do you want to play again?";
    }
    prompt_ = new QLabel(QString::fromUtf8(msg.c_str()));
    playAgainButton_ = new QPushButton("Play Again");

    connect(playAgainButton_, SIGNAL(clicked()), this, SLOT(playAgainClicked()));

    quitButton_ = new QPushButton("Quit");
    quitButton_->setToolTip("Quit game anyway");
    connect(quitButton_, SIGNAL(clicked()), this, SLOT(quitClicked()));

    QHBoxLayout *buttonLayout_ = new QHBoxLayout();
    buttonLayout_->addWidget(playAgainButton_);
    buttonLayout_->addWidget(quitButton_);

    QVBoxLayout *mainLayout_ = new QVBoxLayout();
    mainLayout_->addWidget(prompt_);
    mainLayout_->addLayout(buttonLayout_);
    setLayout(mainLayout_);

    setWindowTitle("Quit");
    setFixedHeight(sizeHint().height());
}

void UI::GameOverWindow::playAgainClicked()
{
    emit playAgain();
    close();
}


void UI::GameOverWindow::quitClicked()
{
    emit quit();
    close();
}

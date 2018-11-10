#include <closingwindow.h>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

CloseWindow::CloseWindow(QWidget *parent): QDialog(parent)
{
    prompt = new QLabel("Do you want to save current game?");
    save_button = new QPushButton("Save");
    save_button->setToolTip("Save current game");

    connect(save_button, SIGNAL(clicked()), this, SLOT(save_clicked()));

    quit_button = new QPushButton("Quit");
    quit_button->setToolTip("Quit game anyway");
    connect(quit_button, SIGNAL(clicked()), this, SLOT(quit_clicked()));

    QHBoxLayout *button_layout = new QHBoxLayout();
    button_layout->addWidget(save_button);
    button_layout->addWidget(quit_button);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addWidget(prompt);
    main_layout->addLayout(button_layout);
    setLayout(main_layout);

    setWindowTitle("Quit");
    setFixedHeight(sizeHint().height());
}

void CloseWindow::save_clicked()
{
    emit save();
    close();
}


void CloseWindow::quit_clicked()
{
    emit quit();
    close();
}

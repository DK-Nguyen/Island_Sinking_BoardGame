#include <configurationwindow.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>

ConfigurationWindow::~ConfigurationWindow()
{
    return;
}

ConfigurationWindow::ConfigurationWindow(QWidget* parent)
    : QDialog (parent)
{
    // player name part
    line_edit_lb = new QLabel(tr("Player names"));
    line_edit = new QLineEdit();
    line_edit_lb->setBuddy(line_edit);

    // number of pawn
    spinbox_lb = new QLabel(tr("Pawns per player"));
    pawn_getter = new QSpinBox();
    pawn_getter->setRange(1,3);
    spinbox_lb->setBuddy(pawn_getter);


    // load button
    load = new QPushButton(tr("Load..."));
    load->setToolTip(tr("Load previously saved game"));
    load->setEnabled(false);

    // play button
    play = new QPushButton(tr("Play"));

    // quit button
    quit = new QPushButton(tr("Quit"));

    // perform layouting
    QHBoxLayout* top = new QHBoxLayout();
    top->addWidget(line_edit_lb);
    top->addWidget(line_edit);

    QHBoxLayout* middle = new QHBoxLayout();
    middle->addWidget(spinbox_lb);
    middle->addWidget(pawn_getter);

    QHBoxLayout* bottom = new QHBoxLayout();
    bottom->addWidget(load);
    bottom->addWidget(play);
    bottom->addWidget(quit);

    QVBoxLayout* main_layout = new QVBoxLayout();
    main_layout->addLayout(top);
    main_layout->addLayout(middle);
    main_layout->addLayout(bottom);
    setLayout(main_layout);

    setWindowTitle(tr("Island Game Configuration"));
    setFixedHeight(sizeHint().height());


    // connect signals and slots
    connect(load, SIGNAL(clicked()), this, SLOT(load_from_file()));
    connect(play, SIGNAL(clicked()), this, SLOT(play_clicked()));
    connect(quit, SIGNAL(clicked()), this, SLOT(close()));

}

void ConfigurationWindow::load_from_file()
// TODO: implement loading from file
{
    QStringList player_names;
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Select Game State file"), "",
                                                    tr("Game State (*.gs);;All Files(*)"));

    return;
}

void ConfigurationWindow::play_clicked()
{

    // get players' name
    QString player_string = line_edit->text();
    if (player_string.isEmpty())
    {
        QMessageBox mes;
        mes.setText("Please enter players' name. Each player name separated by comma");
        mes.exec();
        return;
    }

    QStringList players_ = player_string.split(",", QString::SkipEmptyParts);
    if (players_.size() <2)
    {
        QMessageBox mes;
        mes.setText("At least two player names must be specified");
        mes.exec();
        return;
    }

    QStringList players = QStringList();

    for (auto name : players_){
        players.append(name.simplified());
    }
    // get pawn number
    int no_pawn = pawn_getter->value();




    Configuration config;
    config.no_pawn = no_pawn;
    config.players = players;

    emit game_start(config);

    close();
    return;
}

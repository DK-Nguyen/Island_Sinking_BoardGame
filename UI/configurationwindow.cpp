#include <configurationwindow.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

ConfigurationWindow::~ConfigurationWindow()
{
    return;
}

ConfigurationWindow::ConfigurationWindow(QWidget* parent)
    : QDialog (parent)
{
    // player name part
    label = new QLabel(tr("Player names"));
    line_edit = new QLineEdit();
    label->setBuddy(line_edit);

    // number of pawn
    pawn_getter = new QSpinBox();
    pawn_getter->setRange(2,4);

    // load button
    load = new QPushButton(tr("Load..."));
    load->setToolTip(tr("Load previously saved game"));

    // play button
    play = new QPushButton(tr("Play"));

    // quit button
    quit = new QPushButton(tr("Quit"));

    // perform layouting
    QHBoxLayout* top = new QHBoxLayout();
    top->addWidget(label);
    top->addWidget(line_edit);

    QHBoxLayout* bottom = new QHBoxLayout();
    bottom->addWidget(load);
    bottom->addWidget(play);
    bottom->addWidget(quit);

    QVBoxLayout* main_layout = new QVBoxLayout();
    main_layout->addLayout(top);
    main_layout->addLayout(bottom);
    setLayout(main_layout);

    setWindowTitle(tr("Configuration"));
    setFixedHeight(sizeHint().height());


    // connect signals and slots
    QObject::connect(load, SIGNAL(clicked()), this, SLOT(load_from_file()));
    QObject::connect(play, SIGNAL(clicked()), this, SLOT(play_clicked()));
    QObject::connect(quit, SIGNAL(clicked()), this, SLOT(close()));

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
// TODO: read saved top10 list
{
    QString player_string = line_edit->text();
    if (player_string.isEmpty())
    {
        QMessageBox mes;
        mes.setText("Please enter players' name. Each player name separated by comma");
        mes.exec();
        return;
    }

    QStringList players = player_string.split(",", QString::SkipEmptyParts);
    if (players.size() <2)
    {
        QMessageBox mes;
        mes.setText("At least two player names must be specified");
        mes.exec();
        return;
    }

    int no_pawn = pawn_getter->value();

    Configuration* config_ptr = new Configuration();
    config_ptr->no_pawn = no_pawn;
    config_ptr->players = players;

    QSharedPointer<Configuration> qconfig_ptr(config_ptr);

    emit game_start(qconfig_ptr);
    return;
}

#include <configurationwindow.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>


namespace UI{
ConfigurationWindow::~ConfigurationWindow()
{
    return;
}

ConfigurationWindow::ConfigurationWindow(QWidget* parent)
    : QDialog (parent)
{
    // player name part
    lineEditLb_ = new QLabel(tr("Player names"));
    lineEdit_ = new QLineEdit();
    lineEditLb_->setBuddy(lineEdit_);

    // number of pawn
    spinboxLb_ = new QLabel(tr("Pawns per player"));
    pawnGetter_ = new QSpinBox();
    pawnGetter_->setRange(1,3);
    spinboxLb_->setBuddy(pawnGetter_);

    // play_ button
    play_ = new QPushButton(tr("Play"));

    // quit button
    quit_ = new QPushButton(tr("Quit"));

    // perform layouting
    QHBoxLayout* top = new QHBoxLayout();
    top->addWidget(lineEditLb_);
    top->addWidget(lineEdit_);

    QHBoxLayout* middle = new QHBoxLayout();
    middle->addWidget(spinboxLb_);
    middle->addWidget(pawnGetter_);

    QHBoxLayout* bottom = new QHBoxLayout();
    bottom->addWidget(play_);
    bottom->addWidget(quit_);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(top);
    mainLayout->addLayout(middle);
    mainLayout->addLayout(bottom);
    setLayout(mainLayout);

    setWindowTitle(tr("Island Game Configuration"));
    setFixedHeight(sizeHint().height());

    connect(play_, SIGNAL(clicked()), this, SLOT(playClicked()));
    connect(quit_, SIGNAL(clicked()), this, SLOT(close()));

}

void ConfigurationWindow::playClicked()
{

    // get players' name
    QString playerString = lineEdit_->text();
    if (playerString.isEmpty())
    {
        QMessageBox mes;
        mes.setText("Please enter players' name. Each player name separated by comma");
        mes.exec();
        return;
    }

    QStringList players_ = playerString.split(",", QString::SkipEmptyParts);
    if (players_.size() <2)
    {
        QMessageBox mes;
        mes.setText("At least two player names must be specified");
        mes.exec();
        return;
    }

    if (players_.size() > 8)
    {
        QMessageBox mes;
        mes.setText("Maximum 8 players allowed");
        mes.exec();
        return;
    }

    QStringList players = QStringList();

    for (auto name : players_){
        players.append(name.simplified());
    }
    // get pawn number
    int noPawn = pawnGetter_->value();




    Configuration config;
    config.noPawn = noPawn;
    config.players = players;

    emit gameStart(config);

    close();
    return;
}
}

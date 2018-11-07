#include "configdialog.h"
#include "ui_configdialog.h"

configDialog::configDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configDialog)
{
    ui->setupUi(this);
}

configDialog::~configDialog()
{
    delete ui;
}

int configDialog::getNumPlayers() const
{
    return numPlayers_;
}

int configDialog::getNumPawn() const
{
    return numPawns_;
}

void configDialog::on_loadButton_clicked()
{

}

void configDialog::on_playButton_clicked()
{
    QPlainTextEdit* getNumPlayer = ui->playerNum;
    QPlainTextEdit* getNumPawn = ui->pawnNum;
    numPlayers_ = getNumPlayer->toPlainText().toInt();
    numPawns_ = getNumPawn->toPlainText().toInt();
    accept();
}

void configDialog::on_quitButton_clicked()
{
    reject();
}

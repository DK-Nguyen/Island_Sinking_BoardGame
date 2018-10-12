#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->startingButton, &QPushButton::clicked,
            this, &Dialog::accept);
    connect(ui->closingButton, &QPushButton::clicked,
            this, &Dialog::reject);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::accept(){
    QPlainTextEdit* getNum = ui->getNum;
    numObjects_ = getNum->toPlainText();
    done(QDialog::Accepted);
}

int Dialog::getNumObjects() const{
    return numObjects_.toInt();
}

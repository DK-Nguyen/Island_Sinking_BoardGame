#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class configDialog;
}

class configDialog : public QDialog
{
    Q_OBJECT

public:
    explicit configDialog(QWidget *parent = 0);
    ~configDialog();
    int getNumPlayers() const;
    int getNumPawn() const;

private slots:
    void on_loadButton_clicked();
    void on_playButton_clicked();
    void on_quitButton_clicked();

private:
    Ui::configDialog *ui;
    int numPlayers_;
    int numPawns_;
};

#endif // CONFIGDIALOG_H

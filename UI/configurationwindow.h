#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVector>

namespace UI{
struct Configuration{
    QStringList players;
    int noPawn;
};


class ConfigurationWindow: public QDialog
{
    Q_OBJECT
public:

    ConfigurationWindow(QWidget* parent=nullptr);
    ~ConfigurationWindow();

signals:
    // emit signal containing configuration info to mainWindow
    void gameStart(UI::Configuration config);

private slots:

    // handle play button mouse click
    void playClicked();

private:
    QPushButton* play_;
    QPushButton* quit_;
    QLabel *lineEditLb_, *spinboxLb_;
    QLineEdit* lineEdit_;
    QSpinBox* pawnGetter_;

};
}
#endif // CONFIGURATIONDIALOG_H

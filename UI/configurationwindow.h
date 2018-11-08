#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVector>

struct Configuration{
    QStringList players;
    int no_pawn;
    QHash<QString, int> top10;
};


class ConfigurationWindow: public QDialog
{
    Q_OBJECT
public:

    ConfigurationWindow(QWidget* parent=nullptr);
    ~ConfigurationWindow();

signals:
    void game_start(QSharedPointer<Configuration>);

private slots:
    void play_clicked();
    void load_from_file();

private:
    QPushButton* play;
    QPushButton* quit;
    QPushButton* load;
    QLabel* label;
    QLineEdit* line_edit;
    QSpinBox* pawn_getter;

};

#endif // CONFIGURATIONDIALOG_H

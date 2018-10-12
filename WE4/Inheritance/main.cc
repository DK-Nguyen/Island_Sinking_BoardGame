#include "mainwindow.hh"
#include "objectcontroller.hh"

#include <QApplication>
#include <cstdlib>
#include <time.h>
#include <QDebug>
#include <dialog.h>


int main(int argc, char *argv[])
{
    srand(time(NULL));
    QApplication a(argc, argv);

    // making the dialog
    Dialog dialog;
    dialog.exec();
//    ObjectController* engine = new ObjectController();
//    MainWindow w(engine);
//    w.show();

    if (dialog.exec() == QDialog::Accepted) {
        ObjectController* engine = new ObjectController();
        MainWindow w(engine);
        w.spawnObjects(dialog.getNumObjects(), ObjectType::FASTBALL);
        w.moveObjects();
        w.show();
        return a.exec();
    }


}

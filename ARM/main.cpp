#include "mainwindow.h"
#include "vertices.h"

#include <iostream>
#include <QApplication>

using namespace std;

#define PGM3D_PATH "../shepp_logan.pgm3d"

int main(int argc, char *argv[])
{
    Vertices vert;
    vert.readFile(PGM3D_PATH);

    cout << vert.getCoords(58917).x() << endl;
    cout << vert.getCoords(58917).y() << endl;
    cout << vert.getCoords(58917).z() << endl;
    cout << vert.getColor(58917).value() << endl;

    QApplication a(argc, argv);
    MainWindow w;
    w.setTitle("Shepp Logan");
    w.resize(640, 480);
    w.show();

    return a.exec();
}

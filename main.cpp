#include "ProjectSimplifier.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ProjectSimplifier window;
    window.show();
    window.initalizeUI();
    return app.exec();
}

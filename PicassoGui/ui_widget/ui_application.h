#ifndef UI_APPLICATION_H
#define UI_APPLICATION_H
#include <QApplication>
#include <QEvent>

namespace ui{

class GuiApplication: public QApplication
{
public:
    GuiApplication(int& argc, char ** argv);

    bool notify(QObject * receiver, QEvent* e);
};
}
#endif // UI_APPLICATION_H

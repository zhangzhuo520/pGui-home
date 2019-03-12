#ifndef UI_APPLICATION_H
#define UI_APPLICATION_H
#include <QApplication>
#include <QEvent>

namespace ui{

void outputMessage(QtMsgType type, const char* msg);

class GuiApplication: public QApplication
{
public:
    GuiApplication(int& argc, char ** argv);

    bool notify(QObject * receiver, QEvent* e);

//    int screen_width;

//    int screen_height;
};
}
#endif // UI_APPLICATION_H

#include <QApplication>
#include <QFont>
#include <stdlib.h>
#include "ui_application.h"
#include "ui_frame.h"
#include "deftools/defcontrols.h"
#include "deftools/datastruct.h"

#include "ui_mainwindow.h"

// Only rigister Class,can use sigal and slot

void qt_gui_exit()
{

}

void signal_exit_handler()
{

}

void signal_crash_handler()
{
    const int len=1024;
    void *func[len];
    size_t size;
    int i;
    char **funs;

    signal(signum,SIG_DFL);
    size=backtrace(func,len);
    funs=(char**)backtrace_symbols(func,size);
    fprintf(stderr,"System error, Stack trace:\n");
    for(i=0;i<size;++i) fprintf(stderr,"%d %s \n",i,funs[i]);
    free(funs);
}

int main(int argc, char *argv[])
{
    atexit(qt_gui_exit);
    signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);

    // ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    signal(SIGBUS, signal_crash_handler);     // 总线错误
    signal(SIGSEGV, signal_crash_handler);    // SIGSEGV，非法内存访问
    signal(SIGFPE, signal_crash_handler);       // SIGFPE，数学相关的异常，如被0除，浮点溢出，等等
    signal(SIGABRT, signal_crash_handler);     // SIGABRT，由调用abort函数产生，进程非正常退出
    ui::GuiApplication a(argc, argv);
#if 1
    qApp->setStyle(new ui::ProxyStyle);
    QFont font;
    font.setFamily("Sans Serif");
    font.setPointSize (9);
    a.setFont(font);
    ui::MainWindow w;
#else
    ui::FlexWidget w;
#endif
    w.show();


    return a.exec();
}

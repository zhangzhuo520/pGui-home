#ifndef DRAWIMAGE_THREAD_H
#define DRAWIMAGE_THREAD_H

#include <QThread>

class DrawImageThread : public QThread
{
    Q_OBJECT
public:
    DrawImageThread();

protected:
    virtual void run();
};

#endif // DRAWIMAGE_THREAD_H

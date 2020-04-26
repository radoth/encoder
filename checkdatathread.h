#ifndef CHECKDATATHREAD_H
#define CHECKDATATHREAD_H
#include <QThread>

class checkDataThread: public QThread
{
public:
    checkDataThread();

private:
    virtual void run();
};

#endif // CHECKDATATHREAD_H

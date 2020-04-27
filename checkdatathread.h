#ifndef CHECKDATATHREAD_H
#define CHECKDATATHREAD_H
#include <QThread>

class checkDataThread: public QThread
{
    Q_OBJECT
public:
    checkDataThread();

signals:
    void ProcessEnded(bool status);

private:
    virtual void run();
};

#endif // CHECKDATATHREAD_H

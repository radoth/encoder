#ifndef REFRESHBAR_H
#define REFRESHBAR_H
#include <QThread>

class RefreshBar: public QThread
{
        Q_OBJECT
public:
    RefreshBar();

signals:
    void refreshBar();

private:
    virtual void run();
};

#endif // REFRESHBAR_H

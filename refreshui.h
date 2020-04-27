#ifndef REFRESHUI_H
#define REFRESHUI_H
#include <QThread>

class RefreshUI: public QThread
{
    Q_OBJECT
public:
    RefreshUI();

signals:
    void refreshSignal();

private:
    virtual void run();
};

#endif // REFRESHUI_H

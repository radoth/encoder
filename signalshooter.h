#ifndef SIGNALSHOOTER_H
#define SIGNALSHOOTER_H

#include <QObject>
#include <QWidget>

class SignalShooter : public QObject
{
    Q_OBJECT
public:
    explicit SignalShooter(QObject *parent = nullptr);
    void testSignalGo();

signals:
    void testSignal();


};

#endif // SIGNALSHOOTER_H

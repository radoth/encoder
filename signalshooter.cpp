#include "signalshooter.h"

SignalShooter::SignalShooter(QObject *parent) : QObject(parent)
{

}

void SignalShooter::testSignalGo()
{
    emit testSignal();
}

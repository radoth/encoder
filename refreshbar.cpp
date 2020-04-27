#include "refreshbar.h"
#include "global.h"

RefreshBar::RefreshBar()
{

}

void RefreshBar::run()
{
    while(ifRunning)
    {
        emit refreshBar();
        msleep(100);
    }
}

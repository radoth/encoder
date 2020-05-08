#include "refreshbar.h"
#include "commonData.h"

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

#include "refreshui.h"
#include "global.h"

RefreshUI::RefreshUI()
{

}

void RefreshUI::run()
{
    while(ifRunning)
    {
        emit refreshSignal();
        msleep(100);
    }
}

#include "refreshui.h"
#include "commonData.h"

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

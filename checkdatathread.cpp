#include "checkdatathread.h"
#include "commonData.h"
#include "timeSettings.h"

checkDataThread::checkDataThread()
{

}

void checkDataThread::run()
{    currentFrame=0;
     frameAll=nframes;
     currentGroup=1;
     currentField=1;
    putSeqStatus=routineCtrl();
    fclose(outfile);
    ifRunning=false;
     emit ProcessEnded(putSeqStatus);
}

#include "checkdatathread.h"
#include "global.h"

checkDataThread::checkDataThread()
{

}

void checkDataThread::run()
{    currentFrame=0;
     frameAll=nframes;
     currentGroup=1;
     currentField=1;
    putSeqStatus=putseq();
    fclose(outfile);
    ifRunning=false;
     emit ProcessEnded(putSeqStatus);
}

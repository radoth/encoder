/* 和序列有关的程序*/


#include <cstdio>
#include <cstring>
#include "timeSettings.h"
#include "commonData.h"

bool routineCtrl()
{
  /* this routine assumes (N % M) == 0 */
  int i, j, k, f, f0, n, np, nb, sxf=0, syf=0, sxb=0, syb=0;
  int ipflag;
  FILE *fd;
  char name[256];
  unsigned char *neworg[3], *newref[3];
  static char ipb[5] = {' ','I','P','B','D'};
  struct timeval tv_start, tv_end;
  double picture_time, total_time = 0;

  feedbackInit(); /* initialize rate control */

  /* sequence header, sequence extension and sequence display extension */
  mainHeaderAdd();
  if (!mpeg1Flag)
  {
    seqExtHeaderAdd();
    seqDispExtHeaderAdd();
  }

  /* optionally output some text data (description, copyright or whatever) */
  if (strlen(id_string) > 1)
    usrDataHeaderAdd(id_string);

  /* loop through all frames in encoding/decoding order */
  for (i=0; i<framesCount; i++)
  {
    gettimeofday(&tv_start, NULL);

    if (!quiet)
    {
      //fprintf(stderr,"Encoding frame %d ",i);
      //fflush(stderr);
      currentFrame=i;
    }

    /* f0: lowest frame number in current GOP
     *
     * first GOP contains N-(M-1) frames,
     * all other GOPs contain N frames
     */
    f0 = framePerGOP*((i+(IPDistance-1))/framePerGOP) - (IPDistance-1);

    if (f0<0)
      f0=0;

    if (i==0 || (i-1)%IPDistance==0)
    {
      /* I or P frame */
      for (j=0; j<3; j++)
      {
        /* shuffle reference frames */
        neworg[j] = oldorgframe[j];
        newref[j] = oldrefframe[j];
        oldorgframe[j] = neworgframe[j];
        oldrefframe[j] = newrefframe[j];
        neworgframe[j] = neworg[j];
        newrefframe[j] = newref[j];
      }

      /* f: frame number in display order */
      f = (i==0) ? 0 : i+IPDistance-1;
      if (f>=framesCount)
        f = framesCount - 1;

      if (i==f0) /* first displayed frame in GOP is I */
      {
        /* I frame */
        pictType = I_TYPE;
        forwHorFCode = forwVertFCode = 15;
        backHorFCode = backVertFCode = 15;

        /* n: number of frames in current GOP
         *
         * first GOP contains (M-1) less (B) frames
         */
        n = (i==0) ? framePerGOP-(IPDistance-1) : framePerGOP;

        /* last GOP may contain less frames */
        if (n > framesCount-f0)
          n = framesCount-f0;

        /* number of P frames */
        if (i==0)
          np = (n + 2*(IPDistance-1))/IPDistance - 1; /* first GOP */
        else
          np = (n + (IPDistance-1))/IPDistance - 1;

        /* number of B frames */
        nb = n - np - 1;

        GOPControlInit(np,nb);

        GOPHeaderAdd(f0,i==0); /* set closed_GOP in first GOP only */
      }
      else
      {
        /* P frame */
        pictType = P_TYPE;
        forwHorFCode = motionData[0].forwHorFCode;
        forwVertFCode = motionData[0].forwVertFCode;
        backHorFCode = backVertFCode = 15;
        sxf = motionData[0].sxf;
        syf = motionData[0].syf;
      }
    }
    else
    {
      /* B frame */
      for (j=0; j<3; j++)
      {
        neworg[j] = auxorgframe[j];
        newref[j] = auxframe[j];
      }

      /* f: frame number in display order */
      f = i - 1;
      pictType = B_TYPE;
      n = (i-2)%IPDistance + 1; /* first B: n=1, second B: n=2, ... */
      forwHorFCode = motionData[n].forwHorFCode;
      forwVertFCode = motionData[n].forwVertFCode;
      backHorFCode = motionData[n].backHorFCode;
      backVertFCode = motionData[n].backVertFCode;
      sxf = motionData[n].sxf;
      syf = motionData[n].syf;
      sxb = motionData[n].sxb;
      syb = motionData[n].syb;
    }

    tempRef = f - f0;
    framePredDct = framePredDctTab[pictType-1];
    qScaleType = qscaleTab[pictType-1];
    intravlc = intravlcTab[pictType-1];
    altscan = altscanTab[pictType-1];

    //fprintf(statfile,"\nFrame %d (#%d in display order):\n",i,f);
    //fprintf(statfile," picture_type=%c\n",ipb[pict_type]);
    //fprintf(statfile," temporal_reference=%d\n",temp_ref);
    //fprintf(statfile," frame_pred_frame_dct=%d\n",frame_pred_dct);
    //fprintf(statfile," q_scale_type=%d\n",q_scale_type);
    //fprintf(statfile," intra_vlc_format=%d\n",intravlc);
    //fprintf(statfile," alternate_scan=%d\n",altscan);

    tmpPicture.mmap1.clear();
    tmpPicture.mmap2.clear();
    tmpPicture.quantmap.clear();
    tmpPicture.no=i;
    tmpPicture.dispNo=f;
    tmpPicture.picType=ipb[pictType];
    tmpPicture.tempRef=tempRef;
    tmpPicture.framePredDct=framePredDct;
    tmpPicture.qScaleType=qScaleType;
    tmpPicture.intravlc=intravlc;
    tmpPicture.altscan=altscan;

    if (pictType!=I_TYPE)
    {
      //fprintf(statfile," forward search window: %d...%d / %d...%d\n",
       // -sxf,sxf,-syf,syf);
      //fprintf(statfile," forward vector range: %d...%d.5 / %d...%d.5\n",
        //-(4<<forw_hor_f_code),(4<<forw_hor_f_code)-1,
        //-(4<<forw_vert_f_code),(4<<forw_vert_f_code)-1);
      tmpPicture.sxf=sxf;
      tmpPicture.syf=syf;
      tmpPicture.forw_hor_f_code=forwHorFCode;
      tmpPicture.forw_vert_f_code=forwVertFCode;
    }

    if (pictType==B_TYPE)
    {
      //fprintf(statfile," backward search window: %d...%d / %d...%d\n",
        //-sxb,sxb,-syb,syb);
      //fprintf(statfile," backward vector range: %d...%d.5 / %d...%d.5\n",
        //-(4<<back_hor_f_code),(4<<back_hor_f_code)-1,
        //-(4<<back_vert_f_code),(4<<back_vert_f_code)-1);
      tmpPicture.sxb=sxb;
      tmpPicture.syb=syb;
      tmpPicture.back_hor_f_code=backHorFCode;
      tmpPicture.back_vert_f_code=backVertFCode;
    }

    sprintf(name,tplorg,f+frame0);
    if(frameReadControl(name, neworg, f + frame0)==false)
        return false;

    if (fieldPicFlag)
    {
      if (!quiet)
      {
        //fprintf(stderr,"\nfirst field  (%s) ",topfirst ? "top" : "bot");
        //fflush(stderr);
        currentField=1;
      }

      pictStruct = topFirstFlag ? TOP_FIELD : BOTTOM_FIELD;

      motionEstimation(oldorgframe[0],neworgframe[0],
                        oldrefframe[0],newrefframe[0],
                        neworg[0],newref[0],
                        sxf,syf,sxb,syb,MacroBlockInfo,0,0);

      mainPredictCtrl(oldrefframe,newrefframe,predframe,0,MacroBlockInfo);
      chooseDCT(predframe[0],neworg[0],MacroBlockInfo);
      matrixTransform(predframe,neworg,MacroBlockInfo,blocks);

      if(putpict(neworg[0])==false)
          return false;

      for (k=0; k<mbHeight2*macroBlockWidth; k++)
      {
        if (MacroBlockInfo[k].blockType & MB_INTRA)
          for (j=0; j<blockCount; j++)
            innerIQuan(blocks[k*blockCount+j],blocks[k*blockCount+j],
                         DCPrec,intra_q,MacroBlockInfo[k].mquant);
        else
          for (j=0;j<blockCount;j++)
            outerIQuan(blocks[k*blockCount+j],blocks[k*blockCount+j],
                             inter_q,MacroBlockInfo[k].mquant);
      }

      matrixInverseTransform(predframe,newref,MacroBlockInfo,blocks);
      calcRatio(neworg,newref);
      insertStatistics();

      if (!quiet)
      {
        //fprintf(stderr,"second field (%s) ",topfirst ? "bot" : "top");
        //fflush(stderr);
        currentField=2;
      }

      pictStruct = topFirstFlag ? BOTTOM_FIELD : TOP_FIELD;

      ipflag = (pictType==I_TYPE);
      if (ipflag)
      {
        /* first field = I, second field = P */
        pictType = P_TYPE;
        forwHorFCode = motionData[0].forwHorFCode;
        forwVertFCode = motionData[0].forwVertFCode;
        backHorFCode = backVertFCode = 15;
        sxf = motionData[0].sxf;
        syf = motionData[0].syf;
      }

      motionEstimation(oldorgframe[0],neworgframe[0],
                        oldrefframe[0],newrefframe[0],
                        neworg[0],newref[0],
                        sxf,syf,sxb,syb,MacroBlockInfo,1,ipflag);

      mainPredictCtrl(oldrefframe,newrefframe,predframe,1,MacroBlockInfo);
      chooseDCT(predframe[0],neworg[0],MacroBlockInfo);
      matrixTransform(predframe,neworg,MacroBlockInfo,blocks);

      if(putpict(neworg[0])==false)
          return false;

      for (k=0; k<mbHeight2*macroBlockWidth; k++)
      {
        if (MacroBlockInfo[k].blockType & MB_INTRA)
          for (j=0; j<blockCount; j++)
            innerIQuan(blocks[k*blockCount+j],blocks[k*blockCount+j],
                         DCPrec,intra_q,MacroBlockInfo[k].mquant);
        else
          for (j=0;j<blockCount;j++)
            outerIQuan(blocks[k*blockCount+j],blocks[k*blockCount+j],
                             inter_q,MacroBlockInfo[k].mquant);
      }

      matrixInverseTransform(predframe,newref,MacroBlockInfo,blocks);
      calcRatio(neworg,newref);
      insertStatistics();
    }
    else
    {
      pictStruct = FRAME_PICTURE;

      /* do motion_estimation
       *
       * uses source frames (...orgframe) for full pel search
       * and reconstructed frames (...refframe) for half pel search
       */

      motionEstimation(oldorgframe[0],neworgframe[0],
                        oldrefframe[0],newrefframe[0],
                        neworg[0],newref[0],
                        sxf,syf,sxb,syb,MacroBlockInfo,0,0);

      mainPredictCtrl(oldrefframe,newrefframe,predframe,0,MacroBlockInfo);
      chooseDCT(predframe[0],neworg[0],MacroBlockInfo);
      matrixTransform(predframe,neworg,MacroBlockInfo,blocks);

      if(putpict(neworg[0])==false)
          return false;

      for (k=0; k<macroBlockHeight*macroBlockWidth; k++)
      {
        if (MacroBlockInfo[k].blockType & MB_INTRA)
          for (j=0; j<blockCount; j++)
            innerIQuan(blocks[k*blockCount+j],blocks[k*blockCount+j],
                         DCPrec,intra_q,MacroBlockInfo[k].mquant);
        else
          for (j=0;j<blockCount;j++)
            outerIQuan(blocks[k*blockCount+j],blocks[k*blockCount+j],
                             inter_q,MacroBlockInfo[k].mquant);
      }

      matrixInverseTransform(predframe,newref,MacroBlockInfo,blocks);
      calcRatio(neworg,newref);
      insertStatistics();
    }

	gettimeofday(&tv_end, NULL);

	picture_time = (tv_end.tv_sec - tv_start.tv_sec) * 1000000 + (tv_end.tv_usec - tv_start.tv_usec);
	total_time += picture_time;
    //printf("frame %d costs %f us\n", f + frame0, picture_time);
    tmpPicture.codeNo=f + frame0;
    tmpPicture.pictureTime=picture_time;

    sprintf(name,tplref,f+frame0);
    if(reconstructPicture(name,newref)==false)
        return false;

  }

  //printf("mpeg2 encoded: eclapsed %f us, %d frames, %f fps\n",total_time,nframes,nframes*1000000/total_time);
    globalDATA.totalTime=total_time;
    globalDATA.nframes=framesCount;
    globalDATA.fps=framesCount*1000000/total_time;

  fileEndAdd();

  return true;
}

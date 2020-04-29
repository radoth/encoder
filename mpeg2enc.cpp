/* mpeg2enc.c, main() 和读取参数文件函数*/



#include <cstdio>
#include <cstdlib>

#define GLOBAL /* used by global.h */
#include "config.h"
#include "global.h"
#include <QString>

/* private prototypes */


int mainORI(int argc,char *argv[])
{
  signalshooter =new SignalShooter();

  if (argc!=3)
  {
    printf("\n%s, %s\n",version,author);
    printf("Usage: mpeg2encode in.par out.m2v\n");
    //exit(0);
  }

  /* read parameter file */
  readparmfile(argv[1]);

  /* read quantization matrices */
  readquantmat();

  /* open output file */
  if (!(outfile=fopen(argv[2],"wb")))
  {
     sprintf(errortext,"Couldn't create output file %s",argv[2]);
     error(errortext);
  }

  init();
  putseq();

  fclose(outfile);

  system("PAUSE");
  return 0;
}

bool init()
{
  int i, size;
  static int block_count_tab[3] = {6,8,12};

  initbits();
  init_fdct();
  init_idct();

  /* round picture dimensions to nearest multiple of 16 or 32 */
  mb_width = (horizontal_size+15)/16;
  mb_height = prog_seq ? (vertical_size+15)/16 : 2*((vertical_size+31)/32);
  mb_height2 = fieldpic ? mb_height>>1 : mb_height; /* for field pictures */
  width = 16*mb_width;
  height = 16*mb_height;

  chrom_width = (chroma_format==CHROMA444) ? width : width>>1;
  chrom_height = (chroma_format!=CHROMA420) ? height : height>>1;

  height2 = fieldpic ? height>>1 : height;
  width2 = fieldpic ? width<<1 : width;
  chrom_width2 = fieldpic ? chrom_width<<1 : chrom_width;
  
  block_count = block_count_tab[chroma_format-1];

  /* clip table */
  if (!(clp = (unsigned char *)malloc(1024)))
    {error("malloc failed\n");return false;}
  clp+= 384;
  for (i=-384; i<640; i++)
    clp[i] = (i<0) ? 0 : ((i>255) ? 255 : i);

  for (i=0; i<3; i++)
  {
    size = (i==0) ? width*height : chrom_width*chrom_height;

    if (!(newrefframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(oldrefframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(auxframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(neworgframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(oldorgframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(auxorgframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(predframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
  }

  mbinfo = (struct mbinfo *)malloc(mb_width*mb_height2*sizeof(struct mbinfo));

  if (!mbinfo)
    {error("malloc failed\n");return false;}

  blocks =
    (short (*)[64])malloc(mb_width*mb_height2*block_count*sizeof(short [64]));

  if (!blocks)
    {error("malloc failed\n");return false;}


  return true;
}

void error(QString text)
{
  //fprintf(stderr,text.toStdString().c_str());
  //putc('\n',stderr);
  hasError=1;
}

bool readparmfile(char *fname)
{
  int i;
  int h,m,s,f;
  FILE *fd;
  char line[256];
  static double ratetab[8]=
    {24000.0/1001.0,24.0,25.0,30000.0/1001.0,30.0,50.0,60000.0/1001.0,60.0};
  extern int r,Xi,Xb,Xp,d0i,d0p,d0b; /* rate control */
  extern double avg_act; /* rate control */

  if (!(fd = fopen(fname,"r")))
  {
    sprintf(errortext,"Couldn't open parameter file %s",fname);
    {error(errortext);return false;}
  }

  fgets(id_string,254,fd);
  fgets(line,254,fd); sscanf(line,"%s",tplorg);
  fgets(line,254,fd); sscanf(line,"%s",tplref);
  fgets(line,254,fd); sscanf(line,"%s",iqname);
  fgets(line,254,fd); sscanf(line,"%s",niqname);
  fgets(line,254,fd); sscanf(line,"%s",statname);
  fgets(line,254,fd); sscanf(line,"%d",&inputtype);
  fgets(line,254,fd); sscanf(line,"%d",&nframes);
  fgets(line,254,fd); sscanf(line,"%d",&frame0);
  fgets(line,254,fd); sscanf(line,"%d:%d:%d:%d",&h,&m,&s,&f);
  fgets(line,254,fd); sscanf(line,"%d",&N);
  fgets(line,254,fd); sscanf(line,"%d",&M);
  fgets(line,254,fd); sscanf(line,"%d",&mpeg1);
  fgets(line,254,fd); sscanf(line,"%d",&fieldpic);
  fgets(line,254,fd); sscanf(line,"%d",&horizontal_size);
  fgets(line,254,fd); sscanf(line,"%d",&vertical_size);
  fgets(line,254,fd); sscanf(line,"%d",&aspectratio);
  fgets(line,254,fd); sscanf(line,"%d",&frame_rate_code);
  fgets(line,254,fd); sscanf(line,"%lf",&bit_rate);
  fgets(line,254,fd); sscanf(line,"%d",&vbv_buffer_size);   
  fgets(line,254,fd); sscanf(line,"%d",&low_delay);     
  fgets(line,254,fd); sscanf(line,"%d",&constrparms);
  fgets(line,254,fd); sscanf(line,"%d",&profile);
  fgets(line,254,fd); sscanf(line,"%d",&level);
  fgets(line,254,fd); sscanf(line,"%d",&prog_seq);
  fgets(line,254,fd); sscanf(line,"%d",&chroma_format);
  fgets(line,254,fd); sscanf(line,"%d",&video_format);
  fgets(line,254,fd); sscanf(line,"%d",&color_primaries);
  fgets(line,254,fd); sscanf(line,"%d",&transfer_characteristics);
  fgets(line,254,fd); sscanf(line,"%d",&matrix_coefficients);
  fgets(line,254,fd); sscanf(line,"%d",&display_horizontal_size);
  fgets(line,254,fd); sscanf(line,"%d",&display_vertical_size);
  fgets(line,254,fd); sscanf(line,"%d",&dc_prec);
  fgets(line,254,fd); sscanf(line,"%d",&topfirst);
  fgets(line,254,fd); sscanf(line,"%d %d %d",
    frame_pred_dct_tab,frame_pred_dct_tab+1,frame_pred_dct_tab+2);
  
  fgets(line,254,fd); sscanf(line,"%d %d %d",
    conceal_tab,conceal_tab+1,conceal_tab+2);
  
  fgets(line,254,fd); sscanf(line,"%d %d %d",
    qscale_tab,qscale_tab+1,qscale_tab+2);

  fgets(line,254,fd); sscanf(line,"%d %d %d",
    intravlc_tab,intravlc_tab+1,intravlc_tab+2);
  fgets(line,254,fd); sscanf(line,"%d %d %d",
    altscan_tab,altscan_tab+1,altscan_tab+2);
  fgets(line,254,fd); sscanf(line,"%d",&repeatfirst);
  fgets(line,254,fd); sscanf(line,"%d",&prog_frame);
/* intra slice interval refresh period */  
  fgets(line,254,fd); sscanf(line,"%d",&P);
  fgets(line,254,fd); sscanf(line,"%d",&r);
  fgets(line,254,fd); sscanf(line,"%lf",&avg_act);
  fgets(line,254,fd); sscanf(line,"%d",&Xi);
  fgets(line,254,fd); sscanf(line,"%d",&Xp);
  fgets(line,254,fd); sscanf(line,"%d",&Xb);
  fgets(line,254,fd); sscanf(line,"%d",&d0i);
  fgets(line,254,fd); sscanf(line,"%d",&d0p);
  fgets(line,254,fd); sscanf(line,"%d",&d0b);

  if (N<1)
    {error("N must be positive");return false;}
  if (M<1)
    {error("M must be positive");return false;}
  if (N%M != 0)
    {error("N must be an integer multiple of M");return false;}

  motion_data = (struct motion_data *)malloc(M*sizeof(struct motion_data));
  if (!motion_data)
    {error("malloc failed\n");return false;}

  for (i=0; i<M; i++)
  {
    fgets(line,254,fd);
    sscanf(line,"%d %d %d %d",
      &motion_data[i].forw_hor_f_code, &motion_data[i].forw_vert_f_code,
      &motion_data[i].sxf, &motion_data[i].syf);

    if (i!=0)
    {
      fgets(line,254,fd);
      sscanf(line,"%d %d %d %d",
        &motion_data[i].back_hor_f_code, &motion_data[i].back_vert_f_code,
        &motion_data[i].sxb, &motion_data[i].syb);
    }
  }

  fclose(fd);

  /* make flags boolean (x!=0 -> x=1) */
  mpeg1 = !!mpeg1;
  fieldpic = !!fieldpic;
  low_delay = !!low_delay;
  constrparms = !!constrparms;
  prog_seq = !!prog_seq;
  topfirst = !!topfirst;

  for (i=0; i<3; i++)
  {
    frame_pred_dct_tab[i] = !!frame_pred_dct_tab[i];
    conceal_tab[i] = !!conceal_tab[i];
    qscale_tab[i] = !!qscale_tab[i];
    intravlc_tab[i] = !!intravlc_tab[i];
    altscan_tab[i] = !!altscan_tab[i];
  }
  repeatfirst = !!repeatfirst;
  prog_frame = !!prog_frame;

  /* make sure MPEG specific parameters are valid */
  if(range_checks()==false)
      return false;

  frame_rate = ratetab[frame_rate_code-1];

  /* timecode -> frame number */
  tc0 = h;
  tc0 = 60*tc0 + m;
  tc0 = 60*tc0 + s;
  tc0 = (int)(frame_rate+0.5)*tc0 + f;

  if (!mpeg1)
  {

    if(profile_and_level_checks()==false)
        return false;
  }
  else
  {
    /* MPEG-1 */
    if (constrparms)
    {
      if (horizontal_size>768
          || vertical_size>576
          || ((horizontal_size+15)/16)*((vertical_size+15)/16)>396
          || ((horizontal_size+15)/16)*((vertical_size+15)/16)*frame_rate>396*25.0
          || frame_rate>30.0)
      {
        if (!quiet)
          {
            //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
            warningTextGlobal.append("警告：设置 constrained_parameters_flag = 0");
        }
        constrparms = 0;
      }
    }

    if (constrparms)
    {
      for (i=0; i<M; i++)
      {
        if (motion_data[i].forw_hor_f_code>4)
        {
          if (!quiet)
            {
              //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
              warningTextGlobal.append("警告：设置 constrained_parameters_flag = 0");
          }
          constrparms = 0;
          break;
        }

        if (motion_data[i].forw_vert_f_code>4)
        {
          if (!quiet)
            {
              //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
              warningTextGlobal.append("警告：设置 constrained_parameters_flag = 0");
          }
          constrparms = 0;
          break;
        }

        if (i!=0)
        {
          if (motion_data[i].back_hor_f_code>4)
          {
            if (!quiet)
              {
                //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
                warningTextGlobal.append("警告：设置 constrained_parameters_flag = 0");
            }
            constrparms = 0;
            break;
          }

          if (motion_data[i].back_vert_f_code>4)
          {
            if (!quiet)
              {
                //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
                warningTextGlobal.append("警告：设置 constrained_parameters_flag = 0");
            }
            constrparms = 0;
            break;
          }
        }
      }
    }
  }

  /* relational checks */

  if (mpeg1)
  {
    if (!prog_seq)
    {
      if (!quiet)
        {
          //fprintf(stderr,"Warning: setting progressive_sequence = 1\n");
          warningTextGlobal.append("警告：设置 progressive_sequence = 1");
      }
      prog_seq = 1;
    }

    if (chroma_format!=CHROMA420)
    {
      if (!quiet)
        {
          //fprintf(stderr,"Warning: setting chroma_format = 1 (4:2:0)\n");
          warningTextGlobal.append("警告：设置 chroma_format = 1 (4:2:0)");
      }
      chroma_format = CHROMA420;
    }

    if (dc_prec!=0)
    {
      if (!quiet)
        {
          //fprintf(stderr,"Warning: setting intra_dc_precision = 0\n");
          warningTextGlobal.append("警告：设置 intra_dc_precision = 0");
      }
      dc_prec = 0;
    }

    for (i=0; i<3; i++)
      if (qscale_tab[i])
      {
        if (!quiet)
          {
            //fprintf(stderr,"Warning: setting qscale_tab[%d] = 0\n",i);
            warningTextGlobal.append(QString("警告：设置 qscale_tab[%1] = 0").arg(i));
        }
        qscale_tab[i] = 0;
      }

    for (i=0; i<3; i++)
      if (intravlc_tab[i])
      {
        if (!quiet)
          {
            //fprintf(stderr,"Warning: setting intravlc_tab[%d] = 0\n",i);
            warningTextGlobal.append(QString("警告：设置 intravlc_tab[%1] = 0").arg(i));
        }
        intravlc_tab[i] = 0;
      }

    for (i=0; i<3; i++)
      if (altscan_tab[i])
      {
        if (!quiet)
          {
            //fprintf(stderr,"Warning: setting altscan_tab[%d] = 0\n",i);
            warningTextGlobal.append(QString("警告：设置 altscan_tab[%1] = 0").arg(i));
        }
        altscan_tab[i] = 0;
      }
  }

  if (!mpeg1 && constrparms)
  {
    if (!quiet)
      {
        //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
        warningTextGlobal.append("警告：设置 constrained_parameters_flag = 0");
    }
    constrparms = 0;
  }

  if (prog_seq && !prog_frame)
  {
    if (!quiet)
      {
        //fprintf(stderr,"Warning: setting progressive_frame = 1\n");
        warningTextGlobal.append("警告：设置 progressive_frame = 1");
    }
    prog_frame = 1;
  }

  if (prog_frame && fieldpic)
  {
    if (!quiet)
      {
        //fprintf(stderr,"Warning: setting field_pictures = 0\n");
        warningTextGlobal.append("警告：设置 field_pictures = 0");
    }
    fieldpic = 0;
  }

  if (!prog_frame && repeatfirst)
  {
    if (!quiet)
      {
        //fprintf(stderr,"Warning: setting repeat_first_field = 0\n");
        warningTextGlobal.append("警告：设置 repeat_first_field = 0");
    }
    repeatfirst = 0;
  }

  if (prog_frame)
  {
    for (i=0; i<3; i++)
      if (!frame_pred_dct_tab[i])
      {
        if (!quiet)
          {
            //fprintf(stderr,"Warning: setting frame_pred_frame_dct[%d] = 1\n",i);
            warningTextGlobal.append(QString("警告：设置 frame_pred_frame_dct[%1] = 1").arg(i));
        }
        frame_pred_dct_tab[i] = 1;
      }
  }

  if (prog_seq && !repeatfirst && topfirst)
  {
    if (!quiet)
      {
        //fprintf(stderr,"Warning: setting top_field_first = 0\n");
        warningTextGlobal.append("警告：设置 top_field_first = 0");
    }
    topfirst = 0;
  }

  /* search windows */
  for (i=0; i<M; i++)
  {
    if (motion_data[i].sxf > (4<<motion_data[i].forw_hor_f_code)-1)
    {
      if (!quiet)
        {
          //fprintf(stderr,
          //"Warning: reducing forward horizontal search width to %d\n",
          //(4<<motion_data[i].forw_hor_f_code)-1);
          warningTextGlobal.append(QString("警告：将向前的水平搜索宽度减小到 %1").arg((4<<motion_data[i].forw_hor_f_code)-1));
      }
      motion_data[i].sxf = (4<<motion_data[i].forw_hor_f_code)-1;
    }

    if (motion_data[i].syf > (4<<motion_data[i].forw_vert_f_code)-1)
    {
      if (!quiet)
        {
          //fprintf(stderr,
          //"Warning: reducing forward vertical search width to %d\n",
          //(4<<motion_data[i].forw_vert_f_code)-1);
          warningTextGlobal.append(QString("警告：将向前的垂直搜索宽度减小到 %1").arg((4<<motion_data[i].forw_vert_f_code)-1));
      }
      motion_data[i].syf = (4<<motion_data[i].forw_vert_f_code)-1;
    }

    if (i!=0)
    {
      if (motion_data[i].sxb > (4<<motion_data[i].back_hor_f_code)-1)
      {
        if (!quiet)
          {
            //fprintf(stderr,
            //"Warning: reducing backward horizontal search width to %d\n",
            //(4<<motion_data[i].back_hor_f_code)-1);
            warningTextGlobal.append(QString("警告：将向后的水平搜索宽度减小到 %1").arg((4<<motion_data[i].back_hor_f_code)-1));
        }
        motion_data[i].sxb = (4<<motion_data[i].back_hor_f_code)-1;
      }

      if (motion_data[i].syb > (4<<motion_data[i].back_vert_f_code)-1)
      {
        if (!quiet)
          {
            //fprintf(stderr,
            //"Warning: reducing backward vertical search width to %d\n",
            //(4<<motion_data[i].back_vert_f_code)-1);
            warningTextGlobal.append(QString("警告：将向后的垂直搜索宽度减小到 %1").arg((4<<motion_data[i].back_vert_f_code)-1));
        }
        motion_data[i].syb = (4<<motion_data[i].back_vert_f_code)-1;
      }
    }
  }
return true;
}

bool readquantmat()
{
  int i,v;
  FILE *fd;

  if (iqname[0]=='-')
  {
    /* use default intra matrix */
    load_iquant = 0;
    for (i=0; i<64; i++)
      intra_q[i] = default_intra_quantizer_matrix[i];
  }
  else
  {
    /* read customized intra matrix */
    load_iquant = 1;
    if (!(fd = fopen(iqname,"r")))
    {
      sprintf(errortext,"Couldn't open quant matrix file %s",iqname);
      {error(errortext);return false;}
    }

    for (i=0; i<64; i++)
    {
      fscanf(fd,"%d",&v);
      if (v<1 || v>255)
        {error("invalid value in quant matrix");return false;}
      intra_q[i] = v;
    }

    fclose(fd);
  }

  if (niqname[0]=='-')
  {
    /* use default non-intra matrix */
    load_niquant = 0;
    for (i=0; i<64; i++)
      inter_q[i] = 16;
  }
  else
  {
    /* read customized non-intra matrix */
    load_niquant = 1;
    if (!(fd = fopen(niqname,"r")))
    {
      sprintf(errortext,"Couldn't open quant matrix file %s",niqname);
      {error(errortext);return false;}
    }

    for (i=0; i<64; i++)
    {
      fscanf(fd,"%d",&v);
      if (v<1 || v>255)
        {error("invalid value in quant matrix");return false;}
      inter_q[i] = v;
    }

    fclose(fd);
  }
  return true;
}

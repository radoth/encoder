/* putvlc.c, �䳤�������*/



#include <stdio.h>

#include "config.h"
#include "global.h"
#include "vlc.h"

/* �������� */
static void putDC _ANSI_ARGS_((sVLCtable *tab, int val));

/* Ϊ����DCϵ�������ɱ䳤���� */
void putDClum(val)
int val;
{
  putDC(DClumtab,val);
}

/* Ϊɫ��DCϵ�������ɱ䳤���� */
void putDCchrom(val)
int val;
{
  putDC(DCchromtab,val);
}

//ΪDCϵ�������ɱ䳤�ı���
static void putDC(tab,val)
sVLCtable *tab;
int val;
{
  int absval, size;

  absval = (val<0) ? -val : val; /* abs(val) */

  if (absval>2047 || (mpeg1 && absval>255))
  {
    sprintf(errortext,"DC value out of range (%d)\n",val);
    error(errortext);
  }

  /* ����dct_dc_size */
  size = 0;

  while (absval)
  {
    absval >>= 1;
    size++;
  }

  /*���ݱ�B-12 ��B-13Ϊdct_dc_size ����VLC */
  putbits(tab[size].code,tab[size].len);

  /* ���ӹ̶����ȵĴ��� (dc_dct_differential) */
  if (size!=0)
  {
    if (val>=0)
      absval = val;
    else
      absval = val + (1<<size) - 1; /* val + (2 ^ size) - 1 */
    putbits(absval,size);
  }
}

/*Ϊ��֡�ڷ�ʽ�Ŀ�ĵ�һ��ACϵ������VLC����*/
void putACfirst(run,val)
int run,val;
{
  if (run==0 && (val==1 || val==-1)) /* �������������µ�ֵҪ����Դ�*/
    putbits(2|(val<0),2); 
  else
    putAC(run,val,0); 
}

/*��������DCTϵ�����б��롣*/
void putAC(run,signed_level,vlcformat)
int run,signed_level,vlcformat;
{
  int level, len;
  VLCtable *ptab = NULL;

  level = (signed_level<0) ? -signed_level : signed_level; /* abs(signed_level) */

  /* Ҫ��֤�γ̺ͼ���level����Ч */
  if (run<0 || run>63 || level==0 || level>2047 || (mpeg1 && level>255))
  {
    sprintf(errortext,"AC value out of range (run=%d, signed_level=%d)\n",
      run,signed_level);
    error(errortext);
  }

  len = 0;

  if (run<2 && level<41)
  {
    /* ����vlcformat ѡ����ñ�B-14 ���� B-15 */
	  if (vlcformat)
      ptab = &dct_code_tab1a[run][level-1];
    else
      ptab = &dct_code_tab1[run][level-1];

    len = ptab->len;
  }
  else if (run<32 && level<6)
  {
    /* ����vlcformat ѡ����ñ�B-14 ���� B-15 */
	  if (vlcformat)
      ptab = &dct_code_tab2a[run-2][level-1];
    else
      ptab = &dct_code_tab2[run-2][level-1];

    len = ptab->len;
  }

  if (len!=0) /* ˵������һ��VLC ���� */
  {
    putbits(ptab->code,len);
    putbits(signed_level<0,1); /* ���ʶ */
  }
  else
  {
        /* ˵��������м��ʽ (run, level) û��VLC����:��ʹ��escape���� */
    putbits(1l,6); /* Escape */
    putbits(run,6); /* ��6 bit ��ʾ�γ̣�run ��*/
    if (mpeg1)
    {
      /* ISO/IEC 11172-2 �涨ʹ�� 8 �� 16 bit �Ĵ��� */
		if (signed_level>127)
        putbits(0,8);
      if (signed_level<-127)
        putbits(128,8);
      putbits(signed_level,8);
    }
    else
    {
      /* ISO/IEC 13818-2 �涨ʹ��12 bit ������ݱ�B-16 */
		putbits(signed_level,12);
    }
  }
}

/* Ϊmacroblock_address_increment ����VLC���� */
void putaddrinc(addrinc)
int addrinc;
{
  while (addrinc>33)
  {
    putbits(0x08,11); /* macroblock_escape */
    addrinc-= 33;
  }

  putbits(addrinctab[addrinc-1].code,addrinctab[addrinc-1].len);
}

/* Ϊmacroblock_type ����VLC */
void putmbtype(pict_type,mb_type)
int pict_type,mb_type;
{
  putbits(mbtypetab[pict_type-1][mb_type].code,
          mbtypetab[pict_type-1][mb_type].len);
}

/* Ϊmotion_code ����VLC */
void putmotioncode(motion_code)
int motion_code;
{
  int abscode;

  abscode = (motion_code>=0) ? motion_code : -motion_code; /* abs(motion_code) */
  putbits(motionvectab[abscode].code,motionvectab[abscode].len);
  if (motion_code!=0)
    putbits(motion_code<0,1);  /* ��ʶ, 0Ϊ��, 1Ϊ�� */
}

/* Ϊdmvector[t]����VLC���ݱ� B-11 */
void putdmv(dmv)
int dmv;
{
  if (dmv==0)
    putbits(0,1);
  else if (dmv>0)
    putbits(2,2);
  else
    putbits(3,2);
}

/* Ϊ������ģʽcoded_block_pattern ����VLC��
���� 4:2:2, 4:4:4 ͼ���ʽ�Ŀ鲻���� */
void putcbp(cbp)
int cbp;
{
  putbits(cbptable[cbp].code,cbptable[cbp].len);
}

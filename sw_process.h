/* library for switches for Arduino
 based on sw_process by ZEATEC co., ltd. (http://www.zeatec.jp/efellows/susume/tyuukyuu1-8.php)
 */

/* part of header of original sw_process */
 
/*****************************************************************************************
�X�C�b�`�p���C�u���� 2015/4/7�쐬

���񋟌�
ZEATEC co.,ltd. 

�������p�ɂ���
�]�ځE���f�g�p�ł��B���̃��C�u�������g�p������ł̕s����Ɋւ��ẮA�����Ȃ���e�ɂ���
�Ă���؂̐ӔC��ǂ�Ȃ����̂Ƃ��܂��B

���g�p���@
#include "sw_process.c"���w�b�_�[�ɒǉ�����sw_process()�����C�����[�v�ɓ���Ă��������B���L
�̊֐���ǉ����邱�Ƃł��ꂼ��̓���̎��ɌĂяo����܂��B���L�T���v����RA1�݂̂ł��B�����
�O�̓��͂Ɋւ��Ă�RA1���Q�l�ɒǋL���Ă��������B

sw_a1�ɂ͌��ݒl���擾���Ă��܂��B

*/

/* usage

sw_process(pinNo,id);

and define

void sw_##id##_push();
void sw_##id##_release();
void sw_##id##_hold();
void sw_##id##_wpush();
void sw_##id##_holdon();

 */

/*
*****************************************************************************************/

#define sw_process(BUTTON,ID)		  \
  int sw_##ID = 0; /* RA1�̌��ݒl  */				\
  long holdup_##ID = 30000; /*����������l */			\
  long holdcount_##ID = 0; /* �����Ă���J�E���g�l */		\
  long releaseup_##ID = 10000; /* �����Ă��鎞�̔���l */	\
  long releasecount_##ID = 0; /* �����Ă���J�E���g�l */	\
  long history_##ID = 0; /* �X�C�b�`�̗��� */				\
  															\
  void sw_##ID##_push();									\
  void sw_##ID##_release();									\
  void sw_##ID##_hold();									\
  void sw_##ID##_holdon();									\
  void sw_##ID##ID_wpush();									\
															\
  void sw_##ID##_process(){									\
															\
  sw_##ID = digitalRead(BUTTON);							\
  if(sw_##ID==1){											\
	if(holdcount_##ID<holdup_##ID){							\
	  holdcount_##ID++;										\
	}else if(holdcount_##ID==holdup_##ID){					\
	  holdcount_##ID++;										\
	  sw_##ID##_hold();										\
	}else{													\
	  releasecount_##ID = releaseup_##ID;					\
	  sw_##ID##_holdon();									\
	}														\
  }else{													\
	holdcount_##ID = 0;										\
  }															\
																\
  if(history_##ID == 0 && history_##ID != holdcount_##ID){		\
	sw_##ID##_push();											\
	releasecount_##ID = releasecount_##ID + releaseup_##ID;		\
  }																\
  if(history_##ID != 0 && holdcount_##ID == 0){					\
	sw_##ID##_release();										\
  }																\
  if(releasecount_##ID>0){										\
	releasecount_##ID--;										\
	if(releasecount_##ID>releaseup_##ID){						\
	  sw_##ID##_wpush();										\
	  releasecount_##ID = 0;									\
	}															\
  }																\
  history_##ID = holdcount_##ID;								\
}

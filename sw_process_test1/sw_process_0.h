/* library for switches for Arduino
 based on sw_process by ZEATEC co., ltd. (http://www.zeatec.jp/efellows/susume/tyuukyuu1-8.php)
 */

/* part of header of original sw_process */
 
/*****************************************************************************************
スイッチ用ライブラリ 2015/4/7作成

□提供元
ZEATEC co.,ltd. 

□ご利用について
転載・無断使用可です。このライブラリを使用した上での不具合等に関しては、いかなる内容におい
ても一切の責任を追わないものとします。

□使用方法
#include "sw_process.c"をヘッダーに追加してsw_process()をメインループに入れてください。下記
の関数を追加することでそれぞれの動作の時に呼び出されます。下記サンプルはRA1のみです。それ以
外の入力に関してはRA1を参考に追記してください。

sw_a1には現在値を取得しています。

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
  int sw_##ID = 0; /* RA1の現在値  */				\
  long holdup_##ID = 30000; /*長押し判定値 */			\
  long holdcount_##ID = 0; /* 押しているカウント値 */		\
  long releaseup_##ID = 10000; /* 放している時の判定値 */	\
  long releasecount_##ID = 0; /* 放しているカウント値 */	\
  long history_##ID = 0; /* スイッチの履歴 */				\
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

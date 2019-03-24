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

   (1) Call macro do define everything

       sw_process(pinNo,id);

   in the front matter
   
   (2) Define functions and implement process for each switch action
   
       void sw_##id##_push();
       void sw_##id##_release();
       void sw_##id##_hold();
       void sw_##id##_wpush();
       void sw_##id##_holdon();

   (3) call
   
       sw_##id##_process(int button_status);

	   where buttun_status is the status of the button (for e.g. digitalWrite(pinNO))

	in loop

*/

/*
*****************************************************************************************/

#define sw_process(BUTTON,ID)								\
  int sw_##ID = 0; /* #ID の現在値  */						\
  long holdup_##ID = 5000; /*長押し判定値 */				\
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
  void sw_##ID##_process(int sw){							\
  															\
	if(sw==1){													\
	  if(holdcount_##ID<holdup_##ID){							\
		holdcount_##ID++;										\
	  }else if(holdcount_##ID==holdup_##ID){					\
		holdcount_##ID++;										\
		sw_##ID##_hold();										\
	  }else{													\
		releasecount_##ID = releaseup_##ID;						\
		sw_##ID##_holdon();										\
	  }															\
	}else{														\
	  holdcount_##ID = 0;										\
	}															\
																\
	if(history_##ID == 0 && history_##ID != holdcount_##ID){	\
	  sw_##ID##_push();											\
	  releasecount_##ID = releasecount_##ID + releaseup_##ID;	\
	}															\
	if(history_##ID != 0 && holdcount_##ID == 0){				\
	  sw_##ID##_release();										\
	}															\
	if(releasecount_##ID>0){									\
	  releasecount_##ID--;										\
	  if(releasecount_##ID>releaseup_##ID){						\
		sw_##ID##_wpush();										\
		releasecount_##ID = 0;									\
	  }															\
	}															\
	history_##ID = holdcount_##ID;								\
  }

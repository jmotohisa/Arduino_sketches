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

//押した時
void sw_a1_push(){
	fprintf(c1,"Push ok.\r\n");
}

//放した時
void sw_a1_release(){
	fprintf(c1,"Release ok.\r\n");
}

//長押し
void sw_a1_hold(){
	fprintf(c1,"Hold ok.\r\n");
}

//ダブルクリック
void sw_a1_wpush(){
	fprintf(c1,"Double click ok.\r\n");
}

また、50msec単位の割り込みでflicker_hi_oneを1にするフラグと長押し中との組み合わせで、設定ボ
タンを長押しすると設定値が高速で変わるといった使い方も出来ます。

//長押し中
void sw_a1_holdon(){
    if(flicker_hi_one == 1){
        flicker_hi_one = 0;
        //フリッカーハイのタイミングでしか処理しない
        test_val++;
        
        fprintf(c1,"Holdon ok. test_val=%u\r\n",test_val);
    }
}

*****************************************************************************************/

int sw_a1 = 0;//RA1の現在値
long holdup_a1 = 30000;//長押し判定値
long holdcount_a1 = 0;//押しているカウント値
long releaseup_a1 = 10000;//放している時の判定値
long releasecount_a1 = 0;//放しているカウント値
long history_a1 = 0;//スイッチの履歴

//押した時
void sw_a1_push();
//放した時
void sw_a1_release();
//長押し判定したとき
void sw_a1_hold();
//長押し中
void sw_a1_holdon();
//ダブルクリック
void sw_a1_wpush();

//スイッチ処理
void sw_process(){

	//********************
	//PIN_A1用ソース
	//********************
	//現在値を取得
	//ONでGNDの場合
	sw_a1 = !input(PIN_A1);
	//ONでVCCの場合
	//sw_a1 = input(PIN_A1);

	//長押し
	if(sw_a1==1){
		if(holdcount_a1<holdup_a1){
			holdcount_a1++;
		}else if(holdcount_a1==holdup_a1){
			holdcount_a1++;
			//スイッチを長押しした場合
			sw_a1_hold();
		}else{
			//ダブルクリック用変数割り当て
			releasecount_a1 = releaseup_a1;
			//長押ししたままの場合
			sw_a1_holdon();
		}
	}else{
		holdcount_a1 = 0;
	}

	//押した時
	if(history_a1 == 0 && history_a1 != holdcount_a1){
		sw_a1_push();

		//ダブルクリック用変数割り当て
		releasecount_a1 = releasecount_a1 + releaseup_a1;
	}

	//放した時
	if(history_a1 != 0 && holdcount_a1 == 0){
		sw_a1_release();
	}

	//ダブルクリックしたとき
	if(releasecount_a1>0){
		releasecount_a1--;
		if(releasecount_a1>releaseup_a1){
			sw_a1_wpush();
			releasecount_a1 = 0;
		}
	}

	history_a1 = holdcount_a1;
	//********************
	//そのほかの入力は上記を参考に追記してください。

}

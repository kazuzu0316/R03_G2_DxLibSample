#include "DxLib.h" //DxLibを使う時は必要

//マクロ定義
#define GAME_TITLE  "ゲームタイトル"  //ゲームタイトル
#define GAME_WIDTH  1280			  //ゲーム画面の幅(ウィドス)
#define GAME_HEIGHT 720				  //ゲーム画面の高さ(ハイト)
#define GAME_COLOR  32				  //ゲームの色域

#define GAME_ICON_ID   333				  //ゲームのICONのID

#define GAME_WINDOW_BAR 0				//ウィンドウバーの種類

// プログラムは WinMain から始まります
//Windowsのプログラミング方法 = (WinAPI)で動いている
//DxLibは、DirecttXという、ゲームプログラミングを簡単に使える仕組み
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);  //ウィンドウモードに設定
	SetMainWindowText(GAME_TITLE);  //ウィンドウのタイトルの文字
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);  //ウィンドウの解像度を設定
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);  //ウィンドウの大きさを設定
	SetBackgroundColor(255, 255, 255);  //デフォルトの背景の色
	SetWindowIconID(GAME_ICON_ID);  //アイコンファイルの読込
	SetWindowStyleMode(GAME_WINDOW_BAR);  //ウィンドウバーの状態
	SetWaitVSyncFlag(TRUE);  //ディスプレイの垂直同期を有効にする
	SetAlwaysRunFlag(TRUE);  //ウィンドウをずっとアクティブにする


	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	//位置
	int X = GAME_WIDTH / 2;  //横の中心
	int Y = GAME_HEIGHT / 2;  //縦の中心


	int width = 32;
	int height = 32;  //高さ

	//円の半径を決める
	int radius = 100;

	int Speed = 5;
	int XSpeed = Speed;
	int YSpeed = Speed;

	//ダブルバッファリング有効化
	SetDrawScreen(DX_SCREEN_BACK);

	//無限ループ
	while (1)
	{
		//何かしらのキーが押されたとき
		if (CheckHitKeyAll() != 0)  //0のときに、何かキーが押された
		{
			break;  //無限ループを抜ける
		}

		//メッセージを受け取り続ける
		if (ProcessMessage() != 0)  //-1のとき、エラーやウィンドウが閉じられた
		{
			break;  //無限ループを抜ける
		}

		//画面を消去する
		if (ClearDrawScreen() != 0) { break; }

		/*
		//四角を描画
		DrawBox(
			X, Y, X + width, Y + height,
			GetColor(255, 0, 0),  //色の取得
			TRUE				  //塗りつぶし?
		);
		*/

		//円の描画
		DrawCircle(
			X, Y, radius,
			GetColor(0, 255, 0),
			FALSE, 5
		);



		X += XSpeed;  //四角の位置を右にずらす

		//質問①　四角を斜め右下に動かそう!
		Y += YSpeed;

		//質問②　四角を画面の端にきたら、移動する向きを反転させてみよう
		if (X - radius < 0 || X + radius > GAME_WIDTH)  //画面の横から出た時
		{
			//移動速度の符号を反転させる
			//+1なら、-1する / -1ならば、+1にする
			XSpeed = -XSpeed;  //移動する向きを反転

			//壁に当たったら速くなる
			if (XSpeed > 0) { XSpeed += 10; }
			else if (XSpeed < 0) { XSpeed -= 10; }
		}

		if (Y - radius < 0 || Y + radius > GAME_HEIGHT)  //画面の横から出た時
		{
			//移動速度の符号を反転させる
			//+1なら、-1する / -1ならば、+1にする
			YSpeed = -YSpeed;  //移動する向きを反転

			//壁に当たったら速くなる
			if (YSpeed > 0) { YSpeed += 10; }
			else if (YSpeed < 0) { YSpeed -= 10; }
		}



		ScreenFlip();  //ダブルバッファリングした画面を描画

	}

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}


//ヘッダファイル読み込み
#include "game.h"		//ゲーム全体のヘッダファイル
#include "keyboard.h"	//キーボードの処理
#include "FPS.h"		//FPS処理

//グローバル変数
//シーンを管理する変数
GAME_SCENE GameScene;  //現在のゲームシーン
GAME_SCENE OldGameScene;  //前回のゲームシーン
GAME_SCENE NextGameScene;  //次のゲームシーン

//画面の切り替え
BOOL IsFadeOut = FALSE;  //フェードアウト
BOOL IsFadeIn = FALSE;  //フェードイン

//シーン切り替え
int fadeTimeMill = 2000;						//切り替えミリ秒
int fadeTimeMAX = fadeTimeMill / 1000 * 60;	//ミリ秒をフレーム秒に変換

//フェードアウト
int fadeOutCntInit = 0;				//初期化
int fadeOutCnt = fadeOutCntInit;	//フェードアウトのカウンタ
int fadeOutCntMAX = fadeTimeMAX;	//フェードアウトのカウンタMAX

//フェードイン
int fadeInCntInit = fadeTimeMAX;	//初期化
int fadeInCnt = fadeInCntInit;		//フェードインのカウンタ
int fadeInCntMAX = fadeTimeMAX;				//フェードインのカウンタMAX


//プロトタイプ宣言
VOID Title(VOID);		//タイトル画面
VOID TitleProc(VOID);   //タイトル画面(処理)
VOID TitleDraw(VOID);   //タイトル画面(描画)

VOID Play(VOID);		//プレイ画面
VOID PlayProc(VOID);    //プレイ画面(処理)
VOID PlayDraw(VOID);    //プレイ画面(描画)

VOID End(VOID);			//エンド画面
VOID EndProc(VOID);     //エンド画面(処理)
VOID EndDraw(VOID);     //エンド画面(描画)

VOID Change(VOID);		//切り替え画面
VOID ChangeProc(VOID);  //切り替え画面(処理)
VOID ChangeDraw(VOID);  //切り替え画面(描画)

VOID ChangeScene(GAME_SCENE scnen);  //シーンの切り替え





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


	//ダブルバッファリング有効化
	SetDrawScreen(DX_SCREEN_BACK);

	//円の中心点
	int X = GAME_WIDTH / 2;
	int Y = GAME_HEIGHT / 2;
	//円の半径
	int radius = 50;

	int Speed = 10;

	//最初のシーンは、タイトル画面から
	GameScene = GAME_SCENE_TITLE;

	//無限ループ
	while (1)
	{


		if (ProcessMessage() != 0) { break; }  //メッセージを受け取り続ける
		if (ClearDrawScreen() != 0) { break; }  //画面を消去する



		//キーボード入力の更新
		AllKeyUpdate();

		//FPS値の更新
		FPSUpdate();

		//ESCキーで強制終了
		if (KeyClick(KEY_INPUT_ESCAPE) == TRUE) { break; }

		//以前のシーンを取得
		if (GameScene != GAME_SCENE_CHANGE)
		{
			OldGameScene = GameScene;
		}

		//シーンごとに処理を行う
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:  //タイトル画面
			Title();
			break;
		case GAME_SCENE_PLAY:  //プレイ画面
			Play();
			break;
		case GAME_SCENE_END:  //エンド画面
			End();
			break;
		case GAME_SCENE_CHANGE:  //切り替え画面
			Change();
			break;
		default:
			break;
		}

		//シーンを切り替える
		if (OldGameScene != GameScene)
		{
			//現在のシーンが切り替え場面でないとき
			if (GameScene != GAME_SCENE_CHANGE)
			{
				NextGameScene = GameScene;  //次のシーンを保存
				GameScene = GAME_SCENE_CHANGE;  //画面切り替えシーンに変える
			}
		}





		//キーを入力
		if (KeyDown(KEY_INPUT_UP) == TRUE)
		{
			Y -= Speed; //上に移動
			
		}
		if (KeyDown(KEY_INPUT_DOWN) == TRUE)
		{
			Y += Speed; //下に移動
		}
		if (KeyDown(KEY_INPUT_LEFT) == TRUE)
		{
			X -= Speed; //左に移動
		}
		if (KeyDown(KEY_INPUT_RIGHT) == TRUE)
		{
			X += Speed; //右に移動
		}
		if (KeyDown(KEY_INPUT_1) == TRUE)
		{
			Speed++;  //スピードアップ

		}
		if (KeyDown(KEY_INPUT_2) == TRUE)
		{
			Speed--;  //スピードダウン
		}
		if (radius > X) X = radius;	//左封鎖
		if (radius > Y) Y = radius;	//上封鎖
		if (GAME_WIDTH - radius < X) X = GAME_WIDTH - radius;	//右封鎖
		if (GAME_HEIGHT - radius < Y) Y = GAME_HEIGHT - radius;	//	下封鎖

		DrawCircle(X, Y, radius, GetColor(255, 255, 0), TRUE);

		//FPS値を描画
		FPSDraw();

		//FPS値を待つ
		FPSWait();

		ScreenFlip();  //ダブルバッファリングした画面を描画

	}

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}

VOID ChangeScene(GAME_SCENE scene)
{
	GameScene = scene;				//シーンを切り替え
	IsFadeIn = FALSE;				//フェードインしない
	IsFadeOut = TRUE;				//フェードアウトする

	return;
}





/// <summary>
/// タイトル画面
/// </summary>
VOID Title(VOID)
{
	TitleProc();  //処理
	TitleDraw();  //描画

	return;
}

/// <summary>
/// タイトル画面の処理
/// </summary>
VOID TitleProc(VOID)
{
	//プレイシーンへ切り替える
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーンを切り替え
		//次のシーンの初期化をここで行うと楽

		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);
	}


	return;
}

/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw(VOID)
{
	DrawString(0, 0, "タイトル画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// プレイ画面
/// </summary>
VOID Play(VOID)
{
	PlayProc();  //処理
	PlayDraw();  //描画

	return;
}

/// <summary>
/// プレイ画面の処理
/// </summary>
VOID PlayProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーンを切り替え
		//次のシーンの初期化をここで行うと楽

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_END);
	}
	return;
}

/// <summary>
/// プレイ画面の描画
/// </summary>
VOID PlayDraw(VOID)
{
	DrawString(0, 0, "プレイ画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// エンド画面
/// </summary>
VOID End(VOID)
{
	EndProc();  //処理
	EndDraw();  //描画

	return;
}

/// <summary>
/// エンド画面の処理
/// </summary>
VOID EndProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーンを切り替え
		//次のシーンの初期化をここで行うと楽

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);
	}
	return;
}

/// <summary>
/// エンド画面の描画
/// </summary>
VOID EndDraw(VOID)
{
	DrawString(0, 0, "エンド画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// 切り替え画面
/// </summary>
VOID Change(VOID)
{
	ChangeProc();  //処理
	ChangeDraw();  //描画

	return;
}

/// <summary>
/// 切り替え画面の処理
/// </summary>
VOID ChangeProc(VOID)
{
	//フェードイン
	if (IsFadeIn == TRUE)
	{
		if (fadeInCnt > fadeInCntMAX)
		{
			fadeInCnt--;	//カウンタを減らす
		}
		else
		{
			//フェードイン処理が終わった

			fadeInCnt = fadeInCntInit;	//カウンタ初期化
			IsFadeIn = FALSE;			//フェードイン処理終了
		}

	}

	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		if (fadeOutCnt < fadeOutCntMAX)
		{
			fadeOutCnt++;	//カウンタを増やす
		}
		else
		{
			//フェードアウト処理が終わった

			fadeOutCnt = fadeOutCntInit;	//カウンタ初期化
			IsFadeOut = FALSE;				//フェードアウト処理終了
		}

	}

	//切り替え処理終了?
	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		//フェードインしていない、フェードアウトもしていないとき
		GameScene = NextGameScene;	//次のシーンに切り替え
		OldGameScene = GameScene;	//以前のゲームシーン更新
	}


	return;
}

/// <summary>
/// 切り替え画面の描画
/// </summary>
VOID ChangeDraw(VOID)
{
	//以前のシーンを描画
	switch (OldGameScene)
	{
	case GAME_SCENE_TITLE:
		TitleDraw();	//タイトル画面の描画
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();		//プレイ画面の描画
		break;
	case GAME_SCENE_END:
		EndDraw();		//エンド画面の描画
		break;
	default:
		break;
	}

	//フェードイン
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeInCnt / fadeInCntMAX) * 255);
	}

	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeOutCnt / fadeOutCntMAX) * 255);
	}

	//四角を描画
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0), TRUE);

	//半透明終了
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


	DrawString(0, 16, "切り替え画面", GetColor(0, 0, 0));
	return;
}
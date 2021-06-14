

//ヘッダファイル読み込み
#include "game.h"		//ゲーム全体のヘッダファイル
#include "keyboard.h"	//キーボードの処理
#include "FPS.h"		//FPS処理

//画像の構造体
struct IMAGE
{
	int handle = -1;	//画像のハンドル(管理番号)
	char path[255];		//画像の場所(パス)
	
	int x;				//x位置
	int y;				//y位置
	int width;			//幅
	int height;			//高さ

	BOOL IsDraw = FALSE;//画像が描画できる?
};

//キャラクタの構造体
struct CHARACTOR
{
	IMAGE img;			//画像構造体
	int speed = 1;		//移動速度
	RECT coll;			//当たり判定の領域(四角)
};

//動画の構造体
struct MOVIE
{
	int handle = -1;	//動画のハンドル(管理番号)
	char path[255];		//動画の場所(パス)
	int x;				//x位置
	int y;				//y位置
	int width;			//幅
	int height;			//高さ

	int Volume = 255;	//ボリューム(最小)0～255(最大)
};

//音楽の構造体
struct AUDIO
{
	int handle = -1;	//音楽ハンドル
	char path[255];		//音楽パス

	int Volume = -1;	//ボリューム (MIN 0～255 MAX)
	int playType = -1;	//BGM or SE

};




//グローバル変数
//シーンを管理する変数
GAME_SCENE GameScene;  //現在のゲームシーン
GAME_SCENE OldGameScene;  //前回のゲームシーン
GAME_SCENE NextGameScene;  //次のゲームシーン

//プレイ背景の動画
MOVIE playMovie;

//プレイヤー
CHARACTOR player;

//ゴール
CHARACTOR Goal;

//画像を読み込む
IMAGE TitleLogo;	//タイトルロゴ
IMAGE TitleEnter;	//エンターキーを押してね
IMAGE EndClear;		//クリアロゴ

//音楽
AUDIO TitleBGM;	//
AUDIO PlayBGM;	//
AUDIO EndBGM;	//

//効果音
AUDIO PlayerSE;

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

//PushEnterの点滅
int PushEnterCnt = 0;			//カウント
const int PushEnterCntMAX = 60;		//カウントMAX値
BOOL PushEnterBrink = FALSE;	//点滅しているか？

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

VOID CollUpadateGollPlayer(CHARACTOR* chara);	//当たり判定の領域を更新
VOID CollUpadateGoll(CHARACTOR* chara);			//当たり判定の領域を更新

BOOL OnCollision(RECT a, RECT b);				//短形と短形の当たり判定

BOOL GameLoad(VOID);		//ゲームのデータを読み込み

BOOL LoadImageMem(IMAGE* image, const char* path);
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType);

VOID GameInit(VOID);		//ゲームのデータの初期化



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



	//最初のシーンは、タイトル画面から
	GameScene = GAME_SCENE_TITLE;

	//ゲーム全体の初期化

	//ゲーム読み込み
	if(!GameLoad())
	{
		//データの読み込みに失敗したとき
		DxLib_End();	//DxLib終了
		return -1;		//異常終了
	}
	
	//ゲームの初期化
	GameInit();

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





		//FPS値を描画
		FPSDraw();

		//FPS値を待つ
		FPSWait();

		ScreenFlip();  //ダブルバッファリングした画面を描画

	}

	//終わるときの処理
	DeleteGraph(player.img.handle);	//画像をメモリ上から削除
	DeleteGraph(Goal.img.handle);	//画像をメモリ上から削除

	DeleteGraph(TitleLogo.handle);		//画像をメモリ上から削除
	DeleteGraph(TitleEnter.handle);		//画像をメモリ上から削除
	DeleteGraph(EndClear.handle);		//画像をメモリ上から削除

	DeleteGraph(playMovie.handle);	//動画をメモリ上から削除

	DeleteSoundMem(TitleBGM.handle);	//音楽をメモリ上から削除
	DeleteSoundMem(PlayBGM.handle);		//音楽をメモリ上から削除
	DeleteSoundMem(EndBGM.handle);		//音楽をメモリ上から削除

	DeleteSoundMem(PlayerSE.handle);	//音楽をメモリ上から削除

	DxLib_End();				// ＤＸライブラリ使用の終了処理




	return 0;				// ソフトの終了 
}


/// <summary>
/// ゲームのデータを読み込み
/// </summary>
/// <returns>読み込めたらTRUE / 読み込めなかったらFALSE</returns>
BOOL GameLoad(VOID)
{
	//ゲーム全体の初期化

	//プレイ動画の背景の読み込み
	strcpyDx(playMovie.path, ".\\Movie\\PlayMovie.mp4");	//パスのコピー
	playMovie.handle = LoadGraph(playMovie.path); //画像の読み込み
	/*
	//プレイヤーの画像を読み込み
	strcpyDx(player.img.path, ".\\Image\\player.png");	//パスのコピー
	player.img.handle = LoadGraph(player.img.path); //画像の読み込み
	*/
	//動画が読み込めなかったときは、エラー(-1)が入る
	if (playMovie.handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			playMovie.path,			//メッセージ本文
			"動画読み込みエラー!",	//メッセージタイトル
			MB_OK					//ボタン
		);
		return FALSE;	//読み込み失敗
	}

	//画像の幅と高さを取得
	GetGraphSize(playMovie.handle, &playMovie.width, &playMovie.height);

	//動画のボリューム
	playMovie.Volume = 255;

	/*
	//当たり判定を更新する
	CollUpadateGollPlayer(&player);	//プレイヤーの当たり判定のアドレス
	*/



	//画像を読み込み
	if (!LoadImageMem(&player.img, ".\\Image\\player.\png")) { return FALSE; }
	if (!LoadImageMem(&Goal.img, ".\\Image\\Goal.\png")) { return FALSE; }

	//ロゴを読み込む
	if (!LoadImageMem(&TitleLogo, ".\\Image\\タイトルロゴ.\png")) { return FALSE; }
	if (!LoadImageMem(&TitleEnter, ".\\Image\\PushEnter.\png")) { return FALSE; }
	if (!LoadImageMem(&EndClear, ".\\Image\\Clear2.\png")) { return FALSE; }

	//音楽を読み込む
	if (!LoadAudio(&TitleBGM, ".\\Audio\\Summer_Princess.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&PlayBGM, ".\\Audio\\Neon_Garden.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&EndBGM, ".\\Audio\\I&#039m_here.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }

	if (!LoadAudio(&PlayerSE, ".\\Audio\\可愛い足音.mp3", 255, DX_PLAYTYPE_BACK)) { return FALSE; }

	return TRUE;	//	全て読み込めた!
}

/// <summary>
/// 画像をメモリに読み込み
/// </summary>
/// <param name="image">画像構造体のアドレス</param>
/// <param name="path">画像のパス</param>
/// <returns></returns>
BOOL LoadImageMem(IMAGE* image, const char* path)
{
	//ゴールの画像を読み込み
	strcpyDx(image->path, path);	//パスのコピー
	image->handle = LoadGraph(image->path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (image->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			image->path,			//メッセージ本文
			"画像読み込みエラー！",		//メッセージタイトル
			MB_OK					//ボタン
		);

		return FALSE;	//読み込み失敗
	}

	//画像の幅と高さを取得
	GetGraphSize(image->handle, &image->width, &image->height);

	//読み込めた
	return TRUE;
}

/// <summary>
/// 音楽をメモリに読み込み
/// </summary>
/// <param name="audio">Audio構造体変数のアドレス</param>
/// <param name="path">Audioの音楽パス</param>
/// <param name="volume">ボリューム</param>
/// <param name="playType">DX_PLAYTYPE_LOOP or DX_PLAYTYPE_BACK</param>
/// <returns></returns>
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType)
{
	//音楽の読み込み
	strcpyDx(audio->path, path);					//パスのコピー
	audio->handle = LoadSoundMem(audio->path);		//音楽の読み込み

	//音楽が読み込めなかったときは、エラー(-1)が入る
	if (audio->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			audio->path,			//メッセージ本文
			"音楽読み込みエラー！",		//メッセージタイトル
			MB_OK					//ボタン
		);

		return FALSE;	//読み込み失敗
	}

	//その他の設定
	audio->Volume = volume;
	audio->playType = playType;

	return TRUE;
}

/// <summary>
/// ゲームデータの初期化
/// </summary>
/// <param name=""></param>
VOID GameInit(VOID)
{
	//プレイヤーを初期化
	player.img.x = GAME_WIDTH / 2 - player.img.width / 2;	//中央寄せ
	player.img.y = GAME_HEIGHT / 2 - player.img.height / 2;	//中央寄せ
	player.speed = 500;								//スピード
	player.img.IsDraw = TRUE;							//描画できる

	//当たり判定を更新する
	CollUpadateGollPlayer(&player);	//プレイヤーの当たり判定のアドレス

	//ゴールを初期化
	Goal.img.x = GAME_WIDTH - Goal.img.width;	//中央寄せ
	Goal.img.y = 0;							//中央寄せ
	Goal.speed = 500;					//スピード
	Goal.img.IsDraw = TRUE;					//描画できる

	//当たり判定を更新する
	CollUpadateGoll(&Goal);	//プレイヤーの当たり判定のアドレス

	//タイトルロゴの位置を決める
	TitleLogo.x = GAME_WIDTH / 2 - TitleLogo.width / 2;	//中央揃え
	TitleLogo.y = 100;

	//PusHEnterの位置を決める
	TitleEnter.x = GAME_WIDTH / 2 - TitleEnter.width / 2;	//中央揃え
	TitleEnter.y = GAME_HEIGHT - TitleEnter.height - 100;

	//PushEnterの点滅
	int PushEnterCnt = 0;			//カウント
	//int PushEnterCntMAX = 60;		初期化しなくていい?
	BOOL PushEnterBrink = FALSE;	//点滅しているか？

	//クリアロゴの位置を決める
	EndClear.x = GAME_WIDTH / 2 - EndClear.width / 2;	//中央揃え
	EndClear.y = GAME_HEIGHT / 2 - EndClear.height / 2;		//中央揃え
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
		//BGMを止める
		StopSoundMem(TitleBGM.handle);


		//シーンを切り替え
		//次のシーンの初期化をここで行うと楽

		//ゲームの初期化
		GameInit();

		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);

		return;
	}

	//BGMが流れていないとき
	if (CheckSoundMem(TitleBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(TitleBGM.handle, TitleBGM.playType);
	}

	return;
}

/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw(VOID)
{
	//タイトルロゴの描画
	DrawGraph(TitleLogo.x, TitleLogo.y, TitleLogo.handle, TRUE);

	//MAX値までを持つ
	if (PushEnterCnt < PushEnterCntMAX) { PushEnterCnt++; }
	else
	{
		if (PushEnterBrink == TRUE)PushEnterBrink = FALSE;
		else if (PushEnterBrink == FALSE)PushEnterBrink = TRUE;
	
		PushEnterCnt = 0;	//カウントを初期化
	}

	//PushEnterを描画
	if (PushEnterBrink == TRUE)
	{
		//半透明にする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)PushEnterCnt / PushEnterCntMAX) * 255);

		//PushEnterの描画
		DrawGraph(TitleEnter.x, TitleEnter.y, TitleEnter.handle, TRUE);

		//半透明終了
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (PushEnterBrink == FALSE)
	{
		//半透明にする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)(PushEnterCntMAX - PushEnterCnt) / PushEnterCntMAX) * 255);

		//PushEnterの描画
		DrawGraph(TitleEnter.x, TitleEnter.y, TitleEnter.handle, TRUE);

		//半透明終了
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

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
	/*
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーンを切り替え
		//次のシーンの初期化をここで行うと楽

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_END);
	}
	*/
	//BGMが流れていないとき
	if (CheckSoundMem(PlayBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(PlayBGM.handle, PlayBGM.playType);
	}

	//プレイヤーの操作
	if (KeyDown(KEY_INPUT_UP) == TRUE)
	{
		player.img.y -= player.speed * fps.DeltaTime;
	
		//動くときの効果音を追加
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
			PlaySoundMem(PlayerSE.handle, PlayerSE.playType);

		}
	}

	if (KeyDown(KEY_INPUT_DOWN) == TRUE)
	{
		player.img.y += player.speed * fps.DeltaTime;;

		//動くときの効果音を追加
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
			PlaySoundMem(PlayerSE.handle, PlayerSE.playType);

		}
	}
	if (KeyDown(KEY_INPUT_LEFT) == TRUE)
	{
		player.img.x -= player.speed * fps.DeltaTime;;
	
		//動くときの効果音を追加
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
			PlaySoundMem(PlayerSE.handle, PlayerSE.playType);

		}
	}

	if (KeyDown(KEY_INPUT_RIGHT) == TRUE)
	{
		player.img.x += player.speed * fps.DeltaTime;;
	
		//動くときの効果音を追加
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
			PlaySoundMem(PlayerSE.handle, PlayerSE.playType);

		}
	}

	//当たり判定を更新する
	CollUpadateGollPlayer(&player);
	//当たり判定を更新する
	CollUpadateGoll(&Goal);
	//プレイヤーがゴールに当たった時
	if (OnCollision(player.coll, Goal.coll) == TRUE)
	{
		//BGMを止める
		StopSoundMem(PlayBGM.handle);

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_END);

		//処理を強制終了
		return;
	};

	return;
}

/// <summary>
/// プレイ画面の描画
/// </summary>
VOID PlayDraw(VOID)
{
	//背景動画を描画
	if (GetMovieStateToGraph(playMovie.handle) == 0)
	{
		//再生する
		SeekMovieToGraph(playMovie.handle, 0);	//シークバーを最初に戻す
		PlayMovieToGraph(playMovie.handle);		//動画を再生
	}
	//動画を描画(画像を引き伸ばす)
	DrawExtendGraph(0, 0, GAME_WIDTH, GAME_HEIGHT, playMovie.handle, TRUE);

	//プレイヤーの描画
	if (player.img.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(player.img.x, player.img.y, player.img.handle, TRUE);
	
		//デバックのときは、当たり判定の領域を描画
		if (GAME_DEBUG == TRUE)
		{
			//四角を描画
			DrawBox(player.coll.left, player.coll.top, player.coll.right, player.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
		


	}

	//ゴールの描画
	if (Goal.img.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(Goal.img.x, Goal.img.y, Goal.img.handle, TRUE);

		//デバックのときは、当たり判定の領域を描画
		if (GAME_DEBUG == TRUE)
		{
			//四角を描画
			DrawBox(Goal.coll.left, Goal.coll.top, Goal.coll.right, Goal.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}



	}

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
		//BGMを止める
		StopSoundMem(EndBGM.handle);

		//シーンを切り替え
		//次のシーンの初期化をここで行うと楽

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}
	//BGMが流れていないとき
	if (CheckSoundMem(EndBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(EndBGM.handle, EndBGM.playType);
	}

	return;
}

/// <summary>
/// エンド画面の描画
/// </summary>
VOID EndDraw(VOID)
{
	//EndClearの描画
	DrawGraph(EndClear.x, EndClear.y, EndClear.handle, TRUE);

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

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="coll">当たり判定の領域</param>
VOID CollUpadateGollPlayer(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;					//当たり判定微調整
	chara->coll.top = chara->img.y;						//当たり判定微調整
	chara->coll.right = chara->img.x + chara->img.width -40;	//当たり判定微調整
	chara->coll.bottom = chara->img.y + chara->img.height;	//当たり判定微調整

	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="coll">当たり判定の領域</param>
VOID CollUpadateGoll(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;					//当たり判定微調整
	chara->coll.top = chara->img.y;						//当たり判定微調整
	chara->coll.right = chara->img.x + chara->img.width - 40;	//当たり判定微調整
	chara->coll.bottom = chara->img.y + chara->img.height;	//当たり判定微調整

	return;
}
/*
typedef struct tagRECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
}; RECT, * PRECT;
*/

BOOL OnCollision(RECT a, RECT b)
{
	if (
		a.left < b.right &&	//　短形Aの左辺x座標 < 短形Bの右辺x座標　かつ
		a.top < b.bottom &&	//　短形Aの上辺y座標 < 短形Bの下辺y座標　かつ
		a.right > b.left &&	//　短形Aの右辺x座標 > 短形Bの左辺x座標　かつ
		a.bottom > b.top	//　短形Aの下辺y座標 > 短形Bの上辺y座標
		)
	{
		//当たっているとき

		return TRUE;
	}
	else
	{
		//当たっていないとき
		return FALSE;
	}
}


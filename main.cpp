

//�w�b�_�t�@�C���ǂݍ���
#include "game.h"		//�Q�[���S�̂̃w�b�_�t�@�C��
#include "keyboard.h"	//�L�[�{�[�h�̏���
#include "FPS.h"		//FPS����

//�摜�̍\����
struct IMAGE
{
	int handle = -1;	//�摜�̃n���h��(�Ǘ��ԍ�)
	char path[255];		//�摜�̏ꏊ(�p�X)
	
	int x;				//x�ʒu
	int y;				//y�ʒu
	int width;			//��
	int height;			//����

	BOOL IsDraw = FALSE;//�摜���`��ł���?
};

//�L�����N�^�̍\����
struct CHARACTOR
{
	IMAGE img;			//�摜�\����
	int speed = 1;		//�ړ����x
	RECT coll;			//�����蔻��̗̈�(�l�p)
};

//����̍\����
struct MOVIE
{
	int handle = -1;	//����̃n���h��(�Ǘ��ԍ�)
	char path[255];		//����̏ꏊ(�p�X)
	int x;				//x�ʒu
	int y;				//y�ʒu
	int width;			//��
	int height;			//����

	int Volume = 255;	//�{�����[��(�ŏ�)0�`255(�ő�)
};

//���y�̍\����
struct AUDIO
{
	int handle = -1;	//���y�n���h��
	char path[255];		//���y�p�X

	int Volume = -1;	//�{�����[�� (MIN 0�`255 MAX)
	int playType = -1;	//BGM or SE

};




//�O���[�o���ϐ�
//�V�[�����Ǘ�����ϐ�
GAME_SCENE GameScene;  //���݂̃Q�[���V�[��
GAME_SCENE OldGameScene;  //�O��̃Q�[���V�[��
GAME_SCENE NextGameScene;  //���̃Q�[���V�[��

//�v���C�w�i�̓���
MOVIE playMovie;

//�v���C���[
CHARACTOR player;

//�S�[��
CHARACTOR Goal;

//�摜��ǂݍ���
IMAGE TitleLogo;	//�^�C�g�����S
IMAGE TitleEnter;	//�G���^�[�L�[�������Ă�
IMAGE EndClear;		//�N���A���S

//���y
AUDIO TitleBGM;	//
AUDIO PlayBGM;	//
AUDIO EndBGM;	//

//���ʉ�
AUDIO PlayerSE;

//��ʂ̐؂�ւ�
BOOL IsFadeOut = FALSE;  //�t�F�[�h�A�E�g
BOOL IsFadeIn = FALSE;  //�t�F�[�h�C��

//�V�[���؂�ւ�
int fadeTimeMill = 2000;						//�؂�ւ��~���b
int fadeTimeMAX = fadeTimeMill / 1000 * 60;	//�~���b���t���[���b�ɕϊ�

//�t�F�[�h�A�E�g
int fadeOutCntInit = 0;				//������
int fadeOutCnt = fadeOutCntInit;	//�t�F�[�h�A�E�g�̃J�E���^
int fadeOutCntMAX = fadeTimeMAX;	//�t�F�[�h�A�E�g�̃J�E���^MAX

//�t�F�[�h�C��
int fadeInCntInit = fadeTimeMAX;	//������
int fadeInCnt = fadeInCntInit;		//�t�F�[�h�C���̃J�E���^
int fadeInCntMAX = fadeTimeMAX;				//�t�F�[�h�C���̃J�E���^MAX

//PushEnter�̓_��
int PushEnterCnt = 0;			//�J�E���g
const int PushEnterCntMAX = 60;		//�J�E���gMAX�l
BOOL PushEnterBrink = FALSE;	//�_�ł��Ă��邩�H

//�v���g�^�C�v�錾
VOID Title(VOID);		//�^�C�g�����
VOID TitleProc(VOID);   //�^�C�g�����(����)
VOID TitleDraw(VOID);   //�^�C�g�����(�`��)

VOID Play(VOID);		//�v���C���
VOID PlayProc(VOID);    //�v���C���(����)
VOID PlayDraw(VOID);    //�v���C���(�`��)

VOID End(VOID);			//�G���h���
VOID EndProc(VOID);     //�G���h���(����)
VOID EndDraw(VOID);     //�G���h���(�`��)

VOID Change(VOID);		//�؂�ւ����
VOID ChangeProc(VOID);  //�؂�ւ����(����)
VOID ChangeDraw(VOID);  //�؂�ւ����(�`��)

VOID ChangeScene(GAME_SCENE scnen);  //�V�[���̐؂�ւ�

VOID CollUpadateGollPlayer(CHARACTOR* chara);	//�����蔻��̗̈���X�V
VOID CollUpadateGoll(CHARACTOR* chara);			//�����蔻��̗̈���X�V

BOOL OnCollision(RECT a, RECT b);				//�Z�`�ƒZ�`�̓����蔻��

BOOL GameLoad(VOID);		//�Q�[���̃f�[�^��ǂݍ���

BOOL LoadImageMem(IMAGE* image, const char* path);
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType);

VOID GameInit(VOID);		//�Q�[���̃f�[�^�̏�����



// �v���O������ WinMain ����n�܂�܂�
//Windows�̃v���O���~���O���@ = (WinAPI)�œ����Ă���
//DxLib�́ADirecttX�Ƃ����A�Q�[���v���O���~���O���ȒP�Ɏg����d�g��
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);  //�E�B���h�E���[�h�ɐݒ�
	SetMainWindowText(GAME_TITLE);  //�E�B���h�E�̃^�C�g���̕���
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);  //�E�B���h�E�̉𑜓x��ݒ�
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);  //�E�B���h�E�̑傫����ݒ�
	SetBackgroundColor(255, 255, 255);  //�f�t�H���g�̔w�i�̐F
	SetWindowIconID(GAME_ICON_ID);  //�A�C�R���t�@�C���̓Ǎ�
	SetWindowStyleMode(GAME_WINDOW_BAR);  //�E�B���h�E�o�[�̏��
	SetWaitVSyncFlag(TRUE);  //�f�B�X�v���C�̐���������L���ɂ���
	SetAlwaysRunFlag(TRUE);  //�E�B���h�E�������ƃA�N�e�B�u�ɂ���


	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}


	//�_�u���o�b�t�@�����O�L����
	SetDrawScreen(DX_SCREEN_BACK);



	//�ŏ��̃V�[���́A�^�C�g����ʂ���
	GameScene = GAME_SCENE_TITLE;

	//�Q�[���S�̂̏�����

	//�Q�[���ǂݍ���
	if(!GameLoad())
	{
		//�f�[�^�̓ǂݍ��݂Ɏ��s�����Ƃ�
		DxLib_End();	//DxLib�I��
		return -1;		//�ُ�I��
	}
	
	//�Q�[���̏�����
	GameInit();

	//�������[�v
	while (1)
	{


		if (ProcessMessage() != 0) { break; }  //���b�Z�[�W���󂯎�葱����
		if (ClearDrawScreen() != 0) { break; }  //��ʂ���������



		//�L�[�{�[�h���͂̍X�V
		AllKeyUpdate();

		//FPS�l�̍X�V
		FPSUpdate();

		//ESC�L�[�ŋ����I��
		if (KeyClick(KEY_INPUT_ESCAPE) == TRUE) { break; }

		//�ȑO�̃V�[�����擾
		if (GameScene != GAME_SCENE_CHANGE)
		{
			OldGameScene = GameScene;
		}

		//�V�[�����Ƃɏ������s��
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:  //�^�C�g�����
			Title();
			break;
		case GAME_SCENE_PLAY:  //�v���C���
			Play();
			break;
		case GAME_SCENE_END:  //�G���h���
			End();
			break;
		case GAME_SCENE_CHANGE:  //�؂�ւ����
			Change();
			break;
		default:
			break;
		}

		//�V�[����؂�ւ���
		if (OldGameScene != GameScene)
		{
			//���݂̃V�[�����؂�ւ���ʂłȂ��Ƃ�
			if (GameScene != GAME_SCENE_CHANGE)
			{
				NextGameScene = GameScene;  //���̃V�[����ۑ�
				GameScene = GAME_SCENE_CHANGE;  //��ʐ؂�ւ��V�[���ɕς���
			}
		}





		//FPS�l��`��
		FPSDraw();

		//FPS�l��҂�
		FPSWait();

		ScreenFlip();  //�_�u���o�b�t�@�����O������ʂ�`��

	}

	//�I���Ƃ��̏���
	DeleteGraph(player.img.handle);	//�摜���������ォ��폜
	DeleteGraph(Goal.img.handle);	//�摜���������ォ��폜

	DeleteGraph(TitleLogo.handle);		//�摜���������ォ��폜
	DeleteGraph(TitleEnter.handle);		//�摜���������ォ��폜
	DeleteGraph(EndClear.handle);		//�摜���������ォ��폜

	DeleteGraph(playMovie.handle);	//������������ォ��폜

	DeleteSoundMem(TitleBGM.handle);	//���y���������ォ��폜
	DeleteSoundMem(PlayBGM.handle);		//���y���������ォ��폜
	DeleteSoundMem(EndBGM.handle);		//���y���������ォ��폜

	DeleteSoundMem(PlayerSE.handle);	//���y���������ォ��폜

	DxLib_End();				// �c�w���C�u�����g�p�̏I������




	return 0;				// �\�t�g�̏I�� 
}


/// <summary>
/// �Q�[���̃f�[�^��ǂݍ���
/// </summary>
/// <returns>�ǂݍ��߂���TRUE / �ǂݍ��߂Ȃ�������FALSE</returns>
BOOL GameLoad(VOID)
{
	//�Q�[���S�̂̏�����

	//�v���C����̔w�i�̓ǂݍ���
	strcpyDx(playMovie.path, ".\\Movie\\PlayMovie.mp4");	//�p�X�̃R�s�[
	playMovie.handle = LoadGraph(playMovie.path); //�摜�̓ǂݍ���
	/*
	//�v���C���[�̉摜��ǂݍ���
	strcpyDx(player.img.path, ".\\Image\\player.png");	//�p�X�̃R�s�[
	player.img.handle = LoadGraph(player.img.path); //�摜�̓ǂݍ���
	*/
	//���悪�ǂݍ��߂Ȃ������Ƃ��́A�G���[(-1)������
	if (playMovie.handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//���C���̃E�B���h�E�n���h��
			playMovie.path,			//���b�Z�[�W�{��
			"����ǂݍ��݃G���[!",	//���b�Z�[�W�^�C�g��
			MB_OK					//�{�^��
		);
		return FALSE;	//�ǂݍ��ݎ��s
	}

	//�摜�̕��ƍ������擾
	GetGraphSize(playMovie.handle, &playMovie.width, &playMovie.height);

	//����̃{�����[��
	playMovie.Volume = 255;

	/*
	//�����蔻����X�V����
	CollUpadateGollPlayer(&player);	//�v���C���[�̓����蔻��̃A�h���X
	*/



	//�摜��ǂݍ���
	if (!LoadImageMem(&player.img, ".\\Image\\player.\png")) { return FALSE; }
	if (!LoadImageMem(&Goal.img, ".\\Image\\Goal.\png")) { return FALSE; }

	//���S��ǂݍ���
	if (!LoadImageMem(&TitleLogo, ".\\Image\\�^�C�g�����S.\png")) { return FALSE; }
	if (!LoadImageMem(&TitleEnter, ".\\Image\\PushEnter.\png")) { return FALSE; }
	if (!LoadImageMem(&EndClear, ".\\Image\\Clear2.\png")) { return FALSE; }

	//���y��ǂݍ���
	if (!LoadAudio(&TitleBGM, ".\\Audio\\Summer_Princess.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&PlayBGM, ".\\Audio\\Neon_Garden.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&EndBGM, ".\\Audio\\I&#039m_here.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }

	if (!LoadAudio(&PlayerSE, ".\\Audio\\��������.mp3", 255, DX_PLAYTYPE_BACK)) { return FALSE; }

	return TRUE;	//	�S�ēǂݍ��߂�!
}

/// <summary>
/// �摜���������ɓǂݍ���
/// </summary>
/// <param name="image">�摜�\���̂̃A�h���X</param>
/// <param name="path">�摜�̃p�X</param>
/// <returns></returns>
BOOL LoadImageMem(IMAGE* image, const char* path)
{
	//�S�[���̉摜��ǂݍ���
	strcpyDx(image->path, path);	//�p�X�̃R�s�[
	image->handle = LoadGraph(image->path);	//�摜�̓ǂݍ���

	//�摜���ǂݍ��߂Ȃ������Ƃ��́A�G���[(-1)������
	if (image->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//���C���̃E�B���h�E�n���h��
			image->path,			//���b�Z�[�W�{��
			"�摜�ǂݍ��݃G���[�I",		//���b�Z�[�W�^�C�g��
			MB_OK					//�{�^��
		);

		return FALSE;	//�ǂݍ��ݎ��s
	}

	//�摜�̕��ƍ������擾
	GetGraphSize(image->handle, &image->width, &image->height);

	//�ǂݍ��߂�
	return TRUE;
}

/// <summary>
/// ���y���������ɓǂݍ���
/// </summary>
/// <param name="audio">Audio�\���̕ϐ��̃A�h���X</param>
/// <param name="path">Audio�̉��y�p�X</param>
/// <param name="volume">�{�����[��</param>
/// <param name="playType">DX_PLAYTYPE_LOOP or DX_PLAYTYPE_BACK</param>
/// <returns></returns>
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType)
{
	//���y�̓ǂݍ���
	strcpyDx(audio->path, path);					//�p�X�̃R�s�[
	audio->handle = LoadSoundMem(audio->path);		//���y�̓ǂݍ���

	//���y���ǂݍ��߂Ȃ������Ƃ��́A�G���[(-1)������
	if (audio->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//���C���̃E�B���h�E�n���h��
			audio->path,			//���b�Z�[�W�{��
			"���y�ǂݍ��݃G���[�I",		//���b�Z�[�W�^�C�g��
			MB_OK					//�{�^��
		);

		return FALSE;	//�ǂݍ��ݎ��s
	}

	//���̑��̐ݒ�
	audio->Volume = volume;
	audio->playType = playType;

	return TRUE;
}

/// <summary>
/// �Q�[���f�[�^�̏�����
/// </summary>
/// <param name=""></param>
VOID GameInit(VOID)
{
	//�v���C���[��������
	player.img.x = GAME_WIDTH / 2 - player.img.width / 2;	//������
	player.img.y = GAME_HEIGHT / 2 - player.img.height / 2;	//������
	player.speed = 500;								//�X�s�[�h
	player.img.IsDraw = TRUE;							//�`��ł���

	//�����蔻����X�V����
	CollUpadateGollPlayer(&player);	//�v���C���[�̓����蔻��̃A�h���X

	//�S�[����������
	Goal.img.x = GAME_WIDTH - Goal.img.width;	//������
	Goal.img.y = 0;							//������
	Goal.speed = 500;					//�X�s�[�h
	Goal.img.IsDraw = TRUE;					//�`��ł���

	//�����蔻����X�V����
	CollUpadateGoll(&Goal);	//�v���C���[�̓����蔻��̃A�h���X

	//�^�C�g�����S�̈ʒu�����߂�
	TitleLogo.x = GAME_WIDTH / 2 - TitleLogo.width / 2;	//��������
	TitleLogo.y = 100;

	//PusHEnter�̈ʒu�����߂�
	TitleEnter.x = GAME_WIDTH / 2 - TitleEnter.width / 2;	//��������
	TitleEnter.y = GAME_HEIGHT - TitleEnter.height - 100;

	//PushEnter�̓_��
	int PushEnterCnt = 0;			//�J�E���g
	//int PushEnterCntMAX = 60;		���������Ȃ��Ă���?
	BOOL PushEnterBrink = FALSE;	//�_�ł��Ă��邩�H

	//�N���A���S�̈ʒu�����߂�
	EndClear.x = GAME_WIDTH / 2 - EndClear.width / 2;	//��������
	EndClear.y = GAME_HEIGHT / 2 - EndClear.height / 2;		//��������
}

VOID ChangeScene(GAME_SCENE scene)
{
	GameScene = scene;				//�V�[����؂�ւ�
	IsFadeIn = FALSE;				//�t�F�[�h�C�����Ȃ�
	IsFadeOut = TRUE;				//�t�F�[�h�A�E�g����

	return;
}

/// <summary>
/// �^�C�g�����
/// </summary>
VOID Title(VOID)
{
	TitleProc();  //����
	TitleDraw();  //�`��

	return;
}

/// <summary>
/// �^�C�g����ʂ̏���
/// </summary>
VOID TitleProc(VOID)
{
	//�v���C�V�[���֐؂�ւ���
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//BGM���~�߂�
		StopSoundMem(TitleBGM.handle);


		//�V�[����؂�ւ�
		//���̃V�[���̏������������ōs���Ɗy

		//�Q�[���̏�����
		GameInit();

		//�v���C��ʂɐ؂�ւ�
		ChangeScene(GAME_SCENE_PLAY);

		return;
	}

	//BGM������Ă��Ȃ��Ƃ�
	if (CheckSoundMem(TitleBGM.handle) == 0)
	{
		//BGM�𗬂�
		PlaySoundMem(TitleBGM.handle, TitleBGM.playType);
	}

	return;
}

/// <summary>
/// �^�C�g����ʂ̕`��
/// </summary>
VOID TitleDraw(VOID)
{
	//�^�C�g�����S�̕`��
	DrawGraph(TitleLogo.x, TitleLogo.y, TitleLogo.handle, TRUE);

	//MAX�l�܂ł�����
	if (PushEnterCnt < PushEnterCntMAX) { PushEnterCnt++; }
	else
	{
		if (PushEnterBrink == TRUE)PushEnterBrink = FALSE;
		else if (PushEnterBrink == FALSE)PushEnterBrink = TRUE;
	
		PushEnterCnt = 0;	//�J�E���g��������
	}

	//PushEnter��`��
	if (PushEnterBrink == TRUE)
	{
		//�������ɂ���
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)PushEnterCnt / PushEnterCntMAX) * 255);

		//PushEnter�̕`��
		DrawGraph(TitleEnter.x, TitleEnter.y, TitleEnter.handle, TRUE);

		//�������I��
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (PushEnterBrink == FALSE)
	{
		//�������ɂ���
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)(PushEnterCntMAX - PushEnterCnt) / PushEnterCntMAX) * 255);

		//PushEnter�̕`��
		DrawGraph(TitleEnter.x, TitleEnter.y, TitleEnter.handle, TRUE);

		//�������I��
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	DrawString(0, 0, "�^�C�g�����", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// �v���C���
/// </summary>
VOID Play(VOID)
{
	PlayProc();  //����
	PlayDraw();  //�`��

	return;
}

/// <summary>
/// �v���C��ʂ̏���
/// </summary>
VOID PlayProc(VOID)
{
	/*
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//�V�[����؂�ւ�
		//���̃V�[���̏������������ōs���Ɗy

		//�G���h��ʂɐ؂�ւ�
		ChangeScene(GAME_SCENE_END);
	}
	*/
	//BGM������Ă��Ȃ��Ƃ�
	if (CheckSoundMem(PlayBGM.handle) == 0)
	{
		//BGM�𗬂�
		PlaySoundMem(PlayBGM.handle, PlayBGM.playType);
	}

	//�v���C���[�̑���
	if (KeyDown(KEY_INPUT_UP) == TRUE)
	{
		player.img.y -= player.speed * fps.DeltaTime;
	
		//�����Ƃ��̌��ʉ���ǉ�
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
			PlaySoundMem(PlayerSE.handle, PlayerSE.playType);

		}
	}

	if (KeyDown(KEY_INPUT_DOWN) == TRUE)
	{
		player.img.y += player.speed * fps.DeltaTime;;

		//�����Ƃ��̌��ʉ���ǉ�
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
			PlaySoundMem(PlayerSE.handle, PlayerSE.playType);

		}
	}
	if (KeyDown(KEY_INPUT_LEFT) == TRUE)
	{
		player.img.x -= player.speed * fps.DeltaTime;;
	
		//�����Ƃ��̌��ʉ���ǉ�
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
			PlaySoundMem(PlayerSE.handle, PlayerSE.playType);

		}
	}

	if (KeyDown(KEY_INPUT_RIGHT) == TRUE)
	{
		player.img.x += player.speed * fps.DeltaTime;;
	
		//�����Ƃ��̌��ʉ���ǉ�
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
			PlaySoundMem(PlayerSE.handle, PlayerSE.playType);

		}
	}

	//�����蔻����X�V����
	CollUpadateGollPlayer(&player);
	//�����蔻����X�V����
	CollUpadateGoll(&Goal);
	//�v���C���[���S�[���ɓ���������
	if (OnCollision(player.coll, Goal.coll) == TRUE)
	{
		//BGM���~�߂�
		StopSoundMem(PlayBGM.handle);

		//�G���h��ʂɐ؂�ւ�
		ChangeScene(GAME_SCENE_END);

		//�����������I��
		return;
	};

	return;
}

/// <summary>
/// �v���C��ʂ̕`��
/// </summary>
VOID PlayDraw(VOID)
{
	//�w�i�����`��
	if (GetMovieStateToGraph(playMovie.handle) == 0)
	{
		//�Đ�����
		SeekMovieToGraph(playMovie.handle, 0);	//�V�[�N�o�[���ŏ��ɖ߂�
		PlayMovieToGraph(playMovie.handle);		//������Đ�
	}
	//�����`��(�摜�������L�΂�)
	DrawExtendGraph(0, 0, GAME_WIDTH, GAME_HEIGHT, playMovie.handle, TRUE);

	//�v���C���[�̕`��
	if (player.img.IsDraw == TRUE)
	{
		//�摜��`��
		DrawGraph(player.img.x, player.img.y, player.img.handle, TRUE);
	
		//�f�o�b�N�̂Ƃ��́A�����蔻��̗̈��`��
		if (GAME_DEBUG == TRUE)
		{
			//�l�p��`��
			DrawBox(player.coll.left, player.coll.top, player.coll.right, player.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
		


	}

	//�S�[���̕`��
	if (Goal.img.IsDraw == TRUE)
	{
		//�摜��`��
		DrawGraph(Goal.img.x, Goal.img.y, Goal.img.handle, TRUE);

		//�f�o�b�N�̂Ƃ��́A�����蔻��̗̈��`��
		if (GAME_DEBUG == TRUE)
		{
			//�l�p��`��
			DrawBox(Goal.coll.left, Goal.coll.top, Goal.coll.right, Goal.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}



	}

	DrawString(0, 0, "�v���C���", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// �G���h���
/// </summary>
VOID End(VOID)
{
	EndProc();  //����
	EndDraw();  //�`��

	return;
}

/// <summary>
/// �G���h��ʂ̏���
/// </summary>
VOID EndProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//BGM���~�߂�
		StopSoundMem(EndBGM.handle);

		//�V�[����؂�ւ�
		//���̃V�[���̏������������ōs���Ɗy

		//�^�C�g����ʂɐ؂�ւ�
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}
	//BGM������Ă��Ȃ��Ƃ�
	if (CheckSoundMem(EndBGM.handle) == 0)
	{
		//BGM�𗬂�
		PlaySoundMem(EndBGM.handle, EndBGM.playType);
	}

	return;
}

/// <summary>
/// �G���h��ʂ̕`��
/// </summary>
VOID EndDraw(VOID)
{
	//EndClear�̕`��
	DrawGraph(EndClear.x, EndClear.y, EndClear.handle, TRUE);

	DrawString(0, 0, "�G���h���", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// �؂�ւ����
/// </summary>
VOID Change(VOID)
{
	ChangeProc();  //����
	ChangeDraw();  //�`��

	return;
}

/// <summary>
/// �؂�ւ���ʂ̏���
/// </summary>
VOID ChangeProc(VOID)
{
	//�t�F�[�h�C��
	if (IsFadeIn == TRUE)
	{
		if (fadeInCnt > fadeInCntMAX)
		{
			fadeInCnt--;	//�J�E���^�����炷
		}
		else
		{
			//�t�F�[�h�C���������I�����

			fadeInCnt = fadeInCntInit;	//�J�E���^������
			IsFadeIn = FALSE;			//�t�F�[�h�C�������I��
		}

	}

	//�t�F�[�h�A�E�g
	if (IsFadeOut == TRUE)
	{
		if (fadeOutCnt < fadeOutCntMAX)
		{
			fadeOutCnt++;	//�J�E���^�𑝂₷
		}
		else
		{
			//�t�F�[�h�A�E�g�������I�����

			fadeOutCnt = fadeOutCntInit;	//�J�E���^������
			IsFadeOut = FALSE;				//�t�F�[�h�A�E�g�����I��
		}

	}

	//�؂�ւ������I��?
	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		//�t�F�[�h�C�����Ă��Ȃ��A�t�F�[�h�A�E�g�����Ă��Ȃ��Ƃ�
		GameScene = NextGameScene;	//���̃V�[���ɐ؂�ւ�
		OldGameScene = GameScene;	//�ȑO�̃Q�[���V�[���X�V
	}


	return;
}

/// <summary>
/// �؂�ւ���ʂ̕`��
/// </summary>
VOID ChangeDraw(VOID)
{
	//�ȑO�̃V�[����`��
	switch (OldGameScene)
	{
	case GAME_SCENE_TITLE:
		TitleDraw();	//�^�C�g����ʂ̕`��
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();		//�v���C��ʂ̕`��
		break;
	case GAME_SCENE_END:
		EndDraw();		//�G���h��ʂ̕`��
		break;
	default:
		break;
	}

	//�t�F�[�h�C��
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeInCnt / fadeInCntMAX) * 255);
	}

	//�t�F�[�h�A�E�g
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeOutCnt / fadeOutCntMAX) * 255);
	}

	//�l�p��`��
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0), TRUE);

	//�������I��
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


	DrawString(0, 16, "�؂�ւ����", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// �����蔻��̗̈�X�V
/// </summary>
/// <param name="coll">�����蔻��̗̈�</param>
VOID CollUpadateGollPlayer(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;					//�����蔻�������
	chara->coll.top = chara->img.y;						//�����蔻�������
	chara->coll.right = chara->img.x + chara->img.width -40;	//�����蔻�������
	chara->coll.bottom = chara->img.y + chara->img.height;	//�����蔻�������

	return;
}

/// <summary>
/// �����蔻��̗̈�X�V
/// </summary>
/// <param name="coll">�����蔻��̗̈�</param>
VOID CollUpadateGoll(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;					//�����蔻�������
	chara->coll.top = chara->img.y;						//�����蔻�������
	chara->coll.right = chara->img.x + chara->img.width - 40;	//�����蔻�������
	chara->coll.bottom = chara->img.y + chara->img.height;	//�����蔻�������

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
		a.left < b.right &&	//�@�Z�`A�̍���x���W < �Z�`B�̉E��x���W�@����
		a.top < b.bottom &&	//�@�Z�`A�̏��y���W < �Z�`B�̉���y���W�@����
		a.right > b.left &&	//�@�Z�`A�̉E��x���W > �Z�`B�̍���x���W�@����
		a.bottom > b.top	//�@�Z�`A�̉���y���W > �Z�`B�̏��y���W
		)
	{
		//�������Ă���Ƃ�

		return TRUE;
	}
	else
	{
		//�������Ă��Ȃ��Ƃ�
		return FALSE;
	}
}


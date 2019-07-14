
#pragma comment(lib,"dxguid")
#pragma once

#include <windows.h>
#include <string>
#include <dsound.h>
#include "myLib.h"




namespace DM 
{
	const  DWORD  kSTANDERDSIZE = 88200;	//	��T�C�Y�i�o�b�t�@�T�C�Y�̔����ɓ������j
	const  DWORD  kVOLUME_MIN = (DSBVOLUME_MIN + 5000);		//	DSBVOLUME_MIN�����������邽�߁A�����ŊǗ�

																//	�G���[�ԍ�
	namespace  Err {
		enum Type
		{
			WRG,		//	�t�@�C�����ُ�ł�
			CCIM,		//	DirectMusic�̍쐬�Ɏ��s
			CCIS,		//	DirectSound�̍쐬�Ɏ��s
			INIT,		//	Initialize�Ɏ��s
			SCL,		//	SetCooperativeLevel�Ɏ��s
			CSB,		//	�o�b�t�@�쐬�Ɏ��s
			QI,			//	QueryInterface�Ɏ��s
			SFMT,		//	�t�H�[�}�b�g�ݒ�Ɏ��s
			IA,			//	InitAudio�Ɏ��s
			CCILOADER,	//	���[�_�[�쐬�Ɏ��s
			MBTWC,		//	unicode�ϊ��Ɏ��s
			SSD,		//	�f�B���N�g���ݒ�Ɏ��s
			PSE,		//	�Z�O�����g�f�[�^���Đ��ł��Ȃ�
			INVALID,	//	�t�@�C�����ǂݍ��߂Ȃ�
			DL,			//	�_�E�����[�h�ł��Ȃ�
			UNLD,		//	�A�����[�h�ł��Ȃ�
			INVOPEN,	//	�t�@�C�����J���Ȃ�
			RIFF,		//	RIFF�`�����N���ǂݍ��߂Ȃ�
			FMT,		//	fmt�`�����N���ǂݍ��߂Ȃ�
			NOTPCM,		//	�ǂݍ��ݑΏۂ�WAVE�̓T�|�[�g����Ă��Ȃ��`��
			DATA,		//	data�`�����N�͑��݂��Ȃ�
			READERROR,	//	�ǂݍ��ݎ��s
			NOTHING		//	�G���[�i�V
		};
	}

	//	�G�t�F�N�g�t���O�ԍ�
	namespace Effect {
		enum Type
		{
			NOTHING = 0,	//	�G�t�F�N�g�͂Ȃ�
			FADEIN,			//	�t�F�[�h�C��
			FADEOUT,		//	�t�F�[�h�A�E�g
			STOP,			//	��~��
		};
	}


	//	�X�g���[�~���O�T�E���h���
	struct MYSOUNDINFO
	{
		//	�X�g���[�����
		char					szFileName[MAX_PATH];	//	�Đ��Ώۂ̃t�@�C����
		DWORD					dwReadPosition;			//	�ǂ݂���ł���t�@�C���|�W�V����
		DWORD					dwHeaderSize;			//	�ǂ݂���ł���WAVE�w�b�_�̃T�C�Y
		int						iStreamPos;				//	�������݂̑Ώۂ�\���i0�Ȃ�㔼�A1�Ȃ�O���j
		bool					bStream;				//	�X�g���[�~���O�Đ��������邩�ǂ���
														//	���[�v���
		bool					bLoop;					//	���[�v�����邩�ǂ���
		DWORD					dwReStart;				//	���[�v������Ƃ��A�ǂ��܂Ŗ߂邩
		DWORD					dwLoop;					//	�ǂ��܂ł����烋�[�v�����邩
		DWORD					dwCnt;					//	���񃋁[�v���J��Ԃ���
														//	�t�F�[�h�C���A�E�g����
		Effect::Type			effectflag;		//	�G�t�F�N�g�̎��
		int						iFade;			//	�t�F�[�h��
		long int				iVolume;		//	�{�����[��
		IDirectSoundBuffer8*	sec_ptr;	//	�Z�J���_���i��������́j�o�b�t�@
	};

	class Sound
	{
		Sound();
		bool  Initialize();
	public:
		enum  Kind { eStream, eSE };
		MYSOUNDINFO sinfo;
		typedef  weak_ptr<Sound>  WP;
		typedef  shared_ptr<Sound>  SP;
		~Sound();
		//BGM�������Ȃ��Đ�����ꍇ
		static  Sound::SP  CreateStream(const char*  szFileName);
		//���ʉ��i�r�d�j���Z���Ȃ��Đ�����ꍇ
		static  Sound::SP  CreateSE(const char*  szFileName);
		HRESULT SetVolume(long);			//	�{�����[���������s���܂�
		HRESULT SetFrequency(DWORD);		//	���g����ύX���܂�
		HRESULT SetPan(int);				//	�p���ݒ���s���܂�
		int GetVolume();					//	���݂̃{�����[���l���擾���܂�
		void FadeOut(int);				//	�t�F�[�h�A�E�g�����܂�
		void SetLoopInfo(int, int iLoopCnt = 0);	//	���[�v����ݒ肵�܂�
		HRESULT Play_Normal(bool bLoop);	//	�o�b�t�@���̃f�[�^���Đ����܂�
		HRESULT Play_FadeIn(DWORD, int);	//	�o�b�t�@���̃f�[�^���t�F�[�h�C�����čĐ����܂�
		void Check_Stream(DWORD  dwBufferSize_);
		HRESULT Stop();					//	�Đ����̃o�b�t�@���~�����܂�

	private:
		bool  CreateStream_Sub(const char*);
		bool  CreateSE_Sub(const char*);
		void LoadStream(int iWriteOffset, int iStreamPos);
		Err::Type WriteWaveStream(const char* szFileName,
			BYTE* data_ptr, int iOffset, DWORD dwSize, int iWriteOffset);
		void WriteSoundData(BYTE* data_ptr, int iOffset, int iSize);
		void WriteMute(int iSize);
		Err::Type LoadWaveStreamFirst(const char* szFileName,
			BYTE** data_ptr, DWORD dwSize, WAVEFORMATEX* wf_ptr);
		DWORD GetStatus();					//	�o�b�t�@�̏�Ԃ��擾���܂�
		DWORD GetCurrentPosition();
		Err::Type CreateSoundBuffer(WAVEFORMATEX* wf_ptr, DWORD dwBufbyte);

		///////////////////////////////////////////////////////////////////////////////////////
		Err::Type CreateSoundBuffer(int iNumber, WAVEFORMATEX* wf_ptr, DWORD dwBufbyte);
	};
	std::string GetErrorMessage(Err::Type number);// �G���[�ԍ����當������擾���܂�

	class Obj
	{
	public:
		typedef  weak_ptr<Obj>  WP;
		typedef  shared_ptr<Obj>  SP;
	private:
		HWND								wnd;
		vector<Sound::WP>					streams;
		static  Obj::WP  winst;		//	�P�������݂ł��Ȃ��C���X�^���X�̃E�B�[�N�|�C���^
		Obj();
		bool  Initialize(HWND wnd_, DWORD dwBufferSize);

	public:
		IDirectSound8*			ds_ptr;		//	DirectSound�I�u�W�F�N�g
		IDirectSoundBuffer*		pri_ptr;	//	�v���C�}���i���j�o�b�t�@
											//	�T�E���h�����
		DSCAPS					caps;			//	�f�o�C�X�\�͂��i�[
		DWORD					dwBufferSize;	//	�p�ӂ���o�b�t�@�̃T�C�Y
		~Obj();
		//	DI���C�u�����𐶐�����
		static  Obj::SP  Create(HWND  wnd_, DWORD dwBufferSize = kSTANDERDSIZE);
		static  Obj::SP  GetInst();
		HWND  Wnd() { return  wnd; }
		IDirectSound8&  DSound() { return  *ds_ptr; }
		//��
		//static  Err::Type InitDirectSound(HWND, DWORD dwBufferSize = kSTANDERDSIZE);	//	�T�E���h�������������܂�
		void GetDeviceCaps(DSCAPS*);			//	�f�o�C�X�̏����擾���܂�

		//	�X�g���[�~���O�Đ����Ď����܂�(���t���[���K���Ă�)
		void UpDate(void);
		//
		void Set(Sound::SP  s_);
	};
	//---------------------------------------------------------------------------
	//�T�E���h�}�l�[�W���[
	class  SoundManager {
	private:
		static  map<string, DM::Sound::SP>	arr;
		static  bool notObject;
		SoundManager() { notObject = false; }
	public:
		//�o�^
		static  void  Set(const string name_, DM::Sound::SP sp_);
		//���O�Ō���
		static  DM::Sound::SP  Get(const string name_);
		//�P�̔j��
		static  void  Clear(const string name_);
		//���ׂĔj��
		static  void  AllClear();
	};
}
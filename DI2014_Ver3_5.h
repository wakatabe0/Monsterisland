#pragma warning(disable:4996)
#pragma once
#include <windows.h>
#define DIRECTINPUT_VERSION	0x0800
#pragma comment(lib,"dxguid")
#pragma comment(lib,"dInput8")
#include <dInput.h>
#include "myLib.h"
#include <string>
#include <vector>

//����������������������������������������������������������������������������������������������
//�����͐���n�@�\�Q							�@�@��
//����������������������������������������������������������������������������������������������
namespace DI
{
	namespace  KS {
		enum Type {	//�L�[���	
			Err,
			On,		//�����Ă���Ԓ�
			Off,	//�����Ă���Ԓ�
			Down,	//����������i�P�t���[���̂ݔ����j
			Up,		//����������i�P�t���[���̂ݔ����j
		};
	}
	struct  ButtonInfo {
		bool  on = false;
		bool  off = true;
		bool  down = false;
		bool  up = false;
		//�{�^����ON�ɏ���������
		void  SetOn() {   on = 1; off = 0; down = 0; up = 0; }
		//�{�^����OFF�ɏ���������
		void  SetOff() {  on = 0; off = 1; down = 0; up = 0; }
		//�{�^����DOWN�ɏ���������
		void  SetDown() { on = 1; off = 0; down = 1; up = 0; }
		//�{�^����UP�ɏ���������
		void  SetUp() {   on = 0; off = 1; down = 0; up = 1; }
	};
	struct  StickInfo {
		float                angle;		//�A�i���O�X�e�B�b�N�̊p�x�iRT0�x�@DW90�x�̃��W�A���j
		float                volume;	//�A�i���O�X�e�B�b�N�̋���0.0f�`1.0f
		ML::Vec2             axis;		//�����̗�-1.0f�`+1.0f
		ButtonInfo			 L;
		ButtonInfo			 R;
		ButtonInfo			 U;
		ButtonInfo			 D;
		StickInfo() {
			this->angle = 0;
			this->volume = 0;
			this->axis = ML::Vec2(0,0);
			this->L.SetOff();
			this->R.SetOff();
			this->U.SetOff();
			this->D.SetOff();
		}
	};
	namespace  But {
		enum Type {	//�v���O������Ŏg�p�ł���L�[�̎��
			//��,
			B1,
			//�~
			B2,
			//��
			B3,
			//��
			B4,
			L1, R1, L2, R2,
			ST, SE, L3, R3,
			//��
			HL,		//�n�b�g�X�C�b�`
			//��
			HR,
			//��
			HU,
			//��
			HD,
			S0, S1, S2, S3, S4, S5, S6, S7, S8, S9,
			KMax		//�@�{�^������
		};
	}


	namespace GPB	//�Q�[���p�b�h�̃L�[��\��
	{
		enum Type {
			B01, B02, B03, B04, B05, B06, B07, B08,
			B09, B10, B11, B12, B13, B14, B15, B16,
			HSL, HSR, HSU, HSD,		//�n�b�g�X�C�b�` 
			KMax				//�@�{�^������
		};
	}

	struct  AnalogAxisKB {
		DWORD           vKeyL;	//�L�[�{�[�h�̃o�[�`�����L�[
		DWORD           vKeyR;	//�L�[�{�[�h�̃o�[�`�����L�[
		DWORD           vKeyU;	//�L�[�{�[�h�̃o�[�`�����L�[
		DWORD           vKeyD;	//�L�[�{�[�h�̃o�[�`�����L�[
	};
	//�L�[�ݒ�L�[�{�[�h�p
	struct  KD_KB {
		DWORD           vKey;	//�L�[�{�[�h�̃o�[�`�����L�[
		But::Type       pKey;	//�{�^���̎�ށi�o�f��ł̖��́j
	};
	typedef  vector<KD_KB>	KeyDatas_KB;

	//�L�[�ݒ�Q�[���p�b�h�p
	struct  KD_GP {
		GPB::Type		gKey;	//�Q�[���p�b�h�̃{�^��
		But::Type       pKey;	//�{�^���̎�ށi�o�f��ł̖��́j
	};
	typedef  vector<KD_GP>	KeyDatas_GP;

	//��O�Ƃ��ē�������G���[���
	namespace Err {
		enum Type
		{
			NOTHING,		//�G���[�Ȃ�
			CREATE,			//�������s
			NOTDEVICE,		//�f�o�C�X���擾�I�Ȃ�
			UNNONTYPE,		//�s���ȃf�o�C�X���
			ENUMDEVICE,		//�f�o�C�X�̗񋓂Ɏ��s
			CREATEDEVICE,	//�f�o�C�X�̐������s
			DATAFORMAT,		//�f�[�^�t�H�[�}�b�g�̐ݒ莸�s
			COOPLEVEL,		//�������x���ݒ莸�s
			GPAXIS,			//
			KBPROPERTY,
		};
	}
		//���z�R���g���[���\����
		struct  VGamePad {
			StickInfo  LStick;
			StickInfo  RStick;
			ButtonInfo  B1;
			ButtonInfo  B2;
			ButtonInfo  B3;
			ButtonInfo  B4;
			ButtonInfo  L1;
			ButtonInfo  L2;
			ButtonInfo  L3;
			ButtonInfo  R1;
			ButtonInfo  R2;
			ButtonInfo  R3;
			ButtonInfo  ST;
			ButtonInfo  SE;
			ButtonInfo  HL;
			ButtonInfo  HR;
			ButtonInfo  HU;
			ButtonInfo  HD;
			ButtonInfo  S0;
			ButtonInfo  S1;
			ButtonInfo  S2;
			ButtonInfo  S3;
			ButtonInfo  S4;
			ButtonInfo  S5;
			ButtonInfo  S6;
			ButtonInfo  S7;
			ButtonInfo  S8;
			ButtonInfo  S9;
		};


	class  GamePad
	{
	public:
		typedef  shared_ptr<GamePad>  SP;
		typedef  weak_ptr<GamePad>  WP;

	private:
		enum  DeviceKind { KeyBoard, GPad, UnNon };	//�f�o�C�X�̎��
		struct sKeyParam {
			DWORD             dKey;		//���ۂ̃{�^���܂��̓L�[�{�[�h�̃o�[�`�����L�[
			But::Type		  pKey;		//�{�^���̎�ށi�o�f��ł̖��́j
			KS::Type		  state;	//�L�[�̏��
		};
		IDirectInputDevice8*  device;
		DeviceKind            dk;
		vector<sKeyParam>     buttons;
		vector<DWORD>         kbAxisL;	//�L�[�{�[�h��p�����L�[
		vector<DWORD>         kbAxisR;	//�L�[�{�[�h��p�����L�[
		vector<DWORD>         kbPov4;	//�L�[�{�[�h��pPOV�L�[
		StickInfo		      ls, rs;
		SP  link;
	private:
		GamePad();
		bool  Initialize(
			DeviceKind    dk_,
			DWORD         id_,
			AnalogAxisKB* axisL_,
			AnalogAxisKB* axisR_,
			KeyDatas_KB		buttonKB_,
			KeyDatas_GP		buttonGP_);
		Err::Type  CreateDevice(DWORD  id_);

		void  CheckKB();
		void  CheckGP();
		void  CheckUnNon();

		//	�{�^���̏�Ԃ��m�F
		KS::Type  CheckBT(But::Type  kk_);
		//	�A�i���O�X�e�B�b�N�̓��͂𓾂�
		StickInfo  StickL();
		StickInfo  StickR();
		//
		static  SP  Create(
			DeviceKind		dk_,
			DWORD			id_,
			AnalogAxisKB*	axisL_,
			AnalogAxisKB*	axisR_,
			KeyDatas_KB		buttonKB_,
			KeyDatas_GP		buttonGP_);
	public:
		~GamePad();
		//	�f�o�C�X�𐶐�����
		static  SP  CreateKB(
			AnalogAxisKB	axisL_,
			AnalogAxisKB	axisR_,
			KeyDatas_KB		buttonKB_);
		static  SP  CreateGP(
			DWORD			id_,
			KeyDatas_GP		buttonGP_);
		//	�X�V
		void  UpDate();
		//�Q�̃f�o�C�X�𓝍�����
		//���S�����l����Ȃ�A�N���X���\�b�h��Link���g����
		SP  Link(SP  l) {
			if (this == l.get()) { return 0; }
			else { SP w = link; link = l; return link; }
		}
		//�Q�̃f�o�C�X�𓝍�����
		//�Е��̃f�o�C�X�������ł��Ă��Ȃ��ꍇ�̑Ή�����
		static  GamePad::SP  Link(SP  a, SP  b);

		//�Q�[���p�b�h�̃{�^���̏�Ԃ��擾����
		VGamePad  GetState();
	};
	//--------------------------------
	namespace MB	//�}�E�X�̃{�^����\��
	{
		enum Type {  //�}�E�X�{�^���̎�� 8�܂ŔF���\
			LB, RB, CB, RESERVED1,
			RESERVED2, RESERVED3, RESERVED4, RESERVED5,
			kMax
		};
	}
	//���z�}�E�X�\����
	struct  VMouse {
		POINT         pos;
		POINT         pos_Screen;
		int			  wheel;
		ButtonInfo	  LB;
		ButtonInfo	  RB;
		ButtonInfo	  CB;
		ButtonInfo	  B1;
		ButtonInfo	  B2;
		ButtonInfo	  B3;
		ButtonInfo	  B4;
		ButtonInfo	  B5;
	};
	class Mouse {
	private:
		//    IDirectInputDevice8*	device;
		//--------------------------------
		//�}�E�X
		LPDIRECTINPUTDEVICE8 mouse;
		DIMOUSESTATE2 mouseState;
		POINT		  pos_Screen;	//�X�N���[�����W
		POINT		  pos;			//�N���C�A���g���W
		long          mouseWheel;
		KS::Type      mouseButton[MB::kMax];
		Err::Type     init(float, float);
		float         scaleX, scaleY;
	public:
	private:
		KS::Type     CheckBT(MB::Type mb_);
		Mouse();
	public:
		~Mouse();
		typedef shared_ptr<Mouse> SP;
		typedef weak_ptr<Mouse> WP;
		//����
		static SP   Create(float scaleX_ = 1.f, float scaleY_ = 1.f);
		//	�X�V
		void UpDate();
		//
		VMouse  GetState();
	};
	class  Obj
	{
	public:
		typedef  weak_ptr<Obj>  WP;
		typedef  shared_ptr<Obj>  SP;
	private:
		IDirectInput8*  di;
		HWND  wnd;
		static  Obj::WP  winst;		//	�P�������݂ł��Ȃ��C���X�^���X�̃E�B�[�N�|�C���^
		Obj();
		bool  Initialize(HWND wnd_);

		static  vector<GamePad::WP>  gpads;
		static  vector<Mouse::WP>    mouses;

	public:
		~Obj();
		//	DI���C�u�����𐶐�����
		static  Obj::SP  Create(HWND  wnd_);
		static  Obj::SP  GetInst();
		HWND  Wnd() { return  wnd; }
		IDirectInput8&  DInput() { return  *di; }
		//���t���[���Ăяo��
		void UpDate();
		void Set(GamePad::SP s_);
		void Set(Mouse::SP s_);
	};

	//---------------------------------------------------------------------------
	//�Q�[���p�b�h�I�u�W�F�N�g
	//�o�^
	void  Managed_GamePadSet(const string name_, DI::GamePad::SP sp_);
	//���O�Ō���
	DI::GamePad::SP  Managed_GamePadGet(const string name_);
	//�P�̔j��
	void  Managed_GamePadClear(const string name_);
	//���ׂĔj��
	void  Managed_GamePadAllClear();
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	//�Q�[���p�b�h�}�l�[�W���[
	class  GamePadManager {
	private:
		static  map<string, DI::GamePad::SP>	arr;
		static  bool notObject;
		GamePadManager() { notObject = false; }
	public:
		//�o�^
		static  void  Set(const string name_, DI::GamePad::SP sp_);
		//���O�Ō���
		static  DI::GamePad::SP  Get(const string name_);
		//�P�̔j��
		static  void  Clear(const string name_);
		//���ׂĔj��
		static  void  AllClear();
	};
	//---------------------------------------------------------------------------
	//�}�E�X�}�l�[�W���[
	class  MouseManager {
	private:
		static  map<string, DI::Mouse::SP>	arr;
		static  bool notObject;
		MouseManager() { notObject = false; }
	public:
		//�o�^
		static  void  Set(const string name_, DI::Mouse::SP sp_);
		//���O�Ō���
		static  DI::Mouse::SP  Get(const string name_);
		//�P�̔j��
		static  void  Clear(const string name_);
		//���ׂĔj��
		static  void  AllClear();
	};
}
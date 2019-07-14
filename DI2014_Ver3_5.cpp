#include "DI2014_Ver3_5.h"


namespace DI
{
	const  int  analogMax  =  1000;		//�A�i���O���͂̃{�����[����-1000�`1000�Ƃ���
	const  float  digToAnalog  =  1.0f / analogMax;	//

	Obj::WP Obj::winst;	//	�P�������݂ł��Ȃ��C���X�^���X

	Obj::Obj( )
	{
		wnd = 0;
		di = nullptr;
	}
	bool Obj::Initialize(HWND wnd_)
	{
		wnd = wnd_;
		if(FAILED(DirectInput8Create(	GetModuleHandle(nullptr),
										DIRECTINPUT_VERSION,
										IID_IDirectInput8,
										(VOID**)&di,
										nullptr)))
		{
			//	�f�o�C�X�������s
			return false;
		}
		return true;
	}

	//���t���[���Ăяo��
	vector<GamePad::WP>			Obj::gpads;
	vector<Mouse::WP>			Obj::mouses;
	void Obj::Set(GamePad::SP s_)
	{
		gpads.push_back(s_);
	}
	void Obj::Set(Mouse::SP s_)
	{
		mouses.push_back(s_);
	}

	void Obj::UpDate()
	{
		{
			class cNullCheckGP {
			public:
				// �^�X�N�����ɉ������Ă���Ȃ�true��Ԃ�
				bool operator()(GamePad::WP t_) const {
					bool rtv = t_.expired();
					return  rtv;
				}
			};
			auto endIt = remove_if(this->gpads.begin(), this->gpads.end(), cNullCheckGP());
			this->gpads.erase(endIt, this->gpads.end());	//���ɏ������Ă�����̂�S�Ă��폜����
			for (auto it = gpads.begin(); it != gpads.end(); ++it) {
				auto  sp = it->lock();
				sp->UpDate();
			}
		}

		{
			class cNullCheckM {
			public:
				// �^�X�N�����ɉ������Ă���Ȃ�true��Ԃ�
				bool operator()(Mouse::WP t_) const {
					bool rtv = t_.expired();
					return  rtv;
				}
			};
			auto endIt = remove_if(this->mouses.begin(), this->mouses.end(), cNullCheckM());
			this->mouses.erase(endIt, this->mouses.end());	//���ɏ������Ă�����̂�S�Ă��폜����
			for (auto it = mouses.begin(); it != mouses.end(); ++it) {
				auto  sp = it->lock();
				sp->UpDate();
			}
		}
	}

	Obj::~Obj( )
	{
	//	DI�I�u�W�F�N�g�̉��
		ML::SafeRelease(&di);
	}
	//	DI���C�u�����𐶐�����
	Obj::SP Obj::Create(	HWND		hw_)	//	�E�B���h�E�n���h��
	{
		if(winst.expired( )){
			Obj::SP sp = Obj::SP(new Obj( ));
			if( sp->Initialize( hw_ ) ){
				winst = sp;
				return sp;
			}
		}
		return 0;
	}
	Obj::SP Obj::GetInst( )
	{
		if(!winst.expired( )){
			return winst.lock( );
		}
		return 0;
	}
	GamePad::GamePad(	)
	{
		device = nullptr;
	}
	bool GamePad::Initialize(	
			DeviceKind			dk_,
			DWORD				id_,
			AnalogAxisKB*	axisL_,
			AnalogAxisKB*	axisR_,
			KeyDatas_KB		buttonKB_,
			KeyDatas_GP		buttonGP_)
	{
		dk = dk_;
		StickInfo  w;

		Obj::SP dii = Obj::GetInst( );
		if(dii == 0){	return false; }
		Err::Type  e = this->CreateDevice(id_);
		//���̓f�o�C�X�̐����ɐ������Ȃ�����
		if(e != Err::NOTHING){
			dk = UnNon;
			return true;	//�d�l�ύX�ɂ��true�Ƃ���
		}	
		//�L�[�o�^
		if(axisL_){
			kbAxisL.push_back(axisL_->vKeyL);
			kbAxisL.push_back(axisL_->vKeyR);
			kbAxisL.push_back(axisL_->vKeyU);
			kbAxisL.push_back(axisL_->vKeyD);
		}
		if(axisR_){
			kbAxisR.push_back(axisR_->vKeyL);
			kbAxisR.push_back(axisR_->vKeyR);
			kbAxisR.push_back(axisR_->vKeyU);
			kbAxisR.push_back(axisR_->vKeyD);
		}

		for(DWORD c = 0;  c < buttonKB_.size(); c++)
		{
			sKeyParam w = { buttonKB_[c].vKey, buttonKB_[c].pKey, KS::Off };
			buttons.push_back(w);
		}
		for(DWORD c = 0;  c < buttonGP_.size(); c++)
		{
			sKeyParam w = { (DWORD)buttonGP_[c].gKey, buttonGP_[c].pKey, KS::Off };
			buttons.push_back(w);
		}

		return true;
	}
	GamePad::~GamePad( )
	{
		ML::SafeRelease(&device);
	}
	//	�f�o�C�X�𐶐�����
	GamePad::SP GamePad::Create(	
			DeviceKind			dk_,
			DWORD				id_,
			AnalogAxisKB*		axisL_,
			AnalogAxisKB*		axisR_,
			KeyDatas_KB			buttonKB_,
			KeyDatas_GP			buttonGP_)
	{
		auto sp = SP(new GamePad( ));
		if(sp){
			if( sp->Initialize(dk_,  id_,  axisL_,  axisR_,  buttonKB_,  buttonGP_) )
			{
				if (sp) {
					auto  ob = Obj::GetInst();
					ob->Set(sp);
				}
				return sp;
			}
		}
		return 0;
	}

	GamePad::SP GamePad::CreateKB(
				AnalogAxisKB		axisL_,
				AnalogAxisKB		axisR_,
				KeyDatas_KB			buttonKB_)
	{
		AnalogAxisKB*  aL = nullptr;
		AnalogAxisKB*  aR = nullptr;
		if(axisL_.vKeyL && axisL_.vKeyR && axisL_.vKeyU && axisL_.vKeyD){	aL = &axisL_;} 
		if(axisR_.vKeyL && axisR_.vKeyR && axisR_.vKeyU && axisR_.vKeyD){	aR = &axisR_;} 
		KeyDatas_GP		buttonGP_;
		return  Create(KeyBoard,  0,  aL,  aR, buttonKB_, buttonGP_);
	}
	GamePad::SP GamePad::CreateGP(
				DWORD				id_,
				KeyDatas_GP			buttonGP_)
	{
		KeyDatas_KB		buttonKB_;
		return Create(GPad,  id_,  nullptr,  nullptr, buttonKB_, buttonGP_);
	}
	GamePad::SP GamePad::Link(GamePad::SP a, GamePad::SP b)
	{
	//�Q�̃f�o�C�X�𓝍����Ďg����悤�ɂ���
		if(a != 0 && b != 0){	a->Link(b);	}	//�L�[�{�[�h�Z�@�Q�[���p�b�h�Z
		else if(a != 0){					}	//�L�[�{�[�h�Z�@�Q�[���p�b�hB2
		else if(b != 0){		a = b;		}	//�L�[�{�[�hB2�@�Q�[���p�b�h�Z
		else{								}	//�L�[�{�[�hB2�@�Q�[���p�b�hB2
		return a;
	}
	//	�X�V
	void GamePad::UpDate( )
	{
		switch(dk){		case KeyBoard:		CheckKB( );		break;
						case GPad:			CheckGP();		break;
						case UnNon:			CheckUnNon();	break;	}
		//�֘A�t�����ꂽ�҂�����ꍇ�A������X�V����
		//if(link != 0){	link->UpDate( );}
	}
	//�{�^���̏�Ԃ��X�V����
	ButtonInfo  UpdateButtonState2(bool  nbs_, ButtonInfo  old_)
	{
		ButtonInfo rtv;
		//�{�^���͉�����Ă���
		if (nbs_) {
			//���݂̃{�^���̏�Ԃɍ��킹�āA��Ԃ�ω�������
			if (true == old_.on) { 	rtv.SetOn();}
			else{					rtv.SetDown();}
		}
		//�{�^���͗�����Ă���
		else {
			if (true == old_.on) {	rtv.SetUp();}
			else{					rtv.SetOff();}
		}
		return rtv;
	}
	KS::Type  UpdateButtonState(bool  nbs_, KS::Type  old_)
	{
		if(nbs_){
			if (old_ == KS::Err){ old_ = KS::Off; }
			//�L�[���(�����n)
			switch(old_){
				case KS::Off:
				case KS::Up:
					old_ = KS::Down;	break;	//	�����Ă���&���������ゾ����
				case KS::On:
				case KS::Down:
					old_ = KS::On;	break;	//	�����Ă���&���������ゾ����
			}
		}else{
			//�L�[���(���n)
			switch(old_){
				case KS::Off:
				case KS::Up:
					old_ = KS::Off;	break;	//	�����Ă���&���������ゾ����
				case KS::On:
				case KS::Down:
					old_ = KS::Up;	break;	//	�����Ă���&���������ゾ����
			}
		}
		return old_;
	}
	void  SIUpdate(StickInfo&  si_, float  ax_, float  ay_)
	{
		si_.axis.x  =  ax_;
		si_.axis.y  =  ay_;

		//���͗ʂ����Ȃ��ꍇ�A�j���[�g�����Ƃ���
		if( -0.1f < si_.axis.x  &&  si_.axis.x < 0.1f &&
			-0.1f < si_.axis.y  &&  si_.axis.y < 0.1f ){
				si_.axis.x = 0;
				si_.axis.y = 0;
		}

		//�p�x�ƒ����ɕϊ������A�i���O�X�e�B�b�N����
		if(si_.axis.x  !=  0 || si_.axis.y  !=  0){//�X�e�B�b�N����X�������o�ł���
			//�E��0�x�A����90�x�Ƃ����p�x�����߂�
			si_.angle  =  atan2(si_.axis.y, si_.axis.x);//�X������p�x���Z�o
			//���o�[�̌X���ʂ́}0�`1�ɒu��������
			si_.volume  =  sqrt(si_.axis.x * si_.axis.x  +  si_.axis.y * si_.axis.y);
			si_.volume  =  min(si_.volume, 1.0f);
		}
		else{
			si_.volume  =  0.0f;
		}
		//�S�����{�^���̃C���[�W�Ń}�b�s���O
		si_.L = UpdateButtonState2(ax_ < -0.1f, si_.L);
		si_.R = UpdateButtonState2(ax_ > +0.1f, si_.R);
		si_.U = UpdateButtonState2(ay_ < -0.1f, si_.U);
		si_.D = UpdateButtonState2(ay_ > +0.1f, si_.D);
	}
	void GamePad::CheckKB( )
	{
		BYTE keyStatus[256];
		ZeroMemory(keyStatus, 256);
		//�f�o�C�X���g�p�\���m�F����
		int acquireCnt = 0;
		BOOL brFlag = FALSE;
		if(FAILED(device->Poll( )))
		{
			do
			{
				if(SUCCEEDED(device->Acquire( ))){		brFlag = TRUE;	}
				if(++acquireCnt > 30){					return;		}
			}while(brFlag == FALSE);
		}
		//	�L�[�{�[�h���擾
		if(FAILED(device->GetDeviceState(sizeof(BYTE) * 256, &keyStatus))){	return;	}
		//	�������͂ɓo�^���ꂽ�L�[�ւ̑Ή�
		float  ax_L=0,  ay_L=0;
		if(kbAxisL.size( )==4){
			if(keyStatus[kbAxisL[0]] != 0){	ax_L -= 1;}
			if(keyStatus[kbAxisL[1]] != 0){	ax_L += 1;}
			if(keyStatus[kbAxisL[2]] != 0){	ay_L -= 1;}
			if(keyStatus[kbAxisL[3]] != 0){	ay_L += 1;}
		}
		float  ax_R=0,  ay_R=0;
		if(kbAxisR.size( )==4){
			if(keyStatus[kbAxisR[0]] != 0){	ax_R -= 1;}
			if(keyStatus[kbAxisR[1]] != 0){	ax_R += 1;}
			if(keyStatus[kbAxisR[2]] != 0){	ay_R -= 1;}
			if(keyStatus[kbAxisR[3]] != 0){	ay_R += 1;}
		}

		//LR�X�e�B�b�N���X�V
		SIUpdate(ls,  ax_L,  ay_L);
		SIUpdate(rs,  ax_R,  ay_R);
		//�@�{�^���Ƃ��ēo�^���ꂽ�L�[�ւ̑Ή�
		for(auto it = begin(buttons); it != end(buttons); ++it){
			it->state  =  UpdateButtonState(keyStatus[it->dKey] != 0,  it->state);
		}
	}
	
	bool  KeyCheck(KS::Type  state_, KS::Type  m_)
	{
		namespace  KS = KS;
		//�L�[�ƍ�
		if(	(m_ == KS::Down  &&  state_ == KS::Down)  ||
			(m_ == KS::Up    &&  state_ == KS::Up)    ||
			(m_ == KS::Off   &&  (state_ == KS::Up   || state_ == KS::Off))  ||
			(m_ == KS::On    &&  (state_ == KS::Down || state_ == KS::On))   ){
			return true;
		}
		return false;
	}






	StickInfo  GamePad::StickL()
	{
		StickInfo  me = this->ls;
		StickInfo  linkRtv;
		if(link != nullptr){	
			linkRtv = link->StickL( );
			if(me.volume  <  linkRtv.volume){
				return linkRtv;
			}
		}
		return me;
	}
	StickInfo  GamePad::StickR()
	{
		StickInfo  me = this->rs;
		StickInfo  linkRtv;
		if(link != nullptr){	
			linkRtv = link->StickR( );
			if(me.volume  <  linkRtv.volume){	return linkRtv;}
		}
		return me;
	}
	void  GamePad::CheckUnNon()
	{
		//�A�i���O�X�e�B�b�N�Ή�
		SIUpdate(ls, 0, 0);
		SIUpdate(rs, 0, 0);

		//�{�^���ւ̑Ή�
		for (auto it = begin(buttons); it != end(buttons); ++it)
		{
			it->state = UpdateButtonState(false, it->state);
		}
	}


	void GamePad::CheckGP( )
	{
		DIJOYSTATE2 js;	//	�L�[�����i�[����
		//�f�o�C�X���g�p�\���m�F����
		int acquireCnt = 0;
		BOOL brFlag = FALSE;
		if(FAILED(device->Poll( )))
		{
			do
			{
				if(SUCCEEDED(device->Acquire( ))){		brFlag = TRUE;		}
				if(++acquireCnt > 30){					return;				}
			}while(brFlag == FALSE);
		}

		//	�p�b�h���擾
		if(FAILED(device->GetDeviceState(sizeof(DIJOYSTATE2), &js)))
		{
			return;
		}
		//�A�i���O�X�e�B�b�N�Ή�
		SIUpdate(ls,  js.lX * digToAnalog,  js.lY  * digToAnalog);
		SIUpdate(rs,  js.lZ * digToAnalog,  js.lRz * digToAnalog);


		//POV�Ή�
		int m = 8;
		if(js.rgdwPOV[0] != 0xFFFFFFFF){	m = js.rgdwPOV[0] / 4500;}//0-7
		bool xym[9][4] = {
		//   L R U D
			{0,0,1,0},		//0  UP
			{0,1,1,0},		//1  UPRT
			{0,1,0,0},		//2  RT
			{0,1,0,1},		//3  DWRT
			{0,0,0,1},		//4  DW
			{1,0,0,1},		//5  DWLE
			{1,0,0,0},		//6  LE
			{1,1,0,0},		//7  UPLE
			{0,0,0,0},		//8  NT
		};
		//�{�^���ւ̑Ή�
		bool keyFlag;
		for(auto it = begin(buttons); it != end(buttons); ++it)
		{
			//�L�[�R�[�h�ɑΉ������f�[�^�ύX�������s��
			keyFlag = false;
			//POV�Ή�
			if(		it->dKey == GPB::HSL){		keyFlag = xym[m][0];}
			else if(it->dKey == GPB::HSR){		keyFlag = xym[m][1];}
			else if(it->dKey == GPB::HSU){		keyFlag = xym[m][2];}
			else if(it->dKey == GPB::HSD){		keyFlag = xym[m][3];}
			//�{�^���O�O�`�P�S
			else if(it->dKey >= GPB::B01 &&	
					it->dKey <= GPB::B16){		keyFlag = js.rgbButtons[it->dKey] != 0;}
			//���Ή��̃L�[���ݒ肳��Ă���
			else{	continue;	}
			it->state  =  UpdateButtonState(keyFlag,  it->state);
		}
	}
	ButtonInfo  ConvertKS(DI::KS::Type  ks_)
	{
		ButtonInfo  rtv;
		switch (ks_) {
		default:
		case DI::KS::Off:
			rtv.SetOff();
			break;
		case DI::KS::Down:
			rtv.SetDown();
			break;
		case DI::KS::On:
			rtv.SetOn();
			break;
		case DI::KS::Up:
			rtv.SetUp();
			break;
		}
		return rtv;
	}
	VGamePad  GamePad::GetState()
	{
		VGamePad  rtv;
		rtv.LStick = this->StickL();
		rtv.RStick = this->StickR();
		rtv.B1 = ConvertKS(this->CheckBT(DI::But::B1));
		rtv.B2 = ConvertKS(this->CheckBT(DI::But::B2));
		rtv.B3 = ConvertKS(this->CheckBT(DI::But::B3));
		rtv.B4 = ConvertKS(this->CheckBT(DI::But::B4));
		
		rtv.L1 = ConvertKS(this->CheckBT(DI::But::L1));
		rtv.L2 = ConvertKS(this->CheckBT(DI::But::L2));
		rtv.L3 = ConvertKS(this->CheckBT(DI::But::L3));
		
		rtv.R1 = ConvertKS(this->CheckBT(DI::But::R1));
		rtv.R2 = ConvertKS(this->CheckBT(DI::But::R2));
		rtv.R3 = ConvertKS(this->CheckBT(DI::But::R3));
		
		rtv.ST = ConvertKS(this->CheckBT(DI::But::ST));
		rtv.SE = ConvertKS(this->CheckBT(DI::But::SE));
		
		rtv.HL = ConvertKS(this->CheckBT(DI::But::HL));
		rtv.HR = ConvertKS(this->CheckBT(DI::But::HR));
		rtv.HU = ConvertKS(this->CheckBT(DI::But::HU));
		rtv.HD = ConvertKS(this->CheckBT(DI::But::HD));
		
		rtv.S0 = ConvertKS(this->CheckBT(DI::But::S0));
		rtv.S1 = ConvertKS(this->CheckBT(DI::But::S1));
		rtv.S2 = ConvertKS(this->CheckBT(DI::But::S2));
		rtv.S3 = ConvertKS(this->CheckBT(DI::But::S3));
		rtv.S4 = ConvertKS(this->CheckBT(DI::But::S4));
		rtv.S5 = ConvertKS(this->CheckBT(DI::But::S5));
		rtv.S6 = ConvertKS(this->CheckBT(DI::But::S6));
		rtv.S7 = ConvertKS(this->CheckBT(DI::But::S7));
		rtv.S8 = ConvertKS(this->CheckBT(DI::But::S8));
		rtv.S9 = ConvertKS(this->CheckBT(DI::But::S9));
		return rtv;
	}


	//�Q�̃L�[�𓝍��������A�œ|�ȏ�Ԃ�Ԃ�
	KS::Type  IntegrationKey(KS::Type  a_, KS::Type  b_)
	{
		namespace KS = KS;
		KS::Type ret = KS::Off;

		KS::Type  mat[5][5] = {
			//Err  On  Off   Down  Up
			{KS::Err,  KS::On, KS::Off,  KS::Down, KS::Up}, //Err
			{KS::On,   KS::On, KS::On,   KS::On,   KS::On}, //On
			{KS::Off,  KS::On, KS::Off,  KS::Down, KS::Up}, //Off
			{KS::Down, KS::On, KS::Down, KS::Down, KS::On}, //Down
			{KS::Up,   KS::On, KS::Up,   KS::On,   KS::Up}, //Up
		};
		return  mat[a_][b_];
	}
	KS::Type	 GamePad::CheckBT(But::Type		kk_)
	{
		KS::Type ret = KS::Off;

		//�L�[�z��̒�����ΏۂɂȂ�L�[�����o����
		for(auto it = begin(buttons); it != end(buttons); ++it)
		{
			if(kk_ == it->pKey)		//�o�^�ς݂̃L�[�ꗗ����Y������L�[�����o����
			{
				ret = (KS::Type)it->state;
			}
			if(link != 0){
				ret = IntegrationKey(ret, link->CheckBT(kk_));
			}
		}
		return ret;
	}





	static GUID					a_guid;
	static int					a_id;
	BOOL CALLBACK diEnumDevice(		const DIDEVICEINSTANCE* inDevice,
									void* inContext)
	{
		//�f�o�C�X�ԍ�
		DWORD* num = (DWORD*)inContext;
		//�w�肳�ꂽ�ԍ��̃f�o�C�X�ɓ��B
		if((*num) == a_id)
		{
			a_guid = inDevice->guidInstance;	//�f�t�h�c��ۑ�
			(*num)++;
			return DIENUM_STOP;					//�f�o�C�X�̗񋓂��I������
		}
		else
		{
			(*num)++;
			return DIENUM_CONTINUE;			//���̃f�o�C�X�̃`�F�b�N�Ɉڂ�
		}
	}
	BOOL CALLBACK enumObjectsGamePad(	const DIDEVICEOBJECTINSTANCE* inDevice,
										void* inContext)
	{
		IDirectInputDevice8* did = (IDirectInputDevice8*)inContext;
		DIPROPRANGE diprg; 
		//�f�o�C�X�̓��͂͐�Ύ������Ύ��ł���
		if(inDevice->dwType&DIDFT_AXIS)
		{
			diprg.diph.dwSize = sizeof(DIPROPRANGE);
			diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			diprg.diph.dwHow = DIPH_BYID;
			diprg.diph.dwObj = inDevice->dwType;
			diprg.lMin = -analogMax;
			diprg.lMax =  analogMax;
			//	�f�o�C�X�̃^�C�v��ݒ肷��
			did->SetProperty(DIPROP_RANGE, &diprg.diph);
		}
		else
		{
			MessageBox(nullptr, "���̃^�C�v�̃R���g���[���͑Ή����Ă��܂���", nullptr, MB_OK);
		}
		return DIENUM_CONTINUE;
	}
	Err::Type GamePad::CreateDevice(DWORD id_)
	{
		Obj::SP dii = Obj::GetInst( );
		if(dii == 0){	return Err::NOTDEVICE; }
		if(!(	dk == KeyBoard ||
				dk == GPad ) ){
			return Err::UNNONTYPE;
		}

		struct {
			DWORD				dType;
			LPCDIDATAFORMAT		dFormat;
			DWORD				cLevel;

		}kind[] = {
			//GamePad::KeyBoard
			{DI8DEVCLASS_KEYBOARD, &c_dfDIKeyboard,  DISCL_NONEXCLUSIVE | DISCL_FOREGROUND},
			//GamePad::GamePad
			{DI8DEVCLASS_GAMECTRL, &c_dfDIJoystick2, DISCL_EXCLUSIVE	| DISCL_FOREGROUND},
		};

	//�f�o�C�X�̌��o���s��----------------------------------------------------------
		//�w��f�o�C�X���o�i�O���[�o���ϐ��@a_guid�@����с@a_id�@�ɒ��Ӂj
		DWORD deviceCount = 0;
		a_id = id_;
		if(FAILED(  dii->DInput( ).EnumDevices(
							kind[dk].dType,  diEnumDevice,
							&deviceCount, DIEDFL_ATTACHEDONLY))){		return Err::ENUMDEVICE;	}
		if(dk == GPad  &&  deviceCount <= id_){							return Err::ENUMDEVICE;	}

	//���̓f�o�C�X�𐶐�����
		if(FAILED(dii->DInput( ).CreateDevice( a_guid, &device, nullptr))){		return Err::CREATEDEVICE;	}
	//���̓f�o�C�X�̃t�H�[�}�b�g��ݒ肷��
		if(FAILED(device->SetDataFormat(kind[dk].dFormat))){					return Err::DATAFORMAT;		}
	//���̓f�o�C�X�̋������x����ݒ肷��
		if(FAILED(device->SetCooperativeLevel(dii->Wnd( ), kind[dk].cLevel))){	return Err::COOPLEVEL;		}

	//���̓Q�[���p�b�h�̏ꍇ�A�����L�[�ɐ�Ύ��������͑��Ύ����g�p�\�Ȃ��Ǝ����m�F����B
		switch(dk)
		{
			case GPad:
				//��Ύ�����ё��Ύ����g�p�\�ȏꍇ�A���̐ݒ���s��
				if(FAILED(device->EnumObjects(	enumObjectsGamePad,
												device,
												DIDFT_AXIS)))
				{
					return Err::GPAXIS;
				}
				break;
			case KeyBoard:
				{
					DIPROPDWORD dipdw;
					dipdw.diph.dwSize		= sizeof(DIPROPDWORD);
					dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
					dipdw.diph.dwObj		= 0;
					dipdw.diph.dwHow		= DIPH_DEVICE;
					dipdw.dwData			= 8;//�����ɔ���ł���L�[�̐��ɂȂ�̂��ȁH
					if(FAILED(device->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
					{
						return Err::KBPROPERTY;
					}
				}			
				break;
		}
		return Err::NOTHING;
	}
	Err::Type Mouse::init(float scaleX_, float scaleY_)
	{
		Obj::SP dii = Obj::GetInst();
		if (dii == 0){ return Err::NOTDEVICE; }

		//--------------------------------
		//DirectMouse
		if (FAILED(dii->DInput().CreateDevice(GUID_SysMouse, &mouse, nullptr))){ return Err::CREATEDEVICE; }
		if (FAILED(mouse->SetDataFormat(&c_dfDIMouse2))){ return Err::DATAFORMAT; } // �}�E�X�p�̃f�[�^�E�t�H�[�}�b�g��ݒ�
		mouse->SetCooperativeLevel(dii->Wnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);//���x���ݒ�
		// �f�o�C�X�̐ݒ�    
		DIPROPDWORD diprop;
		diprop.diph.dwSize = sizeof(diprop);
		diprop.diph.dwHeaderSize = sizeof(diprop.diph);
		diprop.diph.dwObj = 0;
		diprop.diph.dwHow = DIPH_DEVICE;
		diprop.dwData = DIPROPAXISMODE_REL; // ���Βl���[�hDIPROPAXISMODE_REL�Őݒ�i��Βl��DIPROPAXISMODE_ABS�j    
		mouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);

		// ���͐���J�n    
		mouse->Acquire();
		//--------------------------------
		scaleX = scaleX_;
		scaleY = scaleY_;

		return Err::NOTHING;
	}

	Mouse::SP Mouse::Create(float scaleX_, float scaleY_)
	{
		auto sp = SP(new Mouse());
		if (sp){
			if (sp->init(scaleX_, scaleY_) != Err::NOTHING){
				MessageBox(nullptr, _T("�}�E�X������܂����I"), nullptr, MB_OK);
				return nullptr;
			}
			else{
				auto  ob = Obj::GetInst();
				ob->Set(sp);
			}
		}
		return sp;
	}
	Mouse::Mouse()
	{
	}

	Mouse::~Mouse()
	{
		if (mouse != NULL)
		{
			mouse->Unacquire();
			ML::SafeRelease(&mouse);
		}
	}

	void Mouse::UpDate()
	{
		//--------------------------------
		//add koyanagi
		// �l�̍X�V
		ZeroMemory(&mouseState, sizeof(DIMOUSESTATE2));
		if (FAILED(mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState))){
			mouse->Acquire();
		}
		//�X�N���[�����W�擾
		GetCursorPos(&this->pos_Screen);
		//�N���C�A���g���W�Z�o
		this->pos = this->pos_Screen;
		Obj::SP dii = Obj::GetInst();
		ScreenToClient(dii->Wnd(), &this->pos);
		this->pos.x = (LONG)(this->pos.x / scaleX);
		this->pos.y = (LONG)(this->pos.y / scaleY);
		//�z�C�[��
		mouseWheel = mouseState.lZ;
		//�{�^���n
		for (int i = 0; i < MB::kMax; i++)
		{
			if ((mouseState.rgbButtons[i] & 0x80) >> 7){
				if (mouseButton[i] == KS::Off || mouseButton[i] == KS::Up)  mouseButton[i] = KS::Down;
				else  mouseButton[i] = KS::On;
			}
			else{
				if (mouseButton[i] == KS::On || mouseButton[i] == KS::Down)  mouseButton[i] = KS::Up;
				else  mouseButton[i] = KS::Off;
			}
		}
		//--------------------------------
	}
	//
	VMouse  Mouse::GetState()
	{
		VMouse  rtv;
		rtv.pos = this->pos;
		rtv.pos_Screen = this->pos_Screen;
		rtv.wheel = this->mouseWheel;
		rtv.LB = ConvertKS(this->CheckBT(DI::MB::LB));
		rtv.RB = ConvertKS(this->CheckBT(DI::MB::RB));
		rtv.CB = ConvertKS(this->CheckBT(DI::MB::CB));

		rtv.B1 = ConvertKS(this->CheckBT(DI::MB::RESERVED1));
		rtv.B2 = ConvertKS(this->CheckBT(DI::MB::RESERVED2));
		rtv.B3 = ConvertKS(this->CheckBT(DI::MB::RESERVED3));
		rtv.B4 = ConvertKS(this->CheckBT(DI::MB::RESERVED4));
		rtv.B5 = ConvertKS(this->CheckBT(DI::MB::RESERVED5));
		return  rtv;
	}
	KS::Type Mouse::CheckBT(MB::Type mb)
	{
		return mouseButton[mb];
	}

	//-----------------------------------------------------------------------
	//GamePad�n
	//-----------------------------------------------------------------------
	map<string, DI::GamePad::SP>	GamePadManager::arr;
	bool GamePadManager::notObject;
	void  GamePadManager::Set(const string name_, DI::GamePad::SP sp_)
	{
		arr[name_] = sp_;
	}
	//-----------------------------------------------------------------------
	DI::GamePad::SP  GamePadManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("GamePadManager �Ώۂ����݂��܂���"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  GamePadManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void GamePadManager::AllClear()
	{
		arr.clear();
	}
	//-----------------------------------------------------------------------
	//Mouse�n
	//-----------------------------------------------------------------------
	map<string, DI::Mouse::SP>	MouseManager::arr;
	bool MouseManager::notObject;
	void  MouseManager::Set(const string name_, DI::Mouse::SP sp_)
	{
		arr[name_] = sp_;
	}
	//-----------------------------------------------------------------------
	DI::Mouse::SP  MouseManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("MouseManager �Ώۂ����݂��܂���"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  MouseManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void MouseManager::AllClear()
	{
		arr.clear();
	}
	//---------------------------------------------------------------------------
}


#include "library.h"
#include "dm2008_Ver1_4.h"


namespace  DM
{

	Obj::WP Obj::winst;	//	�P�������݂ł��Ȃ��C���X�^���X
	Obj::Obj()
	{
		this->wnd = 0;
		this->ds_ptr = nullptr;
		this->dwBufferSize = 0;
		this->pri_ptr = 0;
		ZeroMemory(&this->caps, sizeof(DSCAPS));
	}
	bool Obj::Initialize(HWND wnd_, DWORD dwBufferSize)
	{
		wnd = wnd_;


		//	DirectSound�̏�����
		if (FAILED(CoCreateInstance(CLSID_DirectSound8, NULL, CLSCTX_INPROC,
			IID_IDirectSound8, (void **)&this->ds_ptr)))
		{
			return false;	// return Err::CCIS;
		}
		if (FAILED(this->ds_ptr->Initialize(NULL)))
		{
			return false;	// return Err::INIT;
		}
		if (FAILED(this->ds_ptr->SetCooperativeLevel(wnd, DSSCL_PRIORITY)))
		{
			return false;	// return Err::SCL;
		}
		//	�v���C�}���T�E���h�o�b�t�@���쐬
		DSBUFFERDESC desc =
		{
			sizeof(DSBUFFERDESC)
		};

		desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
		if (FAILED(this->ds_ptr->CreateSoundBuffer(&desc, &this->pri_ptr, NULL)))
		{
			return false;	// return Err::CSB;
		}
		//	���̏�����
		Obj::ds_ptr->GetCaps(&this->caps);
		Obj::dwBufferSize = dwBufferSize;
		//return Err::NOTHING;
		return true;
	}

	Obj::~Obj()
	{
		//	DirectSound
		kREL(this->pri_ptr);
		kREL(this->ds_ptr);
	}
	//	DM���C�u�����𐶐�����
	Obj::SP Obj::Create(HWND		hw_, DWORD dwBufferSize)	//	�E�B���h�E�n���h��
	{
		if (winst.expired()){
			Obj::SP sp = Obj::SP(new Obj());
			if (sp->Initialize(hw_, dwBufferSize)){
				winst = sp;
				return sp;
			}
		}
		return 0;
	}
	Obj::SP Obj::GetInst()
	{
		if (!winst.expired()){
			return winst.lock();
		}
		return 0;
	}



	//--------------------------------------------------------------------------------------------
	void Obj::Set(shared_ptr<class  Sound>  s_)
	{
		this->streams.push_back(s_);
	}
	void Obj::UpDate(void)
	{
		class cNullCheck{
		public:
			// �^�X�N�����ɉ������Ă���Ȃ�true��Ԃ�
			bool operator()(Sound::WP t_) const {
				bool rtv = t_.expired();
				return  rtv;
			}
		};
		auto endIt = remove_if(this->streams.begin(), this->streams.end(), cNullCheck());
		this->streams.erase(endIt, this->streams.end());	//���ɏ������Ă�����̂�S�Ă��폜����

		//	�ǂݍ��݃^�C�~���O�𒲂ׂ�
		for (auto it = this->streams.begin();
			it != this->streams.end();
			it++)
		{
			auto sp = it->lock();
			sp->Check_Stream(this->dwBufferSize);
		}

	}
	//--------------------------------------------------------------------------------------------
	void Obj::GetDeviceCaps(DSCAPS* caps_ptr)
	{
		memcpy(caps_ptr, &this->caps, sizeof(DSCAPS));
	}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------
	Err::Type Sound::CreateSoundBuffer(WAVEFORMATEX* wf_ptr, DWORD dwBufbyte)
	{
		DSBUFFERDESC desc;
		LPDIRECTSOUNDBUFFER lpTempBuff = NULL;
		Err::Type hRet = Err::NOTHING;		//	�֐��߂�l

		//	�T�E���h�f�[�^
		if (!wf_ptr)
		{
			return Err::SFMT;
		}
		//	�T�E���h�o�b�t�@�̐ݒ�
		ZeroMemory(&desc, sizeof(desc));
		desc.lpwfxFormat = wf_ptr;
		desc.dwSize = sizeof(DSBUFFERDESC);
		desc.dwFlags = DSBCAPS_STATIC | DSBCAPS_LOCDEFER |
			DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN;
		desc.dwBufferBytes = dwBufbyte;	//	�o�b�t�@�T�C�Y
		//	DirectSoundBuffer����
		auto  dm = Obj::GetInst();
		if (FAILED(dm->ds_ptr->CreateSoundBuffer(&desc, &lpTempBuff, NULL)))
		{
			hRet = Err::CSB;
			goto end;
		}
		//	DirectSoundBuffer8�擾
		if (FAILED(lpTempBuff->QueryInterface(IID_IDirectSoundBuffer8, (void**)&this->sinfo.sec_ptr)))
		{
			hRet = Err::QI;
			goto end;
		}
	end:
		kREL(lpTempBuff);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	Sound::SP  Sound::CreateSE(const char* szFileName)
	{
		auto sp = SP(new Sound());
		if (sp){
			bool  rtv;
			rtv = sp->CreateSE_Sub(szFileName);
			if (rtv){
				auto  ob = Obj::GetInst();
				return sp;
			}
		}
		return sp;
		//return 0;
	}
	//--------------------------------------------------------------------------------------------
	Sound::SP  Sound::CreateStream(const char* szFileName)
	{
		auto sp = SP(new Sound());
		if (sp){
			bool  rtv;
			rtv = sp->CreateStream_Sub(szFileName);
			if (rtv){
				auto  ob = Obj::GetInst();
				ob->Set(sp);
				return sp;
			}
		}
		return sp;
		//return 0;
	}
	//--------------------------------------------------------------------------------------------
	bool Sound::CreateSE_Sub(const char* szFileName)
	{
		WAVEFORMATEX wf;
		BYTE* data_ptr = NULL;
		bool hRet = true;		//	�֐��߂�l

		kREL(this->sinfo.sec_ptr);
		ZeroMemory(&wf, sizeof(WAVEFORMATEX));
		//	DirectSoundBuffer�ݒ�
		if (Err::NOTHING != this->LoadWaveStreamFirst(szFileName, &data_ptr, 0, &wf))
		{
			hRet = false;// = Err::WRG;
			goto end;
		}
		//	�o�b�t�@�̐���
		if (Err::NOTHING != this->CreateSoundBuffer(&wf, this->sinfo.dwReadPosition - this->sinfo.dwHeaderSize))
		{
			goto end;
		}
		//	�o�b�t�@���b�N
		this->WriteSoundData(data_ptr, 0, this->sinfo.dwReadPosition - this->sinfo.dwHeaderSize);
		//	�T�E���h�����i�[
		strcpy_s(this->sinfo.szFileName, MAX_PATH, szFileName);	//	�T�E���h�t�@�C�������i�[
		this->sinfo.iStreamPos = 0;
		this->sinfo.bLoop = false;
		this->sinfo.bStream = false;
	end:
		verDelete_Vector(data_ptr);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	bool Sound::CreateStream_Sub(const char* szFileName)
	{
		BYTE* data_ptr = NULL;
		WAVEFORMATEX wf;
		bool  hRet = true;// Err::NOTHING;		//	�֐��߂�l

		auto  ds = Obj::GetInst();


		kREL(this->sinfo.sec_ptr);
		//	DirectSoundBuffer�ݒ�
		if (Err::NOTHING != this->LoadWaveStreamFirst(szFileName, &data_ptr, ds->dwBufferSize, &wf))
		{
			hRet = false;// Err::WRG;
			goto end;
		}
		//	�o�b�t�@�̐���
		if (Err::NOTHING != this->CreateSoundBuffer(&wf, ds->dwBufferSize * 2))
		{
			goto end;
		}
		//	�o�b�t�@���b�N
		this->WriteSoundData(data_ptr, 0, ds->dwBufferSize);
		//	�T�E���h�����i�[
		strcpy_s(this->sinfo.szFileName, MAX_PATH, szFileName);	//	�T�E���h�t�@�C�������i�[
		this->sinfo.iStreamPos = 0;
		this->sinfo.bStream = true;
	end:
		verDelete_Vector(data_ptr);
		return hRet;
	}

	//--------------------------------------------------------------------------------------------
	Sound::Sound()
	{
		this->sinfo.sec_ptr = nullptr;
		this->sinfo.bLoop = false;
		this->sinfo.bStream = false;
		this->sinfo.dwCnt = 0;
		this->sinfo.dwHeaderSize = 0;
		this->sinfo.dwLoop = 0;
		this->sinfo.dwReadPosition = 0;
		this->sinfo.dwReStart = 0;
		this->sinfo.iVolume = DSBVOLUME_MAX;
	}
	//--------------------------------------------------------------------------------------------
	Sound::~Sound()
	{
		kREL(this->sinfo.sec_ptr);
	}
	//--------------------------------------------------------------------------------------------
	DWORD Sound::GetCurrentPosition()
	{
		return this->sinfo.dwReadPosition;
	}
	//--------------------------------------------------------------------------------------------
	DWORD Sound::GetStatus( )
	{
		DWORD dwStatus;

		this->sinfo.sec_ptr->GetStatus(&dwStatus);
		return dwStatus;
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::Play_FadeIn(DWORD dwPlayFlag, int iValue)
	{
		HRESULT hRet;

		//	����������Ă��Ȃ��ꍇ�͍Đ����Ȃ�
		if (this->sinfo.sec_ptr == NULL)
		{
			return DSERR_INVALIDCALL;
		}
		hRet = this->sinfo.sec_ptr->Play(0, 0, dwPlayFlag);
		this->SetVolume(kVOLUME_MIN);
		this->sinfo.iVolume = kVOLUME_MIN;
		this->sinfo.effectflag = Effect::FADEIN;
		this->sinfo.iFade = iValue;
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::Play_Normal(bool bLoop)
	{
		HRESULT hRet;
		DWORD dwPlayFlag = 0;

		//	����������Ă��Ȃ��ꍇ�͍Đ����Ȃ�
		if (this->sinfo.sec_ptr == NULL)
		{
			return DSERR_INVALIDCALL;
		}
		//	�Đ�
		this->sinfo.sec_ptr->SetCurrentPosition(0);
		if (bLoop || this->sinfo.bStream)
		{
			dwPlayFlag = DSBPLAY_LOOPING;
		}
		hRet = this->sinfo.sec_ptr->Play(0, 0, dwPlayFlag);
		//	�{�����[����G�t�F�N�g�̏�����
		this->SetVolume(this->sinfo.iVolume);
		this->sinfo.bLoop = bLoop;
		this->sinfo.effectflag = Effect::NOTHING;
		return hRet;
	}

	//--------------------------------------------------------------------------------------------
	Err::Type Sound::LoadWaveStreamFirst(const char* szFileName,
		BYTE** data_ptr, DWORD dwSize, WAVEFORMATEX* wf_ptr)
	{
		HANDLE hFile = NULL;		//	Wave�t�@�C���n���h��
		Err::Type hRet = Err::NOTHING;	//	�߂�l���i�[���܂�
		DWORD dwResult;			//	ReadFile�̌��ʂ��i�[����
		DWORD dwRiffSize;		//	RIFF�`�����N�̑傫�����擾
		DWORD dwFmtSize;		//	fmt�`�����N�̑傫�����擾
		DWORD dwDataSize;		//	data�`�����N�̑傫�����擾
		char szChunk[4] = "";	//	�`�����N�̕�������i�[���܂��i\0�͕ۑ����܂���j
		bool bLoop;

		//	�t�@�C�����J��
		hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, NULL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			hRet = Err::INVOPEN;
			goto end;
		}
		//	RIFF�`�����N�̓ǂݍ���
		ReadFile(hFile, szChunk, 4, &dwResult, NULL);
		//	�ǂݍ��ݎ��s�A��������RIFF�ł͂Ȃ������ꍇ
		if ((dwResult != 4) || (memcmp("RIFF", szChunk, 4)))
		{
			hRet = Err::RIFF;
			goto end;
		}
		ReadFile(hFile, &dwRiffSize, sizeof(DWORD), &dwResult, NULL);
		ReadFile(hFile, szChunk, 4, &dwResult, NULL);
		//	�ǂݍ��ݎ��s�A��������WAVE�ł͂Ȃ������ꍇ
		if ((dwResult != 4) || (memcmp("WAVE", szChunk, 4)))
		{
			hRet = Err::RIFF;
			goto end;
		}
		//	Foramt�`�����N
		bLoop = true;
		while (bLoop)
		{
			ReadFile(hFile, szChunk, 4, &dwResult, NULL);
			//	�ǂݍ��ݎ��s�A�������͕s���ȃf�[�^�`���������ꍇ
			if (dwResult != 4)
			{
				hRet = Err::DATA;
				goto end;
			}
			//	�ړI�̃`�����N�������ꍇ
			if (memcmp("fmt", szChunk, 3) == 0)
			{
				bLoop = false;
			}
			else
			{
				//	�`�����N�Ⴂ
				ReadFile(hFile, &dwDataSize, 4, &dwResult, NULL);
				SetFilePointer(hFile, dwDataSize, 0, FILE_CURRENT);
			}
		}
		//	fmt�f�[�^�T�C�Y
		ReadFile(hFile, &dwFmtSize, sizeof(DWORD), &dwResult, NULL);
		//	�t�H�[�}�b�g���擾
		ReadFile(hFile, wf_ptr, dwFmtSize, &dwResult, NULL);
		if ((wf_ptr->wFormatTag != WAVE_FORMAT_PCM))
		{
			hRet = Err::NOTPCM;
			goto end;
		}
		wf_ptr->cbSize = 0;
		//	Data�`�����N����
		bLoop = true;
		while (bLoop)
		{
			ReadFile(hFile, szChunk, 4, &dwResult, NULL);
			//	�ǂݍ��ݎ��s�A�������͕s���ȃf�[�^�`���������ꍇ
			if (dwResult != 4)
			{
				hRet = Err::DATA;
				goto end;
			}
			//	Data�`�����N���݂�����
			if (memcmp("data", szChunk, 4) == 0)
			{
				bLoop = false;
			}
			else
			{
				//	�`�����N�Ⴂ
				ReadFile(hFile, &dwDataSize, 4, &dwResult, NULL);
				SetFilePointer(hFile, dwDataSize, 0, FILE_CURRENT);
			}
		}
		//	�f�[�^�T�C�Y�ǂݍ���
		ReadFile(hFile, &dwDataSize, 4, &dwResult, NULL);
		//	WAV�w�b�_�T�C�Y��ۑ����Ă����i�\�����Ȃ��`�����N�f�[�^�΍�j
		this->sinfo.dwHeaderSize = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
		//	WAVE�f�[�^��ǂݍ���
		if (dwSize == 0)
		{
			dwSize = dwDataSize;
		}
		(*data_ptr) = new BYTE[dwSize];
		//	SetFilePointer(hFile, Sound::sinfo[iNumber].dwHeaderSize+dwOffset, NULL, FILE_BEGIN);
		ReadFile(hFile, *data_ptr, dwSize, &dwResult, NULL);
		//	�w�肵���T�C�Y�Ɠǂݍ��񂾃T�C�Y���Ⴄ�Ƃ�
		if (dwResult != dwSize)
		{
			hRet = Err::READERROR;
			goto end;
		}
		//	�t�@�C���̃J�����g�|�W�V�������擾���Ă���
		this->sinfo.dwReadPosition = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	end:
		verCloseHandle(hFile);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	void Sound::SetLoopInfo(int iReStartPos, int iLoopCnt)
	{
		this->sinfo.dwReStart = iReStartPos;
		this->sinfo.dwCnt = iLoopCnt;
	}
	//--------------------------------------------------------------------------------------------
	void Sound::FadeOut(int iValue)
	{
		this->sinfo.effectflag = Effect::FADEOUT;
		this->sinfo.iFade = iValue;
	}
	//--------------------------------------------------------------------------------------------
	int Sound::GetVolume( )
	{
		long iVolume = 0;
		if (this->sinfo.sec_ptr == NULL)
		{
			return iVolume;
		}
		this->sinfo.sec_ptr->GetVolume(&iVolume);
		return iVolume;
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::SetPan(int iPan)
	{
		if (this->sinfo.sec_ptr == NULL)
		{
			return DSERR_INVALIDCALL;
		}
		return this->sinfo.sec_ptr->SetPan(iPan);
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::SetFrequency(DWORD dwFreq)
	{
		if (this->sinfo.sec_ptr == NULL)
		{
			return DSERR_INVALIDCALL;
		}
		return this->sinfo.sec_ptr->SetFrequency(dwFreq);
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::SetVolume(long int iVolume)
	{
		if (this->sinfo.sec_ptr == NULL)
		{
			return DSERR_INVALIDCALL;
		}
		this->sinfo.iVolume = iVolume;
		return this->sinfo.sec_ptr->SetVolume(iVolume);
	}
	//--------------------------------------------------------------------------------------------
	void Sound::WriteMute(int iSize)
	{
		if (this->sinfo.sec_ptr == NULL)
		{
			return;
		}

		LPVOID lpSoundData1 = NULL, lpSoundData2 = NULL;
		DWORD size1, size2;

		this->sinfo.sec_ptr->Lock(0, iSize, &lpSoundData1, &size1,
			&lpSoundData2, &size2, DSBLOCK_ENTIREBUFFER);
		// �T�E���h�f�[�^�ǂݍ���
		memset(lpSoundData1, 0, iSize);
		// ���b�N����
		this->sinfo.sec_ptr->Unlock(lpSoundData1, size1, lpSoundData2, size2);
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::Stop( )
	{
		if (this->sinfo.sec_ptr == NULL)
		{
			return DSERR_INVALIDCALL;
		}

		auto  ds = Obj::GetInst();
		if (!this->sinfo.sec_ptr)
		{
			return HRESULT(0);
		}
		//	Sound::sinfo[iNumber].bStream = false;
		if (this->sinfo.bStream) {
			WriteMute(ds->dwBufferSize);
			this->sinfo.dwReadPosition = this->sinfo.dwHeaderSize;			//	�ǂ݂���ł���t�@�C���|�W�V����
			this->sinfo.iStreamPos = 0;
		}
		return this->sinfo.sec_ptr->Stop();
	}
	//--------------------------------------------------------------------------------------------
	void Sound::WriteSoundData(BYTE* data_ptr, int iOffset, int iSize)
	{
		LPVOID lpSoundData1 = NULL, lpSoundData2 = NULL;
		DWORD size1, size2;

		this->sinfo.sec_ptr->Lock(iOffset, iSize, &lpSoundData1, &size1,
			&lpSoundData2, &size2, DSBLOCK_ENTIREBUFFER);
		// �T�E���h�f�[�^�ǂݍ���
		memcpy(lpSoundData1, data_ptr, iSize);
		// ���b�N����
		this->sinfo.sec_ptr->Unlock(lpSoundData1, size1, lpSoundData2, size2);
	}
	//--------------------------------------------------------------------------------------------
	Err::Type Sound::WriteWaveStream(const char* szFileName,
		BYTE* data_ptr, int iOffset, DWORD dwSize, int iWriteOffset)
	{
		HANDLE hFile = NULL;
		DWORD dwResult;
		DWORD dwRead;	//	���ۂɃt�@�C����ǂݍ��񂾃o�C�g��
		Err::Type hRet = Err::NOTHING;

		//	�t�@�C�����J��
		hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, NULL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			hRet = Err::INVOPEN;
			goto end;
		}
		//	�f�[�^��ǂݍ���
		SetFilePointer(hFile, iOffset, NULL, FILE_BEGIN);
		ReadFile(hFile, data_ptr, dwSize, &dwResult, NULL);
		//	�w�肵���T�C�Y�Ɠǂݍ��񂾃T�C�Y���Ⴄ�Ƃ�
		if (dwResult != dwSize)
		{
			//	EOF�������Ɖ��肵�A�������s��
			//	�ǂݍ��ݐ�̃|�C���g���f�[�^�̐擪�ɐݒ肷��
			dwRead = SetFilePointer(hFile, 0, NULL, FILE_CURRENT) - this->sinfo.dwReadPosition;
			if (!this->sinfo.bLoop)
			{
				ZeroMemory(&data_ptr[dwRead], dwSize - dwRead);
				this->sinfo.effectflag = Effect::STOP;
				goto end;
			}
			//	�ǂݍ��݃|�W�V������ݒ肷��
			if (this->sinfo.dwReStart != 0)	//	���X�^�[�g�n�_���ݒ肳��Ă���Ȃ��
			{
				this->sinfo.dwReadPosition = this->sinfo.dwReStart + this->sinfo.dwHeaderSize;
			}
			else
			{
				this->sinfo.dwReadPosition = this->sinfo.dwHeaderSize;
			}
			//	�o�b�t�@�Ƀf�[�^����������
			verCloseHandle(hFile);
			this->WriteSoundData(data_ptr, iWriteOffset, dwRead);
			this->WriteWaveStream(szFileName, &data_ptr[dwRead],
				this->sinfo.dwReadPosition, dwSize - dwRead, iWriteOffset + dwRead);
		}
		else
		{
			//	�t�@�C���̃J�����g�|�W�V�������擾���Ă���
			this->sinfo.dwReadPosition = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
			this->WriteSoundData(data_ptr, iWriteOffset, dwSize);
		}
	end:
		verCloseHandle(hFile);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	void Sound::LoadStream(int iWriteOffset, int iStreamPos)
	{
		BYTE* data_ptr = NULL;	//	�T�E���h�f�[�^

		auto  dm = Obj::GetInst();
		data_ptr = new BYTE[dm->dwBufferSize];
		WriteWaveStream(this->sinfo.szFileName, data_ptr,
			this->sinfo.dwReadPosition, dm->dwBufferSize, iWriteOffset);
		this->sinfo.iStreamPos = iStreamPos;
		verDelete_Vector(data_ptr);
	}
	//--------------------------------------------------------------------------------------------
	void Sound::Check_Stream(DWORD  dwBufferSize_)
	{
		DWORD dwStatus;	//	�Đ��t���O���i�[
		DWORD dwPos;	//	�Đ��|�W�V�������i�[
		//	���������I����Ă��Ȃ��ꍇ
		if (!this->sinfo.sec_ptr)
		{
			return;
		}
		//	�X�g���[�~���O�Đ���������Ă��Ȃ�
		if (!this->sinfo.bStream)
		{
			return;
		}
		//	�o�b�t�@�͍Đ������ǂ����𒲂ׂ�
		this->sinfo.sec_ptr->GetStatus(&dwStatus);
		if (!(dwStatus & DSBSTATUS_PLAYING))
		{
			return;
		}
		//	�ǂݍ��݃^�C�~���O���ǂ������`�F�b�N
		this->sinfo.sec_ptr->GetCurrentPosition(&dwPos, NULL);
		//	�Đ��|�W�V�����t���O��0�ŁA����0-11025�Ȃ�22050-44100�̃f�[�^���X�V����B
		if ((this->sinfo.iStreamPos == 0) && (dwPos > 0) && (dwPos < (dwBufferSize_ / 2)))
		{
			this->LoadStream(dwBufferSize_, 1);
		}
		//	�Đ��|�W�V�����t���O��1�ŁA����22050-33075�Ȃ�0-22050�̃f�[�^���X�V�B
		if ((this->sinfo.iStreamPos == 1) && (dwPos > dwBufferSize_))
		{
			this->LoadStream(0, 0);
		}
		//	�G�t�F�N�g���Ǘ�
		if (this->sinfo.effectflag == Effect::FADEOUT)
		{
			this->sinfo.iVolume -= this->sinfo.iFade;//kVALUE_FADE;
			if (this->sinfo.iVolume < kVOLUME_MIN)
			{
				this->Stop();
				this->sinfo.effectflag = Effect::NOTHING;
			}
			this->SetVolume(this->sinfo.iVolume);
		}
		if (this->sinfo.effectflag == Effect::FADEIN)
		{
			this->sinfo.iVolume += this->sinfo.iFade;//kVALUE_FADE;
			if (this->sinfo.iVolume > DSBVOLUME_MAX)
			{
				this->sinfo.iVolume = DSBVOLUME_MAX;
				this->sinfo.effectflag = Effect::NOTHING;
			}
			this->SetVolume(this->sinfo.iVolume);
		}
		if (this->sinfo.effectflag == Effect::STOP)
		{
			this->Stop( );
			this->sinfo.effectflag = Effect::NOTHING;
		}
	}



	//--------------------------------------------------------------------------------------------
	//	�֐���		�F	GetErrorMessage
	//	�쐬��		�F	�A�R����
	//	�쐬��		�F	2002.06.24
	//	�@�\		�F	�G���[���b�Z�[�W���擾���܂�
	//	�@�\����	�F	�unumber�v�ɑΉ����镶������uszMsg�v�ɃR�s�[���܂�
	//					�uiLength�v�ɂ̓R�s�[��̃o�b�t�@�T�C�Y���w�肵�Ă�������
	//	����		�F	�����́�number		�Q�Ƃ���G���[�i���o�[
	//					�����́�szMsg		���b�Z�[�W�̊i�[��
	//					�����́�iLength		�i�[��̒���
	//	�g�p�ϐ�	�F	char szWork		������̈ʒu���i�[��
	//	�g�p�֐�	�F	strcpy_s	������R�s�[
	//					memcpy			�������R�s�[
	//	�߂�l		�F	����܂���
	//	�X�V����	�F	2002.06.24	Ver1.00		�A�R����	�@�\�̎���
	//					2007.02.09	Ver1.01		�A�R����	�R�s�[���@�̕ύX
	//--------------------------------------------------------------------------------------------

	std::string GetErrorMessage(Err::Type number)
	{
		std::string szMsg;

		switch (number)
		{
		case Err::CCIM: szMsg = "DirectMusic�̍쐬�Ɏ��s"; break;
		case Err::CCIS: szMsg = "DirectSound�̍쐬�Ɏ��s"; break;
		case Err::INIT: szMsg = "Initialize�Ɏ��s"; break;
		case Err::SCL: szMsg = "SetCooperativeLevel�Ɏ��s"; break;
		case Err::CSB: szMsg = "�o�b�t�@�쐬�Ɏ��s"; break;
		case Err::QI: szMsg = "QueryInterface�Ɏ��s"; break;
		case Err::SFMT: szMsg = "�t�H�[�}�b�g�ݒ�Ɏ��s"; break;
		case Err::IA: szMsg = "InitAudio�Ɏ��s"; break;
		case Err::CCILOADER: szMsg = "���[�_�[�쐬�Ɏ��s"; break;
		case Err::MBTWC: szMsg = "unicode�ϊ��Ɏ��s"; break;
		case Err::SSD: szMsg = "�f�B���N�g���ݒ�Ɏ��s"; break;
		case Err::PSE: szMsg = "�Z�O�����g�f�[�^���Đ��ł��Ȃ�"; break;
		case Err::INVALID: szMsg = "�t�@�C�����ǂݍ��߂Ȃ�"; break;
		case Err::DL: szMsg = "�_�E�����[�h�ł��Ȃ�"; break;
		case Err::UNLD: szMsg = "�A�����[�h�ł��Ȃ�"; break;
		case Err::INVOPEN: szMsg = "�t�@�C�����J���Ȃ�"; break;
		case Err::RIFF: szMsg = "RIFF�`�����N���ǂݍ��߂Ȃ�"; break;
		case Err::FMT: szMsg = "fmt�`�����N���ǂݍ��߂Ȃ�"; break;
		case Err::NOTPCM: szMsg = "�ǂݍ��ݑΏۂ�WAVE�̓T�|�[�g����Ă��Ȃ��`��"; break;
		case Err::DATA: szMsg = "data�`�����N�͑��݂��Ȃ�"; break;
		case Err::READERROR: szMsg = "�ǂݍ��ݎ��s"; break;
		case Err::NOTHING: szMsg = "�G���[�͂���܂���"; break;
		default: szMsg = "��`����Ă��Ȃ��G���["; break;
		}
		return szMsg;
	}
	//-----------------------------------------------------------------------
	//Mouse�n
	//-----------------------------------------------------------------------
	map<string, DM::Sound::SP>	SoundManager::arr;
	bool SoundManager::notObject;
	void  SoundManager::Set(const string name_, DM::Sound::SP sp_)
	{
		arr[name_] = sp_;
	}
	//-----------------------------------------------------------------------
	DM::Sound::SP  SoundManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("SoundManager �Ώۂ����݂��܂���"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  SoundManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void SoundManager::AllClear()
	{
		arr.clear();
	}
	//---------------------------------------------------------------------------
}


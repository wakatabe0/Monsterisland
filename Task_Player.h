#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include "BChara.h"

namespace  Player
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("�v���C��");	//�O���[�v��
	const  string  defName("��");	//�^�X�N��
	//-------------------------------------------------------------------
	class  Resource
	{
		bool  Initialize();
		bool  Finalize();
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//���L����ϐ��͂����ɒǉ�����
		DG::Image::SP  img;
	};
	//-------------------------------------------------------------------
	class  Object : public  BChara
	{
	//�ύX�s����������������������������������������������������
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//�������� ������true�Ń^�X�N�V�X�e���֎����o�^
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//�u�������v�^�X�N�������ɂP�񂾂��s������
		void  UpDate();		//�u���s�v�P�t���[�����ɍs������
		void  Render2D_AF();	//�u2D�`��v�P�t���[�����ɍs������
		bool  Finalize();		//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	//�ύX������������������������������������������������������
	public:
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����
		//BChara�Ɏ������Ă��Ȃ��ϐ��E���\�b�h�݂̂����ɏ���
		DI::GamePad::SP  controller;

		//�v�l���󋵔��f(�X�e�[�^�X����j
		void  Think();
		//���[�V�����ɑΉ���������
		void  Move();
		//�A�j���[�V��������
		BChara::DrawInfo  Anim();
		//�ڐG���̉��������i�K���󂯐g�̏����Ƃ��Ď�������j
		void Received(BChara* from_, AttackInfo at_);
	};
}
#pragma warning(disable:4996)
#pragma once
//-----------------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-----------------------------------------------------------------------------
#include "GameEngine_Ver3_81.h"

class BChara : public BTask
{
	//�ύX�s����������������������������������������������������
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;
public:
	//�ύX������������������������������������������������������
	//�L�����N�^���ʃ����o�ϐ�
	ML::Vec2    pos;		//�L�����N�^�ʒu
	ML::Box2D   hitBase;	//�����蔻��͈�
	ML::Vec2	moveVec;	//�ړ��x�N�g��
	int			moveCnt;	//�s���J�E���^
	//���E�̌����i2D�����_�Q�[����p�j
	enum Angle_LRFB { Left, Right,Front,Back };
	Angle_LRFB	angle_LRFB;
	WP			target;
	//�L�����N�^�̍s����ԃt���O
	enum Motion
	{
		Unnon = -1,	//	����(�g���܂���j
		Stand,		//	��~
		Walk,		//	���s
		//Attack,		//	�U��
		//Turn,		//���ʐڐG
		//Bound,		//�_���[�W���󂯂Đ������ł���
		//Lose,		//������E���V
	};
	Motion			motion;			//	���݂̍s���������t���O
	int				animCnt;		//�A�j���[�V�����J�E���^
	float			maxSpeed;		//	�㉺���E�����ւ̈ړ��̉��Z��
	float			decSpeed;		//	�㉺���E�����ւ̈ړ��̌�����
	int hp;							//�L�����N�^�̃q�b�g�|�C���g
	int unHitTime;					//��莞�Ԗ��G

	//bool walkFlag;//�����Ă��邩�����~�܂��Ă��邩�̔���

	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����鎖����
	BChara()
		: pos(0, 0)
		, hitBase(0, 0, 0, 0)
		, moveVec(0, 0)
		, moveCnt(0)
		,angle_LRFB(Front)
		, motion(Stand)
		, maxSpeed(0)
		, decSpeed(0)
		,hp(1)
		,unHitTime(0)
	{
	}
	virtual  ~BChara() {}

	//�L�����N�^���ʃ��\�b�h
	//�߂荞�܂Ȃ��ړ�����
	virtual  void  CheckMove(ML::Vec2&  est_);
	//�����ڐG����
	virtual  bool  CheckFoot();
	//����ڐG����
	virtual  bool  CheckHead();
	//���ʐڐG����i�ēx�r���[�Q�[����p�j
	virtual bool CheckFront_LR();
	//���[�V�������X�V�i�ύX�Ȃ��̏ꍇ	false)
	bool  UpdateMotion(Motion  nm_);

	//	�A�j���[�V�������\����
	struct DrawInfo {
		ML::Box2D		draw, src;
		ML::Color		color;
	};
	//�U�����
	struct AttackInfo {
		int power;		//�U���̈З�
		int hit;		//�������x
		int element;	//�U���̑���
	};
	//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�Ȃ��́j
	virtual void Received(BChara* from_, AttackInfo at_);
	//�ڐG����
	virtual bool CheckHit(const ML::Box2D& hit);
};

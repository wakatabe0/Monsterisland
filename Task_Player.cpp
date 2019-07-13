//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map2D.h"
//#include "Task_Shot01.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/chara3.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->img.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();

		//���f�[�^������
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = ML::Box2D(-16, -16, 32, 32);
		this->angle_LRFB = Front;
		this->controller = ge->in1;
		this->motion = Stand;		//�L�����������
		this->maxSpeed = 3.5f;		//�ő�ړ����x�i���j
		this->decSpeed = 0.5f;		//���x�̌�����
		this->hp = 5;//�v���C���̃q�b�g�|�C���g
		//this->walkFlag = false;
		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		this->moveCnt++;
		this->animCnt++;
		if (this->unHitTime > 0) { this->unHitTime--; }
		//�v�l�E�󋵔��f
		this->Think();
		//�����[�V�����ɑΉ���������
		this->Move();
		//�߂荞�܂Ȃ��ړ�
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);

		//�����蔻��
		//{
		//	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
		//	auto targets = ge->GetTask_Group_G<BChara>("�A�C�e��");
		//	for (auto it = targets->begin();
		//		it != targets->end();
		//		++it) {
		//		//����ɐڐG�̗L�����m�F������
		//		if ((*it)->CheckHit(me)) {
		//			//����Ƀ_���[�W�̏������s�킹��
		//			BChara::AttackInfo at = { 0,0,0 };
		//			(*it)->Received(this, at);
		//			break;
		//		}
		//	}
		//}
		//�J�����̈ʒu���Ē���
		{
			//�v���C������ʂ̉����ɒu�����i����͉�ʒ����j
			int  px = ge->camera2D.w / 2;
			int  py = ge->camera2D.h / 2;
			//�v���C������ʒ����ɒu�������̃J�����̍�����W�����߂�
			int  cpx = int(this->pos.x) - px;
			int  cpy = int(this->pos.y) - py;
			//�J�����̍��W���X�V
			ge->camera2D.x = cpx;
			ge->camera2D.y = cpy;
			if (auto  map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v")) {
				map->adjustCameraPos();
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (this->unHitTime > 0) {
			if ((this->unHitTime / 4) % 2 == 0) {
				return;//8�t���[����4�t���[���摜��\�����Ȃ�
			}
		}

		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//�X�N���[���Ή�
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(di.draw, di.src);
	}
	//-----------------------------------------------------------------------------
	//�v�l���󋵔��f�@���[�V��������
	void  Object::Think()
	{
		auto  inp = this->controller->GetState();
		BChara::Motion  nm = this->motion;	//�Ƃ肠�������̏�Ԃ��w��

		//�v�l�i���́j��󋵂ɉ����ă��[�V������ύX���鎖��ړI�Ƃ��Ă���B
		//���[�V�����̕ύX�ȊO�̏����͍s��Ȃ�
		switch (nm) {
		case  Stand:	//�����Ă���
			if (inp.LStick.L.on) { nm = Walk; }
			if (inp.LStick.R.on) { nm = Walk; }
			if (inp.LStick.U.on) { nm = Walk; }
			if (inp.LStick.D.on) { nm = Walk; }
			break;
		case  Walk:		//�����Ă���
			if (inp.LStick.L.off&&inp.LStick.R.off
				&&inp.LStick.U.off&&inp.LStick.D.off) { nm = Stand; }
			break;
		//case  Attack:	//�U����
		//	if (moveCnt == 8) { nm = Stand; }
		//	break;
		//case Bound: //�_���[�W���󂯂Đ������ł���
		//	if (this->moveCnt >= 12 && this->CheckFoot() == true) {nm = Stand;}
		//	break;
		}
		//���[�V�����X�V
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//���[�V�����ɑΉ���������
	//(���[�V�����͕ύX���Ȃ��j
	void  Object::Move()
	{
		auto  inp = this->controller->GetState();
		//�ړ����x����
		switch(this->motion) {
		default:
			if (this->moveVec.x < 0) {
				this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
			}
			else {
				this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
			}
			if (this->moveVec.y < 0) {
				this->moveVec.y = min(this->moveVec.y + this->decSpeed, 0);
			}
			else {
				this->moveVec.y = max(this->moveVec.y - this->decSpeed, 0);
			}
		
		//�ړ����x�����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۖ����j
		//case Bound:
		case Unnon:	break;
		}
		//-----------------------------------------------------------------
		//���[�V�������ɌŗL�̏���
		switch (this->motion) {
		case  Stand:	//�����Ă���
			//this->walkFlag = false;//��{��false,�����Ă��鎞�����ύX����
			break;
		case  Walk:		//�����Ă���
			if (inp.LStick.L.on) {
				this->angle_LRFB = Left;
				//this->walkFlag = true;
				this->moveVec.x=-this->maxSpeed;
			}
			if (inp.LStick.R.on) {
				this->angle_LRFB = Right;
				//this->walkFlag = true;
				this->moveVec.x = this->maxSpeed;
			}
			if (inp.LStick.U.on) {
				this->angle_LRFB = Front;
				//this->walkFlag = true;
				this->moveVec.y =- this->maxSpeed;
			}
			if (inp.LStick.D.on){
				this->angle_LRFB = Back;
				//this->walkFlag = true;
				this->moveVec.y = this->maxSpeed;
			}
			break;
		//case  Attack:	//�U����
			//��������`���Ȃ��猂��
			//if (moveCnt == 4) {
			//	//�e�𐶐�
			//	auto shot = Shot01::Object::Create(true);
			//	shot->pos = this->pos;
			//	if (this->angle_LR == Right) {
			//		shot->moveVec = ML::Vec2(+5, -6);
			//		shot->pos += ML::Vec2(20, 0);
			//	}
			//	else {
			//		shot->moveVec = ML::Vec2(-5, -6);
			//		shot->pos += ML::Vec2(-20, 0);
			//	}
			//}
			//break;
		}
	}
	//-----------------------------------------------------------------------------
	//�ڐG���̉��������i�K���󂯐g�̏����Ƃ��Ď�������j
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		//if (this->unHitTime > 0) {
		//	return;//���G���Ԓ��̓_���[�W���󂯂Ȃ�
		//}
		//this->unHitTime = 90;
		//this->hp -= at_.power;	//������
		//if (this->hp <= 0) {
		//	this->Kill();
		//}
		//������΂����
		/*if (this->pos.x > from_->pos.x) {
			this->moveVec = ML::Vec2(+4, -9);
		}
		else {
			this->moveVec = ML::Vec2(-4, -9);
		}*/
		//this->UpdateMotion(Bound);
		//from_�͍U�����Ă�������A�J�E���^�[�Ȃǂŋt�Ƀ_���[�W��^�������Ƃ��Ɏg��
	}
	//-----------------------------------------------------------------------------
	//�A�j���[�V��������
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(0, 0, 64, 64), defColor },	//�O�������s�i�E���j//0
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(64, 0, 64, 64), defColor },	//�O������~//1
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(128, 0, 64, 64), defColor },//�O�������s�i����)//2

			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(0, 64, 64, 64), defColor },//���������s�i�E���j//3
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(64, 64, 64, 64), defColor },//��������~ //4
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(128, 64, 64, 64), defColor },//���������s�i�����j//5

			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(0, 128, 64, 64), defColor },//�E�������s�i�E���j//6
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(64, 128, 64, 64), defColor },//�E������~ //7
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(128, 128, 64, 64), defColor },//�E�������s�i�����j//8

			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(0, 192, 64, 64), defColor },//���������s�i�����j//9
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(64, 192, 64, 64), defColor },//��������~ //10
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(128, 192, 64, 64), defColor },//���������s�i�E���j//11
		};
		BChara::DrawInfo  rtv;
		//int  work;
		int anim[4] = { 1,0,1,2 };//�A�j���[�V�����p�^�[��
		switch (this->motion) {
		//default:		rtv = imageTable[1];	break;
		//	��~----------------------------------------------------------------------------
		case  Stand:	
			//if (walkFlag==false) {
				if (this->angle_LRFB == Back) { rtv = imageTable[1]; }
				if (this->angle_LRFB == Left) { rtv = imageTable[4]; }
				if (this->angle_LRFB == Right) { rtv = imageTable[7]; }
				if (this->angle_LRFB == Front) { rtv = imageTable[10]; }
			//}
			break;
		//	���s----------------------------------------------------------------------------
		case  Walk:
				//if (walkFlag == true) {
			    if (this->angle_LRFB == Back) {
					rtv = imageTable[anim[animCnt / 8 % 4]];
				}
				if (this->angle_LRFB == Left) {
					rtv = imageTable[anim[animCnt / 8 % 4] + 3];
				}
				if (this->angle_LRFB == Right) {
					rtv = imageTable[anim[animCnt / 8 % 4] + 6];
				}
				if (this->angle_LRFB == Front) {
					rtv = imageTable[anim[animCnt / 8 % 4] + 9];
				}
			
			//}
			/*work = this->animCnt / 8;
			work %= 12;
			rtv = imageTable[work + 1];*/
			break;
		//	�_���[�W---------------------------------------------------------------------
	/*	case
			Bound:	rtv = imageTable[8];	break;*/
		}
		//�����ɉ����ĉ摜�����E���]����
	/*	if (Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}*/
		
		return rtv;
	}
	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//���\�[�X�N���X�̐���
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}
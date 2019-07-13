
//-----------------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-----------------------------------------------------------------------------
#include "BChara.h"
#include  "MyPG.h"
#include  "Task_Map2D.h"

//-----------------------------------------------------------------------------
//���[�V�������X�V�i�ύX�Ȃ��̏ꍇ	false)
bool  BChara::UpdateMotion(Motion  nm_)
{
	if (nm_ == this->motion) {
		return  false;
	}
	else {
		this->motion = nm_;		//���[�V�����ύX
		this->moveCnt = 0;		//�s���J�E���^�N���A
		this->animCnt = 0;		//�A�j���[�V�����J�E���^�̃N���A
		return  true;
	}
}

//-----------------------------------------------------------------------------
//����ڐG����
bool  BChara::CheckHead()
{
	//�����蔻�����ɂ��ē����`�𐶐�
	ML::Box2D  head(this->hitBase.x,
		this->hitBase.y - 1,
		this->hitBase.w,
		1);
	head.Offset((int)this->pos.x, (int)this->pos.y);


	auto   map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (nullptr == map) { return false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
	return map->CheckHit(head);
}
//-----------------------------------------------------------------------------
//�߂荞�܂Ȃ��ړ�����
void BChara::CheckMove(ML::Vec2&  e_)
{
	//�}�b�v�����݂��邩���ׂĂ���A�N�Z�X
	auto   map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (nullptr == map) { return; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j

								   //�����ɑ΂���ړ�
	while (e_.x != 0) {
		float  preX = this->pos.x;
		if (e_.x >= 1) { this->pos.x += 1;		e_.x -= 1; }
		else if (e_.x <= -1) { this->pos.x -= 1;		e_.x += 1; }
		else { this->pos.x += e_.x;		e_.x = 0; }
		ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
		if (true == map->CheckHit(hit)) {
			this->pos.x = preX;		//�ړ����L�����Z��
			break;
		}
	}
	//�c���ɑ΂���ړ�
	while (e_.y != 0) {
		float  preY = this->pos.y;
		if (e_.y >= 1) { this->pos.y += 1;		e_.y -= 1; }
		else if (e_.y <= -1) { this->pos.y -= 1;		e_.y += 1; }
		else { this->pos.y += e_.y;		e_.y = 0; }
		ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
		if (true == map->CheckHit(hit)) {
			this->pos.y = preY;		//�ړ����L�����Z��
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//�����ڐG����
bool  BChara::CheckFoot()
{
	//�����蔻�����ɂ��đ�����`�𐶐�
	ML::Box2D  foot(this->hitBase.x,
		this->hitBase.y + this->hitBase.h,
		this->hitBase.w,
		1);
	foot.Offset(this->pos);

	auto   map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (nullptr == map) { return false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
	return map->CheckHit(foot);
}
//-----------------------------------------------------------------------------
//�����ڐG����
bool  BChara::CheckFront_LR()
{
	//�����蔻�����ɂ��ċ�`�𐶐�
	if (this->angle_LRFB == Left) {
		ML::Box2D  front_LR(
			this->hitBase.x - 1,
			this->hitBase.y,
			1,
			this->hitBase.h
		);
		front_LR.Offset(this->pos);

		auto   map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v");
		if (nullptr == map) { return false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
		return map->CheckHit(front_LR);
	}
	else {
		ML::Box2D front_LR(
			this->hitBase.x + this->hitBase.w,
			this->hitBase.y,
			1,
			this->hitBase.h
			);
		front_LR.Offset(this->pos);

		auto   map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v");
		if (nullptr == map) { return false; }//�}�b�v��������Δ��肵�Ȃ�(�o���Ȃ��j
		return map->CheckHit(front_LR);
	}

}
//-----------------------------------------------------------------------------
//�ڐG���̉��������i���ꎩ�̂̓_�~�[�̂悤�Ȃ��́j
void BChara::Received(BChara* from_, AttackInfo at_)
{
	ML::MsgBox("Received ��������Ă��܂���");
}
//-----------------------------------------------------------------------------
//�ڐG����
bool  BChara::CheckHit(const ML::Box2D& hit_)
{
	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
	return me.Hit(hit_);
}
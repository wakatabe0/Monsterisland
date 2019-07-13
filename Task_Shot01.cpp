//-------------------------------------------------------------------
//�v���C���̏o���e
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Shot01.h"
#include "Task_Map2D.h"
//#include "Task_Effect00.h"
#include "Task_Enemy00.h"

namespace  Shot01
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/Shot00.png");
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
		this->render2D_Priority[1] = 0.4f;
		/*this->pos.x = 0;
		this->pos.y = 0;*/
		this->hitBase = ML::Box2D(-8, -8, 16, 16);
		this->moveVec = ML::Vec2(0, 0);
		this->moveCnt = 0;
		this->hp = 5;//�e�̃q�b�g�|�C���g
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
		//���E�̎��Ԃ��}���������
		if (this->moveCnt >= 100) {
			//���Ő\��
			this->Kill();
			return;
		}
		//�ړ�
		this->pos += this->moveVec;
		//�d�͉���
		this->moveVec.y += ML::Gravity(32) * 5;

		//�ړ���ŏ�Q���ɐڐG���������
		//�}�b�v�����݂��邩���ׂĂ���A�N�Z�X
		if (auto map = ge->GetTask_One_GN<Map2D::Object>("�t�B�[���h", "�}�b�v")) {
			ML::Box2D hit = this->hitBase.OffsetCopy(this->pos);
			if (true == map->CheckHit(hit)) {
				//���Ő\��
				this->Kill();

				//�Ƃ肠�������͂΂�T����
				/*for (int c = 0; c < 4; ++c) {
					auto eff = Effect00::Object::Create(true);
					eff->pos = this->pos;
				}*/
				return;
			}
		}
	
		
		//�e�ƓG�����蔻��
		{
			ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
			auto targets = ge->GetTask_Group_G<BChara>("�G");
			for (auto it = targets->begin();
				it != targets->end();
				++it) {
				//����ɐڐG�̗L�����m�F������
				if ((*it)->CheckHit(me)) {
					//����Ƀ_���[�W�̏������s�킹��
					BChara::AttackInfo at = { 5,0,0 };
					(*it)->Received(this, at);
					//���Ő\��
					this->Kill();
					break;
				}
			}
		}
		
	}
	
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(-8, -8, 16, 16);
		draw.Offset(this->pos);
		ML::Box2D src(0, 0, 32, 32);

		//�X�N���[���Ή�
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		this->res->img->Draw(draw, src);
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
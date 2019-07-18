//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EffectCreate.h"

#include "Task_EffectBomb.h"
//#include "Task_EffectHit.h"

namespace  EffectCreate
{
	Resource::WP  Resource::instance;

	void Object::CreateEffect(Object::EFFECTTYPE type, ML::Vec2 p, float scale_)
	{
		switch (type)
		{

	/*	case EFFECTTYPE::HIT:
		{
			auto  h = EffectHit::Object::Create(true);
			h->pos = p;
			h->sizeH = 30;
			h->sizeW = 30;
		}
		break;*/
		case EFFECTTYPE::L_MYST:

			break;
		case EFFECTTYPE::R_MYST:

			break;
		case EFFECTTYPE::N_MYST:

			break;
		case EFFECTTYPE::BOMB:
		{
			auto  b = EffectBomb::Object::Create(true);
			b->pos = p;
			b->sizeH = 128;
			b->sizeW = 128;



		}
		break;

		case EFFECTTYPE::BIGBOMB:
		{
			auto  b = EffectBomb::Object::Create(true);
			b->pos = p;
			b->sizeH = 256;
			b->sizeW = 256;



		}
		break;


		case EFFECTTYPE::LITTLEBOMB:
		{
			auto  b = EffectBomb::Object::Create(true);
			b->pos = p;
			b->sizeH = 64;
			b->sizeW = 64;



		}
		break;
		case EFFECTTYPE::LITTLEBOMBR:
		{
			auto  b = EffectBomb::Object::Create(true);
			b->pos = p;
			b->moveVec = ML::Vec2(3, 0);
			b->sizeH = 64;
			b->sizeW = 64;



		}
		break;





		case EFFECTTYPE::BOMBRING:

			break;

		case EFFECTTYPE::BOMBCHORD:


			break;

		}

	}

	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
	}

	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
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
	//リソースクラスの生成
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
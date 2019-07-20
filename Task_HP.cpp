//-------------------------------------------------------------------
//HP
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_HP.h"
#include  "Task_Player.h"

namespace  HP
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img1 = DG::Image::Create("./data/image/hpfream.png");
		this->img2 = DG::Image::Create("./data/image/hp.png");
		return true; 
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->img1.reset();
		this->img2.reset();
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
		this->render2D_Priority[1] = 0.3f;
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
		this->moveCnt++;
		this->animCnt++;
	
		//if (this->unHitTime > 0) { this->unHitTime--; }

		//Motion nm = this->motion;
		//switch (this->motion) {
		//case Stand:
		//	break;
		//case Lose:
		//	this->pos.y -= 3;
		//	if (this->moveCnt > 20) {
		//		this->Kill();//一定時間経過後、消滅
		//	}
		//	break;
		//}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//HP（空白）
		for (int i = 0; i < 5;++i) {
			ML::Box2D draw1(0 + i * 20, 0, 20, 20);
			ML::Box2D src1(0, 0, 40, 40);
			draw1.Offset(this->pos);
			this->res->img1->Draw(draw1, src1);
		}
		//HP
		for (int i = 0; i < ge->Insert_hp; ++i) {
			ML::Box2D draw2(0 + i * 20, 0, 20, 20);
			ML::Box2D src2(0, 0, 40, 40);
			draw2.Offset(this->pos);
			this->res->img2->Draw(draw2, src2);
		}
	}
	//-------------------------------------------------------------------
	//アニメーション制御
	/*BChara::DrawInfo Object::Anim()
	{
		BChara::DrawInfo imageTable[] = {
			{ ML::Box2D(-16,-16,32,32),ML::Box2D(0,0,32,32),ML::Color(1,1,1,1) },
		{ ML::Box2D(-16,-16,32,32),ML::Box2D(0,0,32,32),ML::Color(0.3f,1,1,1) },

		};
		BChara::DrawInfo  rtv;
		switch (this->motion) {
		case Stand:
			rtv = imageTable[0];
			break;
		case Lose:
			rtv = imageTable[1];
			break;
		}
		return rtv;
	}*/
	//-------------------------------------------------------------------
	//接触時の応答処理（必ず受け身の処理として実装する）
	/*void Object::Received(BChara* from_, AttackInfo at_)
	{
		from_->hp += 5;
	}*/
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
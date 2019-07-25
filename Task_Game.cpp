//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game.h"
#include  "Task_Title.h"
#include "Task_Gameover.h"
#include "Task_Gameclear.h"
#include  "Task_Map2D.h"
#include  "Task_Player.h"
#include "Task_Enemy00.h"
#include "Task_Arrow.h"
#include "Task_HP.h"
#include "Task_Item00.h"
#include "Task_Boat.h"
#include "Task_EventEngine.h"

namespace  Game
{
	Resource::WP  Resource::instance;
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
		ge->camera2D = ML::Box2D(-200, -100, 480, 270);//取りあえず初期値設定
		ge->GameoverFlag = false;//ゲームオーバーフラグ
		ge->GameclearFlag = false;//ゲームクリアフラグ
		ge->evFlags.clear();//本来はNewゲーム＆Loadゲームでクリアする

	   //★タスクの生成
	  
	   //マップの生成
		auto  m = Map2D::Object::Create(true);
		m->Load("./data/Map/map0.txt");
		//プレイヤの生成
		auto  pl = Player::Object::Create(true);
		pl->pos.x = 32 * 5;
		pl->pos.y = 32 * 18;

		//敵の生成
		for (int c = 0; c < 8; ++c) {
			auto ene = Enemy00::Object::Create(true);
			ene->pos.x = 100.0f + c * 80;
			ene->pos.y = 100;
		}
		//矢印（次の場所へ）の生成
		auto  ar = Arrow::Object::Create(true);
		ar->pos.x = 32 * 23;
		ar->pos.y = 16;
		ar->eventFileName = "./data/event/Map1Load.txt";
		
		//HPの生成
		auto hp = HP::Object::Create(true);

	
		//アイテム配置
			auto item = Item00::Object::Create(true);
			item->pos.x = 688;
			item->pos.y = 400;
	
		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->KillAll_G("フィールド");
		ge->KillAll_G("プレイヤ");
		ge->KillAll_G("敵");
		ge->KillAll_G("アイテム");
		ge->KillAll_G("矢印");
		ge->KillAll_G("HP");
		ge->KillAll_G("ゴール");
		ge->KillAll_G("船");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			
			//ゲームオーバーへ
			if (ge->GameoverFlag == true) {
				auto nextTask = Gameover::Object::Create(true);
			}
			//ゲームクリアへ
			else if (ge->GameclearFlag == true) {
				auto nextTask = Gameclear::Object::Create(true);
			}
			//タイトルへ
			else {
				auto nextTask = Title::Object::Create(true);
			}
		}		

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState( );
		if (inp.ST.down) {
			//自身に消滅要請
			this->Kill();
		}
		//ゲームオーバーフラグ（true）になったら消滅要請
		else if (ge->GameoverFlag == true) {
			this->Kill();
		}
		//ゲームクリアフラグ(ture)になったら消滅要請
		else if (ge->GameclearFlag == true) {
			this->Kill();
		}
		//else if (inp.SE.down) {//←の条件を変える
		//	//イベントエンジン起動
		//	if (auto ev = EventEngine::Object::Create_Mutex()) {
		//		ev->Set("./data/event/event0000.txt");
		//	}
		//}

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
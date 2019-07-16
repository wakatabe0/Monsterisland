﻿//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game.h"
#include  "Task_Title.h"
//#include "Task_Ending.h"
#include  "Task_Map2D.h"
#include  "Task_Player.h"
//#include  "Task_Sprite.h"
#include "Task_Enemy00.h"
//#include "Task_Item.h"
//#include "Task_Item01.h"
//#include "Task_Item02.h"

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

	   //★タスクの生成
	   //マップの生成
		auto  m = Map2D::Object::Create(true);
		m->Load("./data/Map/map0.txt");
		//プレイヤの生成
		auto  pl = Player::Object::Create(true);
		pl->pos.x = 32 * 5;
		pl->pos.y = 32 * 18;

		//妖精の生成
		/*auto  spr = Sprite::Object::Create(true);
		spr->pos = pl->pos;
		spr->target = pl;
*/
		//敵の生成
		for (int c = 0; c < 10; ++c) {
			auto ene = Enemy00::Object::Create(true);
			ene->pos.x = 100.0f + c * 80;
			ene->pos.y = 100;
		}
		//アイテム仮配置
		//Item00
		/*for (int c = 0; c < 3; ++c) {
			auto item = Item00::Object::Create(true);
			item->pos.x = 100.0f + c * 100;
			item->pos.y = 80;
		}*/
		//Item01
		/*for (int c = 0; c < 3; ++c) {
			auto item = Item01::Object::Create(true);
			item->pos.x = 500.0f + c * 100;
			item->pos.y = 80;
		}*/
		//Item02
	/*	for (int c = 0; c < 3; ++c) {
			auto item = Item02::Object::Create(true);
			item->pos.x = 112.0f + c * 100;
			item->pos.y = 700;
		}*/
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

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			//エンディングへ
			auto nextTask = Title::Object::Create(true);
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
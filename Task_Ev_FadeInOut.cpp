﻿//-------------------------------------------------------------------
//フェードイン＆アウト
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Ev_FadeInOut.h"

namespace  Ev_FadeInOut
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
		this->render2D_Priority[1] = 0.005f;
		this->cnt = 0;
		this->Stop();//自身を停止状態にしておく
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		this->img.reset();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		if (this->mode == In) {
			this->cnt--;
			if (this->cnt < 0) {
				//イベントエンジンを再開させる
				ge->StopAll_GN("イベント", "実行エンジン", false);
				//消滅する
				this->Kill();
			}
		}
		if(this->mode == Out) {
			this->cnt++;
			if (this->cnt > 60) {
				//イベントエンジンを再開させる
				ge->StopAll_GN("イベント", "実行エンジン", false);
				//停止状態にする
				this->Stop();
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(0, 0, ge->screen2DWidth, ge->screen2DHeight);
		this->img->Draw( draw, this->src,
			ML::Color(this->cnt / 0.3f, 1, 1, 1));//フェードイン・フェードアウト画面の表示時間
	}
	//-------------------------------------------------------------------
	//タスクを生成する（フェードイン）か、フェードアウトする
	void Object::CreateOrFadeInOut(stringstream& ss_)
	{
		//新規作成か更新の判別
		auto p = ge->GetTask_One_GN<Object>(defGroupName, defName);

		//新規作成の場合
		if (nullptr == p) {
			p = Object::Create(true);
			p->Set(ss_);
		}
		//更新の場合
		else {
			p->Set(ss_);
		}
	}
	//フェードアウト・フェードインの準備を行う
	void Object::Set(stringstream& ss_)
	{
		//パラメータを分解する
		string filePath;
		ss_ >> filePath;
		//フェードインする場合
		if (filePath == "in") {
			this->mode = In;
			this->cnt = 60;
		}
		//フェードアウトする場合
		else {
			this->mode = Out;
			this->cnt = 0;
			//画像を読み込む
			this->img = DG::Image::Create(filePath);
			POINT s = this->img->Size();
			this->src = ML::Box2D(0, 0, s.x, s.y);
		}
		//イベントエンジンを一時停止させる
		ge->StopAll_GN("イベント", "実行エンジン");
		//停止状態を解除する
		this->Stop(false);
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
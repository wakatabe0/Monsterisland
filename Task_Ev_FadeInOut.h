﻿#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//フェードイン＆アウト
//-------------------------------------------------------------------
#include "GameEngine_Ver3_81.h"

namespace  Ev_FadeInOut
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("フェードIO");	//グループ名
	const  string  defName("Def");	//タスク名
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
		//共有する変数はここに追加する
		/*「変数宣言を書く」*/
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
		//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate();		//「実行」１フレーム毎に行う処理
		void  Render2D_AF();	//「2D描画」１フレーム毎に行う処理
		bool  Finalize();		//「終了」タスク消滅時に１回だけ行う処理
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:
		//追加したい変数・メソッドはここに追加する
		/*「変数宣言を書く」
			「追加メソッドを書く」*/
		DG::Image::SP img;
		int cnt;
		ML::Box2D src;
		enum Mode{Out,In};
		Mode mode;

		//タスクを生成する（フェードイン）か、フェードアウトする
		static void CreateOrFadeInOut(stringstream& ss_);
		//フェードアウト・フェードインの準備を行う
		void Set(stringstream& ss_);
	};
}

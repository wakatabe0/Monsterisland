#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//イベント処理中の画像表示
//-------------------------------------------------------------------
#include "GameEngine_Ver3_81.h"

namespace  Ev_Image
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("イベント画像");	//グループ名
	const  string  defName("NoName");	//タスク名
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
		ML::Vec2 pos;
		ML::Box2D drawBase;
		ML::Box2D src;

		//タスクを生成するか、同名のタスクの情報を更新する
		static void CreateOrReset(stringstream& ss_);
		//表示するメッセージを設定する
		void Set(const string& taskName_, stringstream& ss_);
	};
}

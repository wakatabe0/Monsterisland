//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EffectBomb.h"

namespace  EffectBomb
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/effect/explode.png");
		//this->bomSe = DM::Sound::CreateSE("./data/sound/bom.wav");

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
		this->render2D_Priority[1] = 0.0f;
		this->pos = ML::Vec2(0, 0);
		this->moveVec = ML::Vec2(0.f, 0.f);
		this->alpha = 1.f;

		this->animCnt = 0;	//切り替えのカウント
		this->animMax = 12;	//アニメーション数
		this->animWait = 2;//切り替え時間
		this->animNo = 0;	//現在のアニメ番号
		this->animXCount = 12;

		this->scale = 1.f;

		//this->res->bomSe->Play_Normal(false);

		//se::LoadFile("bomb", "./data/sound/bom.wav");
		//se::Play("bomb");

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
		//座標の切り替え
		this->pos += this->moveVec;
		//アニメカウント加算
		this->animCnt++;
		if ((this->animCnt >= this->animWait)) {
			this->animCnt = 0;
			//アニメ変化
			this->animNo++;
			if (this->animNo >= this->animMax) {
				this->Kill();
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw1((int)(-this->sizeW / 2 * this->scale), 
			(int)(-this->sizeH / 2 * this->scale),
			(int)(this->sizeW * this->scale),
			(int)(this->sizeH* this->scale));
		draw1.Offset((int)this->pos.x, (int)this->pos.y);
		ML::Box2D  src1(this->animNo%this->animXCount* 96,this->animNo/this->animXCount* 96, 96, 96);
		
		//スクロール対応
		draw1.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(draw1, src1);
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
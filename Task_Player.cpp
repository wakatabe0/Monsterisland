//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map2D.h"
#include "Task_Shot01.h"
#include "Task_Gameover.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/chara3.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->img.reset();
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
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = ML::Box2D(-16, -16, 32, 32);
		this->angle_LRFB = Front;
		this->controller = ge->in1;
		this->motion = Stand;		//キャラ初期状態
		this->maxSpeed = 3.5f;		//最大移動速度（横）
		this->decSpeed = 0.5f;		//速度の減衰量
		this->hp = 5;//プレイヤのヒットポイント
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
		if (this->unHitTime > 0) { this->unHitTime--; }
		//思考・状況判断
		this->Think();
		//現モーションに対応した制御
		this->Move();
		//めり込まない移動
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);

		//アイテムの当たり判定
		//{
		//	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
		//	auto targets = ge->GetTask_Group_G<BChara>("アイテム");
		//	for (auto it = targets->begin();
		//		it != targets->end();
		//		++it) {
		//		//相手に接触の有無を確認させる
		//		if ((*it)->CheckHit(me)) {
		//			//相手にダメージの処理を行わせる
		//			BChara::AttackInfo at = { 0,0,0 };
		//			(*it)->Received(this, at);
		//			break;
		//		}
		//	}
		//}
		//カメラの位置を再調整
		{
			//プレイヤを画面の何処に置くか（今回は画面中央）
			int  px = ge->camera2D.w / 2;
			int  py = ge->camera2D.h / 2;
			//プレイヤを画面中央に置いた時のカメラの左上座標を求める
			int  cpx = int(this->pos.x) - px;
			int  cpy = int(this->pos.y) - py;
			//カメラの座標を更新
			ge->camera2D.x = cpx;
			ge->camera2D.y = cpy;
			if (auto  map = ge->GetTask_One_GN<Map2D::Object>("フィールド", "マップ")) {
				map->adjustCameraPos();
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (this->unHitTime > 0) {
			if ((this->unHitTime / 4) % 2 == 0) {
				return;//8フレーム中4フレーム画像を表示しない
			}
		}

		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//スクロール対応
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(di.draw, di.src);
	}
	//-----------------------------------------------------------------------------
	//思考＆状況判断　モーション決定
	void  Object::Think()
	{
		auto  inp = this->controller->GetState();
		BChara::Motion  nm = this->motion;	//とりあえず今の状態を指定

		//思考（入力）や状況に応じてモーションを変更する事を目的としている。
		//モーションの変更以外の処理は行わない
		switch (nm) {
		case  Stand:	//立っている
			if (inp.LStick.L.on) { nm = Walk; }
			if (inp.LStick.R.on) { nm = Walk; }
			if (inp.LStick.U.on) { nm = Walk; }
			if (inp.LStick.D.on) { nm = Walk; }
			if (inp.B4.down) { nm = Attack; }//弾の発射
			break;
		case  Walk:		//歩いている
			if (inp.LStick.L.off&&inp.LStick.R.off
				&&inp.LStick.U.off&&inp.LStick.D.off) { nm = Stand; }
			if (inp.B4.down) { nm = Attack; }
			break;
		case  Attack:	//攻撃中
			if (moveCnt == 8) { nm = Stand; }
			break;
		//case Bound: //ダメージを受けて吹き飛んでいる
		//	if (this->moveCnt >= 12 && this->CheckFoot() == true) {nm = Stand;}
		//	break;
		}
		//モーション更新
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//モーションに対応した処理
	//(モーションは変更しない）
	void  Object::Move()
	{
		auto  inp = this->controller->GetState();
		//移動速度減衰
		switch(this->motion) {
		default:
			if (this->moveVec.x < 0) {
				this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
			}
			else {
				this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
			}
			if (this->moveVec.y < 0) {
				this->moveVec.y = min(this->moveVec.y + this->decSpeed, 0);
			}
			else {
				this->moveVec.y = max(this->moveVec.y - this->decSpeed, 0);
			}
		
		//移動速度減衰を無効化する必要があるモーションは下にcaseを書く（現在対象無し）
		//case Bound:
		case Unnon:	break;
		}
		//-----------------------------------------------------------------
		//モーション毎に固有の処理
		switch (this->motion) {
		case  Stand:	//立っている
			break;
		case  Walk:		//歩いている
			if (inp.LStick.L.on) {
				this->angle_LRFB = Left;
				this->moveVec.x=-this->maxSpeed;
			}
			if (inp.LStick.R.on) {
				this->angle_LRFB = Right;
				this->moveVec.x = this->maxSpeed;
			}
			if (inp.LStick.U.on) {
				this->angle_LRFB = Front;
				this->moveVec.y =- this->maxSpeed;
			}
			if (inp.LStick.D.on){
				this->angle_LRFB = Back;
				this->moveVec.y = this->maxSpeed;
			}
			break;
		case  Attack:	//攻撃中
			//弾を発射
			if (moveCnt == 4) {
				//弾を生成
				auto shot = Shot01::Object::Create(true);
				shot->pos = this->pos;
				if (this->angle_LRFB == Right) {
					shot->moveVec = ML::Vec2(5, 0);
					shot->pos += ML::Vec2(20, 0);
				}
				else if(this->angle_LRFB==Left){
					shot->moveVec = ML::Vec2(-5, 0);
					shot->pos += ML::Vec2(-20, 0);
				}
				else if(this->angle_LRFB==Front){
					shot->moveVec = ML::Vec2(0, -5);
					shot->pos += ML::Vec2(0, -20);
				}
				else {
					shot->moveVec = ML::Vec2(0, 5);
					shot->pos += ML::Vec2(0, 20);
				}
			}
			break;
		}
	}
	//-----------------------------------------------------------------------------
	//接触時の応答処理（必ず受け身の処理として実装する）
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		if (this->unHitTime > 0) {
			return;//無敵時間中はダメージを受けない
		}
		this->unHitTime = 90;
		this->hp -= at_.power;	//仮処理
		if (this->hp <= 0) {
			this->Kill();

			//★データ＆タスク解放
			ge->KillAll_G("フィールド");
			ge->KillAll_G("敵");


			if (!ge->QuitFlag() && this->nextTaskCreate) {
				//★引き継ぎタスクの生成
				//エンディングへ
				auto nextTask = Gameover::Object::Create(true);
			}

		}
		////吹き飛ばされる
		//if (this->pos.x > from_->pos.x) {
		//	this->moveVec = ML::Vec2(+4, -9);
		//}
		//else {
		//	this->moveVec = ML::Vec2(-4, -9);
		//}
		//this->UpdateMotion(Bound);
		//from_は攻撃してきた相手、カウンターなどで逆にダメージを与えたいときに使う
	}
	//-----------------------------------------------------------------------------
	//アニメーション制御
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(0, 0, 64, 64), defColor },	//前向き歩行（右足）//0
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(64, 0, 64, 64), defColor },	//前向き停止//1
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(128, 0, 64, 64), defColor },//前向き歩行（左足)//2

			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(0, 64, 64, 64), defColor },//左向き歩行（右足）//3
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(64, 64, 64, 64), defColor },//左向き停止 //4
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(128, 64, 64, 64), defColor },//左向き歩行（左足）//5

			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(0, 128, 64, 64), defColor },//右向き歩行（右足）//6
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(64, 128, 64, 64), defColor },//右向き停止 //7
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(128, 128, 64, 64), defColor },//右向き歩行（左足）//8

			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(0, 192, 64, 64), defColor },//後ろ向き歩行（左足）//9
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(64, 192, 64, 64), defColor },//後ろ向き停止 //10
			{ ML::Box2D(-16, -16, 32, 32), ML::Box2D(128, 192, 64, 64), defColor },//後ろ向き歩行（右足）//11
		};
		BChara::DrawInfo  rtv;
		int anim[4] = { 1,0,1,2 };//アニメーションパターン
		switch (this->motion) {
		//default:		rtv = imageTable[1];	break;
		//	停止----------------------------------------------------------------------------
		case  Stand:	
				if (this->angle_LRFB == Back) { rtv = imageTable[1]; }
				if (this->angle_LRFB == Left) { rtv = imageTable[4]; }
				if (this->angle_LRFB == Right) { rtv = imageTable[7]; }
				if (this->angle_LRFB == Front) { rtv = imageTable[10]; }
			break;
		//	歩行----------------------------------------------------------------------------
		case  Walk:
			    if (this->angle_LRFB == Back) {
					rtv = imageTable[anim[animCnt / 8 % 4]];
				}
				if (this->angle_LRFB == Left) {
					rtv = imageTable[anim[animCnt / 8 % 4] + 3];
				}
				if (this->angle_LRFB == Right) {
					rtv = imageTable[anim[animCnt / 8 % 4] + 6];
				}
				if (this->angle_LRFB == Front) {
					rtv = imageTable[anim[animCnt / 8 % 4] + 9];
				}
			
			break;
			//	攻撃----------------------------------------------------------------------------
		case  Attack:
			if (this->angle_LRFB == Back) { rtv = imageTable[2]; }
			if (this->angle_LRFB == Left) { rtv = imageTable[3]; }
			if (this->angle_LRFB == Right) { rtv = imageTable[8]; }
			if (this->angle_LRFB == Front) { rtv = imageTable[9]; }
			break;
		//	ダメージ---------------------------------------------------------------------
	/*	case
			Bound:	rtv = imageTable[8];	break;*/
		}
		//向きに応じて画像を左右反転する
	/*	if (Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}*/
		
		return rtv;
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
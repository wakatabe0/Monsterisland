//-------------------------------------------------------------------
//オブジェクトを生成する窓口
//-------------------------------------------------------------------
#include "AppearObject.h"
//生成対象にするものは全てinclude
#include "Task_Enemy00.h"
#include "Task_Player.h"
#include "Task_Item00.h"
#include "Task_Goal.h"
#include "Task_Boat.h"
//-------------------------------------------------------------------
//BCharaを継承しているタスク専用
//名称指定のみで、対応するタスクを生成し、エンジンに登録する
BChara::SP AppearObject_BChara(const string& name_)
{
	//生成
	BChara::SP w = nullptr;
	if ("Player" == name_) { w = Player::Object::Create(true); }
	else if ("Enemy00" == name_) { w = Enemy00::Object::Create(true); }
	else if ("Goal" == name_) { w = Goal::Object::Create(true); }
	else if ("Boat" == name_) { w = Boat::Object::Create(true); }
	else if ("Item00" == name_) { w = Item00::Object::Create(true); }
	return w;
}
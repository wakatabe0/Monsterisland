#include "DG2014_Ver3_6.h"
#include <map>

namespace  DG
{
	//-----------------------------------------------------------------------
	//Image�n
	//-----------------------------------------------------------------------
	unordered_map<string, DG::Image::SP>	ImageManager::arr;
	bool ImageManager::notObject;
	DG::Image::SP  ImageManager::Set(const string name_, DG::Image::SP sp_)
	{
		arr[name_] = sp_;
		return sp_;
	}
	//-----------------------------------------------------------------------
	DG::Image::SP  ImageManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("ImageManager �Ώۂ����݂��܂���"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  ImageManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void ImageManager::AllClear()
	{
		arr.clear();
	}

	//-----------------------------------------------------------------------
	//Font�n
	//-----------------------------------------------------------------------
	unordered_map<string, DG::Font::SP>	FontManager::arr;
	bool FontManager::notObject;
	DG::Font::SP  FontManager::Set(const string name_, DG::Font::SP sp_)
	{
		arr[name_] = sp_;
		return sp_;
	}
	//-----------------------------------------------------------------------
	DG::Font::SP  FontManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("FontManager �Ώۂ����݂��܂���"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  FontManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void FontManager::AllClear()
	{
		arr.clear();
	}
	//-----------------------------------------------------------------------
	//Mesh�n
	//-----------------------------------------------------------------------
	unordered_map<string, DG::Mesh::SP>	MeshManager::arr;
	bool MeshManager::notObject;
	DG::Mesh::SP  MeshManager::Set(const string name_, DG::Mesh::SP sp_)
	{
		arr[name_] = sp_;
		return sp_;
	}
	//-----------------------------------------------------------------------
	DG::Mesh::SP  MeshManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("MeshManager �Ώۂ����݂��܂���"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  MeshManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void MeshManager::AllClear()
	{
		arr.clear();
	}
	//-----------------------------------------------------------------------
	//Mesh�n
	//-----------------------------------------------------------------------
	unordered_map<string, DG::Texture::SP>	TextureManager::arr;
	bool TextureManager::notObject;
	DG::Texture::SP  TextureManager::Set(const string name_, DG::Texture::SP sp_)
	{
		arr[name_] = sp_;
		return  sp_;
	}
	//-----------------------------------------------------------------------
	DG::Texture::SP  TextureManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("TextureManager �Ώۂ����݂��܂���"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  TextureManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void TextureManager::AllClear()
	{
		arr.clear();
	}


	//void  Line_Initialize();
	//void  Line_Finalize();

	//D3D10_PRIMITIVE_TOPOLOGY	line_priTopo;		//�v���~�e�B�u�g�|���W�[
	//DG::InputLayout::SP		line_inputLayout;	//���_���̓��̓��C�A�E�g
	//DWORD						line_tecNum;		//�e�N�j�b�N�ԍ�
	//DG::VertexBuffer::SP		line_vPos;

	////-----------------------------------------------------------------------
	//void  Line_Draw(
	//	const ML::Vec3&	posS_,
	//	const ML::Vec3&	posE_)
	//{
	//	//�o�[�e�b�N�X�o�b�t�@�̐���
	//	ML::Vec3	pos[] = { posS_, posE_ };
	//	auto  vPos = DG::VertexBuffer::Create((BYTE*)pos, sizeof(ML::Vec3), _countof(pos));

	//	//�`��p�����[�^��萔�o�b�t�@�ɔ��f������
	//	obj->EffectState().Update_Params();

	//	//�e�N�j�b�N�I���i�Ăяo�����V�F�[�_�����肷��j
	//	obj->ETechnique().GetPassByIndex(line_tecNum)->Apply(0);
	//	//���_�C���v�b�g���C�A�E�g���Z�b�g
	//	line_inputLayout->IASet();
	//	//�v���~�e�B�u�E�g�|���W�[���Z�b�g
	//	obj->Device().IASetPrimitiveTopology(line_priTopo);

	//	//���_�o�b�t�@���e�X���b�g�ɐڑ��i�f�ޓ������I���j
	//	UINT  ofsPos = 0 * sizeof(ML::Vec3);
	//	obj->Device().IASetVertexBuffers(0, 1, &vPos->buf, &vPos->stride, &ofsPos);
	//	//�����_�����O�J�n
	//	obj->Device().Draw(vPos->num, 0);
	//}
	//void  Line_Initialize()
	//{
	//	//�o�[�e�b�N�X�V�F�[�_���������_�C���v�b�g���C�A�E�g����
	//	//���_�C���v�b�g���C�A�E�g���`	
	//	D3D10_INPUT_ELEMENT_DESC layout[] = {
	//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	//	};
	//	//���_�C���v�b�g���C�A�E�g�̐���
	//	line_tecNum = 2;
	//	line_inputLayout = DG::InputLayout::Create(line_tecNum, layout, _countof(layout));
	//	//�v���~�e�B�u�E�g�|���W�[��I��
	//	line_priTopo = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
	//}
	//void  Line_Finalize()
	//{
	//}
}
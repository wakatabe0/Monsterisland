#pragma warning(disable:4996)
#pragma once

#include "DG2014_DGObject.h"

#include "DG2014_IndexBuffer.h"
#include "DG2014_InputLayout.h"
#include "DG2014_VertexBuffer.h"
#include "DG2014_VertexContainer.h"

#include "DG2014_Texture.h"
#include "DG2014_Mesh.h"
#include "DG2014_Image.h"
#include "DG2014_Font.h"
#include "DG2014_Effect.h"

namespace
{
	enum{	spriteBufferMax = 4096};
}


namespace DG
{
	//---------------------------------------------------------------------------
	//�摜�}�l�[�W���[
	class  ImageManager {
	private:
		static  unordered_map<string, DG::Image::SP>	arr;
		static  bool notObject;
		ImageManager() { notObject = false; }
	public:
		//�o�^
		static  DG::Image::SP  Set(const string name_, DG::Image::SP sp_);
		//���O�Ō���
		static  DG::Image::SP  Get(const string name_);
		//�P�̔j��
		static  void  Clear(const string name_);
		//���ׂĔj��
		static  void  AllClear();
	};
	//---------------------------------------------------------------------------
	//�t�H���g�}�l�[�W���[
	class  FontManager {
	private:
		static  unordered_map<string, DG::Font::SP>	arr;
		static  bool notObject;
		FontManager() { notObject = false; }
	public:
		//�o�^
		static  DG::Font::SP  Set(const string name_, DG::Font::SP sp_);
		//���O�Ō���
		static  DG::Font::SP  Get(const string name_);
		//�P�̔j��
		static  void  Clear(const string name_);
		//���ׂĔj��
		static  void  AllClear();
	};
	//---------------------------------------------------------------------------
	//���b�V���}�l�[�W���[
	class  MeshManager {
	private:
		static  unordered_map<string, DG::Mesh::SP>	arr;
		static  bool notObject;
		MeshManager() { notObject = false; }
	public:
		//�o�^
		static  DG::Mesh::SP  Set(const string name_, DG::Mesh::SP sp_);
		//���O�Ō���
		static  DG::Mesh::SP  Get(const string name_);
		//�P�̔j��
		static  void  Clear(const string name_);
		//���ׂĔj��
		static  void  AllClear();
	};
	//---------------------------------------------------------------------------
	//�e�N�X�`���}�l�[�W���[
	class  TextureManager {
	private:
		static  unordered_map<string, DG::Texture::SP>	arr;
		static  bool notObject;
		TextureManager() { notObject = false; }
	public:
		//�o�^
		static  DG::Texture::SP  Set(const string name_, DG::Texture::SP sp_);
		//���O�Ō���
		static  DG::Texture::SP  Get(const string name_);
		//�P�̔j��
		static  void  Clear(const string name_);
		//���ׂĔj��
		static  void  AllClear();
	};
	//-----------------------------------------------------------------------
	//void  Line_Draw(
	//	const ML::Vec3&	posS_,
	//	const ML::Vec3&	posE_);
}
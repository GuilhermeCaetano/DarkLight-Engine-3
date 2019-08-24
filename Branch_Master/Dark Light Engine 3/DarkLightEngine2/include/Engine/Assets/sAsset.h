#ifndef _sAsset_HG_
#define _sAsset_HG_

#include <string>
#include <Mesh\sModelDrawInfo.h>


struct sAsset
{	
public:
	enum eAssetTypes
	{
		MESH_COMMON,
		MESH_SKINNED,
		MESH_ANIMATION,
		TEXTURE,
		AUDIO
	};

	enum eAssetStates
	{
		UNLOADED,
		LOADING,
		LOADED_INTO_CPU,
		LOADED_INTO_GPU
	};

	sAsset(const std::string& name, const std::string& friendlyName, sAsset::eAssetTypes assetType)
	{
		this->_name = name;
		this->_friendlyName = friendlyName;
		this->_assetType = assetType;
		this->_assetState = eAssetStates::UNLOADED;
		this->_assetInfo = nullptr;
		this->_ownsData = false;
	}

	~sAsset()
	{
		if (this->_ownsData && this->_assetInfo != nullptr)
		{
			this->DestroyAssetInfo();
		}
	}

	void DestroyAssetInfo()
	{
		if (this->_assetType == eAssetTypes::MESH_COMMON || this->_assetType == eAssetTypes::MESH_SKINNED)
		{
			sModelDrawInfo* modelInfo = (sModelDrawInfo*)this->_assetInfo;
			delete modelInfo;
		}
		else if (this->_assetType == eAssetTypes::TEXTURE)
		{

		}

		this->_assetInfo = nullptr;
		this->_ownsData = false;
	}

	std::string _name;
	std::string _friendlyName;
	eAssetTypes _assetType;
	eAssetStates _assetState;
	void* _assetInfo;
	bool _ownsData;

};



#endif // !_sAsset_HG_


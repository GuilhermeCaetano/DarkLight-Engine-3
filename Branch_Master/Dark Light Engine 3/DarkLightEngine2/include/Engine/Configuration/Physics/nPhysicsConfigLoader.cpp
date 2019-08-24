// nPhysicsConfigLoader.cpp

#include "nPhysicsConfigLoader.h"
#include <Loaders\nJsonLoader.h>
#include <Engine\Configuration\sConfig.h>
#include <Interfaces\Physics\iPhysicsFactory.h>
#include <Windows.h>


namespace nConfig
{
	// This is used to define a function named f_CreateRigidbody,
	// with no params (), and of return type iRigidbodyFactory*
	typedef nPhysics::iPhysicsFactory*(*f_CreateFactory)();

	bool LoadPhysicsDLL(std::string& pathName, sConfig& outConfig)
	{
		// Load document from json file
		rapidjson::Document jsonDoc;
		if (!nLoad::LoadJsonFile(pathName, jsonDoc))
		{
			// Error and bail out
			printf("Error! Failed to open Json config file!\n");
			return false;
		}

		// Get physics dll type to use
		rapidjson::Value::ConstMemberIterator load = jsonDoc.FindMember("PhysicsDllType");
		if (!load->value.IsString())
		{
			printf("Error, wrong json format for Physics dll!\n");
			return false;
		}

		outConfig.physicsDLLType = load->value.GetString();
		return true;
	}

	nPhysics::iPhysicsFactory* CreatePhysicsFactory(HINSTANCE& hProckDLL, const sConfig& config)
	{
		// Load desired library using GetProcDLL
		hProckDLL = LoadLibraryA(config.physicsDLLType.c_str());
		if (!hProckDLL)
		{
			printf("Error! Failed to load library %s!\n", config.physicsDLLType.c_str());
			return false;
		}

		// Create factory from the .dll and return it
		f_CreateFactory CreateFactory = (f_CreateFactory)GetProcAddress(hProckDLL, "CreateFactory");
		if (!CreateFactory)
		{
			printf("Error! Couldn't create factory from dll!\n");
			return false;
		}

		nPhysics::iPhysicsFactory* pPhysicsFactory = CreateFactory();
		return pPhysicsFactory;
	}
}


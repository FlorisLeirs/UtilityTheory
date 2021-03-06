#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"
#include "Steering.h"
#include "UtilitySystem.h"
#include "SteeringBehaviour.h"
#include "ExploreBehaviour.h"
#include "Inventory.h"
#include "ItemUsage.h"

using namespace Utility;

//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "UtilityAI";
	info.Student_FirstName = "Floris";
	info.Student_LastName = "Leirs";
	info.Student_Class = "2DAE07";

	m_pInventory = new Inventory(m_pInterface->Inventory_GetCapacity());
	m_pUtilitySystem = new UtilitySystem(m_pInventory);
	m_pUtilitySystem->AddBehavior(new FleeBehaviour());
	m_pUtilitySystem->AddBehavior(new WanderBehaviour());
	m_pUtilitySystem->AddBehavior(new MoveToHouseBehaviour());
	m_pUtilitySystem->AddBehavior(new GetOutOfHouse());
	m_pUtilitySystem->AddBehavior(new PickUpItem(m_pInventory));
	m_pUtilitySystem->AddBehavior(new Heal(m_pInventory));
	m_pUtilitySystem->AddBehavior(new Eat(m_pInventory));
	m_pUtilitySystem->AddBehavior(new Shoot(m_pInventory));
}

//Called only once
void Plugin::DllInit()
{
	//Called when the plugin is loaded

}

//Called only once
void Plugin::DllShutdown()
{
	//Called when the plugin gets unloaded
	delete m_pUtilitySystem;
	m_pUtilitySystem = nullptr;

	delete m_pInventory;
	m_pInventory = nullptr;
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be usefull to inspect certain behaviours (Default = false)
	params.AutoGrabClosestItem = false; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		//m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_GrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = false;
	}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	auto steering = SteeringPlugin_Output();

	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterface->Agent_GetInfo();

	auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(m_Target); //To start you can use the mouse position as guidance

	auto vHousesInFOV = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	auto vEntitiesInFOV = GetEntitiesInFOV(); //uses m_pInterface->Fov_GetEntityByIndex(...)


	for (auto& e : vEntitiesInFOV)
	{
		if (e.Type == eEntityType::PURGEZONE)
		{
			PurgeZoneInfo zoneInfo;
			m_pInterface->PurgeZone_GetInfo(e, zoneInfo);
			std::cout << "Purge Zone in FOV:" << e.Location.x << ", " << e.Location.y << " ---EntityHash: " << e.EntityHash << "---Radius: " << zoneInfo.Radius << std::endl;
		}

	}


	//INVENTORY USAGE DEMO
	//********************

	if (m_GrabItem)
	{
		ItemInfo item;
		//Item_Grab > When DebugParams.AutoGrabClosestItem is TRUE, the Item_Grab function returns the closest item in range
		//Keep in mind that DebugParams are only used for debugging purposes, by default this flag is FALSE
		//Otherwise, use GetEntitiesInFOV() to retrieve a vector of all entities in the FOV (EntityInfo)
		//Item_Grab gives you the ItemInfo back, based on the passed EntityHash (retrieved by GetEntitiesInFOV)
		if (m_pInterface->Item_Grab({}, item))
		{
			//Once grabbed, you can add it to a specific inventory slot
			//Slot must be empty
			m_pInterface->Inventory_AddItem(0, item);
		}
	}

	if (m_UseItem)
	{
		//Use an item (make sure there is an item at the given inventory slot)
		m_pInterface->Inventory_UseItem(0);
	}

	if (m_RemoveItem)
	{
		//Remove an item from a inventory slot
		m_pInterface->Inventory_RemoveItem(0);
	}

	////Simple m_pSeek Behaviour (towards Target)
	//steering.LinearVelocity = nextTargetPos - agentInfo.Position; //Desired Velocity
	//steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	//steering.LinearVelocity *= agentInfo.MaxLinearSpeed; //Rescale to Max Speed

	//if (Distance(m_Target, agentInfo.Position) < 2.f)
	//{
	//	steering.LinearVelocity = Elite::ZeroVector2;
	//}

	m_pUtilitySystem->SetCurrentBehaviour(m_pInterface, dt);
	steering = m_pUtilitySystem->ExecuteBehaviour(m_pInterface);
	SteerAwayFromWorldBorders(steering);

	//steering.LinearVelocity = flee.GetSteering(m_pInterface).LinearVelocity;

	//steering.AngularVelocity = m_AngSpeed; //Rotate your character to inspect the world while walking
	//steering.AutoOrient = false; //Setting AutoOrientate to TRue overrides the AngularVelocity
	//steering.AngularVelocity = agentInfo.MaxAngularSpeed;

	if (agentInfo.Stamina > 0.f)
		m_CanRun = true;
	steering.RunMode = (steering.RunMode == true) ? m_CanRun : false; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)
	//steering.LinearVelocity.Normalize();
	//steering.LinearVelocity *= agentInfo.MaxLinearSpeed;
								 //SteeringPlugin_Output is works the exact same way a SteeringBehaviour output

								 //@End (Demo Purposes)
	m_Target = steering.LinearVelocity + agentInfo.Position;

	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;

	return steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
	
	m_pInterface->Draw_Direction(m_pInterface->Agent_GetInfo().Position, Elite::Vector2(cosf(m_pInterface->Agent_GetInfo().Orientation - static_cast<float>(E_PI_2)), sinf(m_pInterface->Agent_GetInfo().Orientation - static_cast<float>(E_PI_2))), 20.f, {0,0,1});
}

vector<HouseInfo> Plugin::GetHousesInFOV() const
{
	vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}

vector<EntityInfo> Plugin::GetEntitiesInFOV() const
{
	vector<EntityInfo> vEntitiesInFOV = {};

	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			vEntitiesInFOV.push_back(ei);
			continue;
		}

		break;
	}

	return vEntitiesInFOV;
}

void Plugin::SteerAwayFromWorldBorders(SteeringPlugin_Output& steering)
{
	float maxDistanceFromBorder{ 10.f };
	SteeringPlugin_Output extraSteering{};
	auto agentPos = m_pInterface->Agent_GetInfo().Position;
	auto worldInfo = m_pInterface->World_GetInfo();
	if (agentPos.x - (worldInfo.Center.x + worldInfo.Dimensions.x / 2) > maxDistanceFromBorder)
	{
		extraSteering.LinearVelocity.x = -1.f;
		extraSteering.AngularVelocity = 5.f;
	}
	else if (agentPos.x - (worldInfo.Center.x - worldInfo.Dimensions.x / 2) < maxDistanceFromBorder)
	{
		extraSteering.LinearVelocity.x = 1.f;
		extraSteering.AngularVelocity = 5.f;
	}

	if (agentPos.y - (worldInfo.Center.y + worldInfo.Dimensions.y / 2) > maxDistanceFromBorder / 2)
	{
		extraSteering.LinearVelocity.y = -1.f;
		extraSteering.AngularVelocity = 5.f;
	}
	else if (agentPos.y - (worldInfo.Center.y - worldInfo.Dimensions.y / 2) < maxDistanceFromBorder / 2)
	{
		extraSteering.LinearVelocity.y = 1.f;
		extraSteering.AngularVelocity = 5.f;
	}

	if (extraSteering.AngularVelocity > 0.f)
	{
		extraSteering.LinearVelocity.Normalize();
		extraSteering.LinearVelocity *= m_pInterface->Agent_GetInfo().MaxLinearSpeed;
		steering.LinearVelocity *= .3f;
		steering.LinearVelocity += extraSteering.LinearVelocity * .7f;
		steering.LinearVelocity.Normalize();
		steering.LinearVelocity *= m_pInterface->Agent_GetInfo().MaxLinearSpeed;
	}
}

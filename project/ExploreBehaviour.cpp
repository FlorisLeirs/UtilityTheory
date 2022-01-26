#include "stdafx.h"
#include "ExploreBehaviour.h"
#include "IExamInterface.h"
#include "Inventory.h"
#include "Steering.h"
#include "UtilityEquations.h"

//----------------
// MOVE TO HOUSE
//----------------
Utility::MoveToHouseBehaviour::MoveToHouseBehaviour()
	: BaseBehaviour(0.1f)
{
}

void Utility::MoveToHouseBehaviour::CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots)
{
	m_Utility = Logistic(5.f, static_cast<float>(emptyInventorySlots), 1.f) / 1.7f;
}

SteeringPlugin_Output Utility::MoveToHouseBehaviour::Execute(IExamInterface* pInterface)
{
	auto steering = Seek(pInterface, m_HouseCenter.r[0]);
	return steering;
}

bool Utility::MoveToHouseBehaviour::IsFinished(const IExamInterface* pInterface, const float dt)
{
	m_ElapsedTime += dt;
	if (m_ElapsedTime >= 3.f)
	{
		m_ElapsedTime = 0.f;
		return true;
	}
	if (pInterface->Agent_GetInfo().IsInHouse && pInterface->Agent_GetInfo().Position.DistanceSquared(m_HouseCenter.r[0]) < 100.f)
	{
		m_HouseCenter.r[1] = m_HouseCenter.r[0];
		m_HouseCenter.r[0] =Elite::Vector2();
		return true;
	}
	return false;
}

bool Utility::MoveToHouseBehaviour::IsValid(IExamInterface* pInterface, const float dt)
{
	if (pInterface->Agent_GetInfo().IsInHouse)
		return false;

	vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (pInterface->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	if (vHousesInFOV.empty())
		return false;

	Elite::Vector2 temp{ m_HouseCenter.r[0] };
	bool isNewHouse{ false };
	float distanceSquared{ pInterface->Agent_GetInfo().Position.DistanceSquared(m_HouseCenter.r[1]) };
	for_each(vHousesInFOV.begin(), vHousesInFOV.end(), [&](HouseInfo houseInfo)
		{
			if (abs(m_HouseCenter.r[1].Magnitude() - houseInfo.Center.Magnitude()) > FLT_EPSILON && distanceSquared > pInterface->Agent_GetInfo().Position.DistanceSquared(houseInfo.Center))
			{
				temp = houseInfo.Center;
				distanceSquared = pInterface->Agent_GetInfo().Position.DistanceSquared(houseInfo.Center);
				isNewHouse = true;
			}

		});

	if (isNewHouse)
	{
		m_HouseCenter.r[1] = m_HouseCenter.r[0];
		m_HouseCenter.r[0] = temp;
	}
	if(m_HouseCenter.r[0] == Elite::Vector2())
		return false;

	return true;
}


//---------------
// PICK UP ITEM
//---------------
Utility::PickUpItem::PickUpItem(Inventory* pInventory)
	:BaseBehaviour(.9f)
	, m_pInventory(pInventory)
{
}

void Utility::PickUpItem::CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots)
{
}

SteeringPlugin_Output Utility::PickUpItem::Execute(IExamInterface* pInterface)
{
	SteeringPlugin_Output steering{};

	if (m_ItemPos.Distance(pInterface->Agent_GetInfo().Position) < pInterface->Agent_GetInfo().GrabRange)
	{
		if (m_pInventory->GetAmountOfEmptySlots() == 0)
		{
			m_Done = true;
			return SteeringPlugin_Output{};
		}

		ItemInfo ii{};
		EntityInfo ei = {};
		for (int i = 0;; ++i)
		{
			if (pInterface->Fov_GetEntityByIndex(i, ei))
			{
				if (ei.Location == m_ItemPos)
				{
					pInterface->Item_GetInfo(ei, ii);
					pInterface->Item_Grab(ei, ii);
					switch (ii.Type)
					{
					case eItemType::PISTOL:
						m_pInventory->AddPistol(ii, pInterface);
						break;
					case eItemType::FOOD:
						m_pInventory->AddFood(ii, pInterface);
						break;
					case eItemType::MEDKIT:
						m_pInventory->AddMedKit(ii, pInterface);
						break;
					default:
						pInterface->Item_Destroy(ei);
						break;
					}
					m_Done = true;

					Rotate(10.f, pInterface->Agent_GetInfo().MaxAngularSpeed, steering);
					return steering;
				}
				continue;
			}

			break;
		}
		Rotate(10.f, pInterface->Agent_GetInfo().MaxAngularSpeed, steering);
		return steering;
	}

	steering = Seek(pInterface, m_ItemPos);
	steering.AutoOrient = true;
	return steering;
}

bool Utility::PickUpItem::IsFinished(const IExamInterface* pInterface, const float dt)
{
	return m_Done;
}

bool Utility::PickUpItem::IsValid(IExamInterface* pInterface, const float dt)
{
	m_Done = false;

	if (m_pInventory->GetAmountOfEmptySlots() == 0)
		return false;

	ItemInfo ii{};
	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (pInterface->Fov_GetEntityByIndex(i, ei))
		{
			if (ei.Type == eEntityType::ITEM)
			{
				pInterface->Item_GetInfo(ei, ii);
				switch (ii.Type)
				{
				case eItemType::PISTOL:
					if (m_pInventory->GetAmountOfPistols() < 2)
					{
						m_ItemPos = ii.Location;
						return true;
					}
					break;
				case eItemType::FOOD:
					if (m_pInventory->GetAmountOfFood() < 2)
					{
						m_ItemPos = ii.Location;
						return true;
					}
					break;
				case eItemType::MEDKIT:
					if (m_pInventory->GetAmountOfMedKits() < 2)
					{
						m_ItemPos = ii.Location;
						return true;
					}
					break;
				default:
					return false;
				}

			}
			continue;
		}

		break;
	}

	return false;
}


//--------------------
// GET OUT OF HOUSE
//--------------------
Utility::GetOutOfHouse::GetOutOfHouse()
	: BaseBehaviour(0.15f)

{
}

void Utility::GetOutOfHouse::CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots)
{
}

SteeringPlugin_Output Utility::GetOutOfHouse::Execute(IExamInterface* pInterface)
{
	return Seek(pInterface, pInterface->World_GetInfo().Center);
}

bool Utility::GetOutOfHouse::IsFinished(const IExamInterface* pInterface, const float dt)
{
	return !pInterface->Agent_GetInfo().IsInHouse;
}

bool Utility::GetOutOfHouse::IsValid(IExamInterface* pInterface, const float dt)
{
	if (pInterface->Agent_GetInfo().IsInHouse)
	{
		m_TimeInHouse += dt;
		if (m_TimeInHouse > 10.f)
		{
			m_TimeInHouse = 0.f;
			return  true;
		}
		return false;
	}
	if (m_TimeInHouse > FLT_EPSILON)
		m_TimeInHouse = 0.f;
	return false;
}
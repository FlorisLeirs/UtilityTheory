#include "stdafx.h"
#include "ItemUsage.h"

#include <IExamInterface.h>

#include "Inventory.h"
#include "UtilityEquations.h"
#include "Steering.h"


//------------
// HEAL
//------------
Utility::Heal::Heal(Inventory* pInventory)
	:BaseBehaviour(0.1f)
	, m_pInventory(pInventory)
{
}

void Utility::Heal::CalculateUtility(IExamInterface* pInterface, const UINT)
{
	m_Utility = std::max<float>(std::min<float>(Linear(0.7f, pInterface->Agent_GetInfo().Health), 1.f), 0.65f);
}

SteeringPlugin_Output Utility::Heal::Execute(IExamInterface* pInterface)
{
	m_pInventory->UseMedKit(pInterface);
	return SteeringPlugin_Output();
}

bool Utility::Heal::IsFinished(const IExamInterface* pInterface, const float dt)
{
	return true;
}

bool Utility::Heal::IsValid(IExamInterface* pInterface, const float dt)
{
	return m_pInventory->GetAmountOfMedKits() != 0 && pInterface->Agent_GetInfo().Health < 8.f;
}


//-------------
// EAT
//-------------
Utility::Eat::Eat(Inventory* pInventory)
	: BaseBehaviour(0.4f)
	, m_pInventory(pInventory)
{
}

void Utility::Eat::CalculateUtility(IExamInterface* pInterface, const UINT)
{
	m_Utility = std::max<float>(std::min<float>(Linear(pInterface->Agent_GetInfo().Energy, -10.f) + 1, 1.f), 0.45f);
}

SteeringPlugin_Output Utility::Eat::Execute(IExamInterface* pInterface)
{
	m_pInventory->UseFood(pInterface);
	return SteeringPlugin_Output();
}

bool Utility::Eat::IsFinished(const IExamInterface* pInterface, const float dt)
{
	return true;
}

bool Utility::Eat::IsValid(IExamInterface* pInterface, const float dt)
{
	return m_pInventory->GetAmountOfFood() != 0 && pInterface->Agent_GetInfo().Energy < 5.f;
}


//-----------
// SHOOT
//-----------
Utility::Shoot::Shoot(Inventory* pInventory)
	: BaseBehaviour(0.5f)
	, m_pInventory(pInventory)
{
}

void Utility::Shoot::CalculateUtility(IExamInterface* pInterface, const UINT)
{
	m_Utility = std::max<float>(std::min<float>(Quadratic(pInterface->Agent_GetInfo().Health - 10.f, -10.f, 1.f), .1f), 0.7f);
}

SteeringPlugin_Output Utility::Shoot::Execute(IExamInterface* pInterface)
{
	std::vector<EnemyInfo> zombies{};
	GetZombieLocationsInFov(pInterface, zombies);
	SteeringPlugin_Output steering{};
	auto agentInfo = pInterface->Agent_GetInfo();
	if (zombies.empty())
	{
		Rotate(10.f, agentInfo.MaxAngularSpeed, steering);
		return steering;
	}

	EnemyInfo closestZombie{};
	float distance{ FLT_MAX };
	for_each(zombies.cbegin(), zombies.cend(), [&closestZombie, &agentInfo, &distance](const EnemyInfo& zombie)
		{
			if (agentInfo.Position.DistanceSquared(zombie.Location) < distance)
			{
				distance = agentInfo.Position.DistanceSquared(zombie.Location);
				closestZombie = zombie;
			}
		});

	if (IsInLineOfFire(closestZombie, agentInfo))
	{
		m_pInventory->UsePistol(pInterface);
		m_Done = true;
	}
	else
	{
		float angle = atan2f((closestZombie.Location - agentInfo.Position).y, (closestZombie.Location - agentInfo.Position).x);
		angle = angle > static_cast<float>(E_PI) ?  static_cast<float>(E_PI) - angle : angle;
		Rotate(angle, agentInfo.MaxAngularSpeed, steering);
	}

	return steering;
}

bool Utility::Shoot::IsFinished(const IExamInterface* pInterface, const float dt)
{
	return m_Done;
}

bool Utility::Shoot::IsValid(IExamInterface* pInterface, const float dt)
{
	if (m_pInventory->GetAmountOfPistols() == 0)
		return false;

	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (pInterface->Fov_GetEntityByIndex(i, ei))
		{
			if (ei.Type == eEntityType::ENEMY)
			{
				m_Done = false;
				return true;
			}
			continue;
		}
		return false;
	}
}

void Utility::Shoot::GetZombieLocationsInFov(IExamInterface* pInterface, std::vector<EnemyInfo>& zombies)
{
	EntityInfo ei = {};
	EnemyInfo enemy{};
	for (int i = 0;; ++i)
	{
		if (pInterface->Fov_GetEntityByIndex(i, ei))
		{
			if (ei.Type == eEntityType::ENEMY)
			{
				pInterface->Enemy_GetInfo(ei, enemy);
				zombies.push_back(enemy);
			}
			continue;
		}
		m_Done = true;
		return;
	}
}

bool Utility::Shoot::IsInLineOfFire(const EnemyInfo& zombie, const AgentInfo& agentInfo)
{
	Elite::Vector2 rayDirection = Elite::Vector2(cosf(agentInfo.Orientation - static_cast<float>(E_PI_2)),
		sinf(agentInfo.Orientation - static_cast<float>(E_PI_2)));
	Elite::Vector2 sphereToRay = agentInfo.Position - zombie.Location;
	rayDirection.Normalize();
	float discriminant{};
	const float a = rayDirection.SqrtMagnitude();
	const float b = Elite::Dot((2 * rayDirection), sphereToRay);
	const float c = Elite::Dot(sphereToRay, sphereToRay) - zombie.Size * zombie.Size;

	discriminant = b * b - 4 * a * c;

	return discriminant >= 0.f;
}

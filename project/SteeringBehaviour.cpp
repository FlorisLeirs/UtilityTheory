#include "stdafx.h"
#include "SteeringBehaviour.h"

#include <IExamInterface.h>

#include "Steering.h"
#include "UtilityEquations.h"

Utility::FleeBehaviour::FleeBehaviour()
	: BaseBehaviour(0.2f)
{
}

Utility::FleeBehaviour::~FleeBehaviour()
{

}

void Utility::FleeBehaviour::CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots)
{
	m_Utility = std::max<float>(std::min<float>(Linear(0.7f, pInterface->Agent_GetInfo().Health), 0.9f), 0.2f);
}

SteeringPlugin_Output Utility::FleeBehaviour::Execute(IExamInterface* pInterface)
{
	SteeringPlugin_Output steering{};

	std::vector<Elite::Vector2> fleeTargets{};
	GetZombieLocationsInFov(pInterface, fleeTargets);

	if (!fleeTargets.empty())
		SetDesiredLinearVelocity(pInterface->Agent_GetInfo(), fleeTargets);

	steering = Flee(pInterface, m_DesiredLinearVelocity);
	Rotate(10.f, pInterface->Agent_GetInfo().MaxAngularSpeed, steering);
	steering.RunMode = m_Run;
	return steering;
}

bool Utility::FleeBehaviour::IsFinished(const IExamInterface* pInterface, const float dt)
{
	m_TimeFleeing += dt;
	if (m_TimeFleeing >= 3.f)
	{
		m_TimeFleeing = 0.f;
		m_DesiredLinearVelocity = { pInterface->Agent_GetInfo().LinearVelocity };
		return true;
	}
	return false;
}

bool Utility::FleeBehaviour::IsValid(IExamInterface* pInterface, const float dt)
{
	bool enemyinFOV{ false };
	m_Run = false;
	EntityInfo ei = {};

	for (int i = 0;; ++i)
	{
		if (pInterface->Fov_GetEntityByIndex(i, ei))
		{
			if (ei.Type == eEntityType::ENEMY)
			{
				EnemyInfo enemy{};
				if (pInterface->Enemy_GetInfo(ei, enemy))
				{
					if (enemy.Type == eEnemyType::ZOMBIE_RUNNER || enemy.Location.Distance(pInterface->Agent_GetInfo().Position) < pInterface->Agent_GetInfo().FOV_Range / 2.f)
						m_Run = true;
					enemyinFOV = true;
				}
			}
			continue;
		}

		break;
	}
	if (pInterface->Agent_GetInfo().WasBitten)
	{
		m_DesiredLinearVelocity += pInterface->Agent_GetInfo().LinearVelocity;
		m_TimeFleeing = 1.5f;
		m_Run = true;
		return true;
	}
	return enemyinFOV;
}

void Utility::FleeBehaviour::GetZombieLocationsInFov(IExamInterface* pInterface, std::vector<Elite::Vector2>& locations)
{
	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (pInterface->Fov_GetEntityByIndex(i, ei))
		{
			if (ei.Type == eEntityType::ENEMY)
			{
				EnemyInfo enemy{};
				if (pInterface->Enemy_GetInfo(ei, enemy))
				{
					locations.push_back(enemy.Location);
					//locations.push_back(enemy.LinearVelocity);
				}
				
			}
			continue;
		}
		return;
	}
}

void Utility::FleeBehaviour::SetDesiredLinearVelocity(const AgentInfo& agent, const std::vector<Elite::Vector2>& targets)
{
	for (auto currentTarget : targets)
	{
		Elite::Vector2 currentSteering;
		//currentSteering = currentTarget - agent.Position;
		currentSteering = currentTarget;
		//currentSteering.Normalize();
		//currentSteering *= 100.f;
		//currentSteering /= Elite::Distance(agent.Position, currentTarget);
		m_DesiredLinearVelocity += currentSteering;
	}

}



//------------
// WANDER
//------------
Utility::WanderBehaviour::WanderBehaviour()
	: BaseBehaviour(0.05f)
{
}

Utility::WanderBehaviour::~WanderBehaviour()
{

}

void Utility::WanderBehaviour::CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots)
{

}

SteeringPlugin_Output Utility::WanderBehaviour::Execute(IExamInterface* pInterface)
{
	auto agent = pInterface->Agent_GetInfo();
	m_AngleRadians += Elite::randomFloat(-m_MaxAngleChange, m_MaxAngleChange);
	Elite::Vector2 targetLocation{ cos(m_AngleRadians), sin(m_AngleRadians) };
	targetLocation = targetLocation.GetNormalized() * m_Radius;
	targetLocation += agent.Position + agent.LinearVelocity.GetNormalized() * m_TargetOffset;

	auto steering = Seek(pInterface, targetLocation);
	m_PrevDirection = steering.LinearVelocity.GetNormalized();
	Rotate(10.f, pInterface->Agent_GetInfo().MaxAngularSpeed, steering);
	return steering;

}

bool Utility::WanderBehaviour::IsFinished(const IExamInterface* pInterface, const float)
{
	return true;
}




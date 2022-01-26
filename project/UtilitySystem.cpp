#include "stdafx.h"
#include "UtilitySystem.h"

#include "Inventory.h"
#include "Steering.h"
#include "SteeringBehaviour.h"

Utility::UtilitySystem::UtilitySystem(Inventory* pInventory)
	: m_pCurrentBehaviour(new WanderBehaviour())
	, m_pInventory(pInventory)
{
}

Utility::UtilitySystem::~UtilitySystem()
{
	m_pCurrentBehaviour = nullptr;
	m_pPossibleBehaviours.clear();
	for (auto& pE : m_pBehaviours)
	{
		delete pE;
		pE = nullptr;
	}
	m_pBehaviours.clear();

	m_pInventory = nullptr;
}

void Utility::UtilitySystem::SetCurrentBehaviour(IExamInterface* pInterface, const float dt)
{
	if (m_pCurrentBehaviour->IsFinished(pInterface, dt))
	{
		CheckValidBehaviours(pInterface, dt);

		for (auto& pBehaviour : m_pPossibleBehaviours)
		{
			pBehaviour->CalculateUtility(pInterface, m_pInventory->GetAmountOfEmptySlots());
		}

		const auto mostDesiredBehaviour = std::max_element(m_pPossibleBehaviours.begin(), m_pPossibleBehaviours.end(), BehaviourComp);
		m_pCurrentBehaviour = *mostDesiredBehaviour._Ptr;

		m_pPossibleBehaviours.clear();
	}


}

SteeringPlugin_Output Utility::UtilitySystem::ExecuteBehaviour(IExamInterface* pInterface) const
{
	return m_pCurrentBehaviour->Execute(pInterface);
}

void Utility::UtilitySystem::CheckValidBehaviours(IExamInterface* pInterface, const float dt)
{
	for(auto& pBehaviour : m_pBehaviours)
	{
		if(pBehaviour->IsValid(pInterface, dt))
			m_pPossibleBehaviours.push_back(pBehaviour);
	}
}

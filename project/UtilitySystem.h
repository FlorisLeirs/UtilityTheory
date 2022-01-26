#pragma once
#include "BaseBehaviour.h"
#include "Exam_HelperStructs.h"
class IExamInterface;
class Inventory;
namespace Utility
{
	class UtilitySystem
	{
	public:
		UtilitySystem(Inventory* pInventory);
		~UtilitySystem();

		void SetCurrentBehaviour(IExamInterface* pInterface, const float dt);
		SteeringPlugin_Output ExecuteBehaviour(IExamInterface* pInterface) const;

		void AddBehavior(BaseBehaviour* pBehaviour) { m_pBehaviours.push_back(pBehaviour); }

	private:
		BaseBehaviour* m_pCurrentBehaviour = nullptr;
		std::vector<BaseBehaviour*> m_pBehaviours;
		std::vector<BaseBehaviour*> m_pPossibleBehaviours{};
		Inventory* m_pInventory;

		void CheckValidBehaviours(IExamInterface* pInterface, const float dt);
	};
}


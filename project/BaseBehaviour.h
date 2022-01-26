#pragma once
#include "Exam_HelperStructs.h"
class IExamInterface;

namespace Utility
{
	class BaseBehaviour
	{
	public:
		BaseBehaviour(float baseUtility) : m_Utility(baseUtility) {}
		virtual ~BaseBehaviour() = default;

		virtual void CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots) = 0;
		virtual SteeringPlugin_Output Execute(IExamInterface* pInterface) = 0;
		virtual bool IsFinished(const IExamInterface* pInterface, const float dt) = 0;
		virtual bool IsValid(IExamInterface* pInterface, const float dt) = 0;

		bool operator>(const BaseBehaviour& rhs) const { return m_Utility > rhs.m_Utility; }
		bool operator<(const BaseBehaviour& rhs) const { return m_Utility < rhs.m_Utility; }

	protected:
		float m_Utility;
	};

	static bool BehaviourComp(BaseBehaviour*& lhs, BaseBehaviour*& rhs) { return lhs->operator<(*rhs); }
}

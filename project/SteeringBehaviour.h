#pragma once
#include "BaseBehaviour.h"


namespace Utility
{
	class FleeBehaviour final : public BaseBehaviour
	{
	public:
		FleeBehaviour();
		~FleeBehaviour() override;

		void CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots) override;
		SteeringPlugin_Output Execute(IExamInterface* pInterface) override;
		bool IsFinished(const IExamInterface* pInterface, const float dt) override;
		bool IsValid(IExamInterface* pInterface, const float dt) override;

	private:
		float m_TimeFleeing{ 0.f };
		Elite::Vector2 m_DesiredLinearVelocity{};
		bool m_Run{};

		void GetZombieLocationsInFov(IExamInterface* pInterface, std::vector<Elite::Vector2>& locations);
		void SetDesiredLinearVelocity(const AgentInfo& agent, const std::vector<Elite::Vector2>& targets);
	};

	class WanderBehaviour final : public BaseBehaviour
	{
	public:
		WanderBehaviour();
		~WanderBehaviour() override;

		void CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots) override;
		SteeringPlugin_Output Execute(IExamInterface* pInterface) override;
		bool IsFinished(const IExamInterface* pInterface, const float dt) override;
		bool IsValid(IExamInterface* pInterface, const float dt) override { return true; }

	private:
		float m_TargetOffset = 10.f;
		float m_AngleRadians = 0.f;
		float m_Radius = 5.f;
		float m_MaxAngleChange = Elite::ToRadians(55);
		Elite::Vector2 m_PrevDirection{};
	};

}

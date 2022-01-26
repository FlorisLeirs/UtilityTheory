#pragma once
#include "BaseBehaviour.h"

class Inventory;

namespace Utility
{
	class Heal final : public BaseBehaviour
	{
	public:
		Heal(Inventory* pInventory);
		~Heal() override = default;

		void CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots) override;
		SteeringPlugin_Output Execute(IExamInterface* pInterface) override;
		bool IsFinished(const IExamInterface* pInterface, const float dt) override;
		bool IsValid(IExamInterface* pInterface, const float dt) override;

	private:
		Inventory* m_pInventory;
	};

	class Eat final : public BaseBehaviour
	{
	public:
		Eat(Inventory* pInventory);
		~Eat() override = default;

		void CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots) override;
		SteeringPlugin_Output Execute(IExamInterface* pInterface) override;
		bool IsFinished(const IExamInterface* pInterface, const float dt) override;
		bool IsValid(IExamInterface* pInterface, const float dt) override;

	private:
		Inventory* m_pInventory;
	};

	class Shoot final : public BaseBehaviour
	{
	public:
		Shoot(Inventory* pInventory);
		~Shoot() override = default;

		void CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots) override;
		SteeringPlugin_Output Execute(IExamInterface* pInterface) override;
		bool IsFinished(const IExamInterface* pInterface, const float dt) override;
		bool IsValid(IExamInterface* pInterface, const float dt) override;

	private:
		Inventory* m_pInventory;
		bool m_Done{ false };

		void GetZombieLocationsInFov(IExamInterface* pInterface, std::vector<EnemyInfo>& zombies);
		bool IsInLineOfFire(const EnemyInfo& zombie, const AgentInfo& agentInfo);
	};
}

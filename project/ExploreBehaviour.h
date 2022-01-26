#pragma once
#include "BaseBehaviour.h"


class Inventory;

namespace Utility
{
	class MoveToHouseBehaviour final : public Utility::BaseBehaviour
	{
	public:
		MoveToHouseBehaviour();
		~MoveToHouseBehaviour() override = default;

		void CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots) override;
		SteeringPlugin_Output Execute(IExamInterface* pInterface) override;
		bool IsFinished(const IExamInterface* pInterface, const float dt) override;
		bool IsValid(IExamInterface* pInterface, const float dt) override;

	private:
		Elite::Mat22 m_HouseCenter{};
		float m_ElapsedTime{ 0.f };
	};

	class PickUpItem final : public BaseBehaviour
	{
	public:
		PickUpItem(Inventory* pInventory);
		~PickUpItem() override = default;

		void CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots) override;
		SteeringPlugin_Output Execute(IExamInterface* pInterface) override;
		bool IsFinished(const IExamInterface* pInterface, const float dt) override;
		bool IsValid(IExamInterface* pInterface, const float dt) override;

	private:
		Elite::Vector2 m_ItemPos{};
		Inventory* m_pInventory;
		bool m_Done{false};
	};

	class GetOutOfHouse final : public BaseBehaviour
	{
	public:
		GetOutOfHouse();
		~GetOutOfHouse() override = default;
		void CalculateUtility(IExamInterface* pInterface, const UINT emptyInventorySlots) override;
		SteeringPlugin_Output Execute(IExamInterface* pInterface) override;
		bool IsFinished(const IExamInterface* pInterface, const float dt) override;
		bool IsValid(IExamInterface* pInterface, const float dt) override;
	private:
		float m_TimeInHouse{};
	};
}


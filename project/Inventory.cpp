#include "stdafx.h"
#include "Inventory.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

Inventory::Inventory(UINT capacity)
	: m_SlotAvailable()
{
	m_SlotAvailable.resize(capacity, true);
}

bool Inventory::AddPistol(ItemInfo& pistol, IExamInterface* pInterface)
{
	UINT idx = GetEmptySlot();
	if (idx == m_SlotAvailable.size())
		return false;

	m_Pistols.push(idx);
	m_SlotAvailable[idx] = false;
	return pInterface->Inventory_AddItem(idx, pistol);
}

bool Inventory::AddFood(ItemInfo& food, IExamInterface* pInterface)
{
	UINT idx = GetEmptySlot();
	if (idx == m_SlotAvailable.size())
		return false;

	m_Food.push(idx);
	m_SlotAvailable[idx] = false;
	return pInterface->Inventory_AddItem(idx, food);
}

bool Inventory::AddMedKit(ItemInfo& medKit, IExamInterface* pInterface)
{
	UINT idx = GetEmptySlot();
	if (idx == m_SlotAvailable.size())
		return false;

	m_MedKits.push(idx);
	m_SlotAvailable[idx] = false;
	return pInterface->Inventory_AddItem(idx, medKit);
}

bool Inventory::UsePistol(IExamInterface* pInterface)
{
	if (m_Pistols.empty())
		return false;

	ItemInfo pistol{};
	pInterface->Inventory_GetItem(m_Pistols.front(), pistol);
	pInterface->Inventory_UseItem(m_Pistols.front());

	if (pInterface->Weapon_GetAmmo(pistol) == 0)
	{
		pInterface->Inventory_RemoveItem(m_Pistols.front());
		m_SlotAvailable[m_Pistols.front()] = true;
		m_Pistols.pop();
	}
	return true;
}

bool Inventory::UseFood(IExamInterface* pInterface)
{
	if (m_Food.empty())
		return false;
	m_SlotAvailable[m_Food.front()] = true;
	pInterface->Inventory_UseItem(m_Food.front());
	pInterface->Inventory_RemoveItem(m_Food.front());
	m_Food.pop();
	return true;
}

bool Inventory::UseMedKit(IExamInterface* pInterface)
{
	if (m_MedKits.empty())
		return false;
	m_SlotAvailable[m_MedKits.front()] = true;
	pInterface->Inventory_UseItem(m_MedKits.front());
	pInterface->Inventory_RemoveItem(m_MedKits.front());
	m_MedKits.pop();
	return true;
}

UINT Inventory::GetAmountOfEmptySlots() const
{
	UINT counter{0};
	for (UINT i{}; i != m_SlotAvailable.size(); ++i)
	{
		if (m_SlotAvailable[i])
			++counter;
	}
	return counter;
}

UINT Inventory::GetEmptySlot() const
{
	for (UINT i{}; i != m_SlotAvailable.size(); ++i)
	{
		if (m_SlotAvailable[i])
			return i;
	}
	return m_SlotAvailable.size();
}

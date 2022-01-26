#pragma once

struct ItemInfo;
class IExamInterface;

class Inventory
{
public:
	Inventory(UINT capacity);

	bool AddPistol(ItemInfo& pistol, IExamInterface* pInterface);
	bool AddFood(ItemInfo& food, IExamInterface* pInterface);
	bool AddMedKit(ItemInfo& medKit, IExamInterface* pInterface);

	bool UsePistol(IExamInterface* pInterface);
	bool UseFood(IExamInterface* pInterface);
	bool UseMedKit(IExamInterface* pInterface);

	UINT GetAmountOfEmptySlots()  const;
	UINT GetAmountOfMedKits() const {return m_MedKits.size();}
	UINT GetAmountOfFood() const {return m_Food.size();}
	UINT GetAmountOfPistols() const {return m_Pistols.size();}

private:
	std::vector<bool> m_SlotAvailable; // true = empty
	std::queue<UINT> m_Pistols{};
	std::queue<UINT> m_Food{};
	std::queue<UINT> m_MedKits{};

	UINT GetEmptySlot() const;
};

#include "pch.h"
#include "DataManager.hpp"

#include <filesystem>

#include "magic_enum/magic_enum.hpp"

DataManager::DataManager()
{
}

void DataManager::Initialize()
{
	try
	{
		m_datasheet = std::make_unique<SQLite::Database>("Common/Database/datasheet.db");
		
		// Load monster data
		{
			SQLite::Statement stmt(*m_datasheet, "SELECT id, ptype, hp, power, attackRange FROM monster");
			while (stmt.executeStep())
			{
				auto type = *magic_enum::enum_cast<mmo::EObjectType>(stmt.getColumn(1).getString());
				MonsterData data(
					stmt.getColumn(0).getInt(),
					type,
					stmt.getColumn(2).getInt(),
					stmt.getColumn(3).getInt(),
					stmt.getColumn(4).getDouble()
				);
				m_monsterData[type] = data;
			}
		}
		// Load item data
		{
			SQLite::Statement stmt(*m_datasheet, "SELECT id, ptype FROM item");
			while (stmt.executeStep())
			{
				auto type = *magic_enum::enum_cast<mmo::EItemType>(stmt.getColumn(1).getString());
				ItemData data(
					stmt.getColumn(0).getUInt(),
					type
				);
				m_itemData[type] = data;
			}
		}
		// Load drop data
		{
			SQLite::Statement stmt(*m_datasheet,
				"SELECT m.ptype, i.ptype "	\
				"FROM drops d "				\
				"INNER JOIN monster m ON "	\
				"d.monsterId = m.id "		\
				"INNER JOIN item i ON "		\
				"d.itemId = i.id"
			);
			while (stmt.executeStep())
			{
				auto monsterType = *magic_enum::enum_cast<mmo::EObjectType>(stmt.getColumn(0).getString());
				auto itemType = *magic_enum::enum_cast<mmo::EItemType>(stmt.getColumn(1).getString());
				m_dropData[monsterType].push_back(m_itemData[itemType]);
			}
		}
		// Load skill data
		{
			SQLite::Statement stmt(*m_datasheet, "SELECT id, ptype, damage, range_x, range_y FROM skill");
			while (stmt.executeStep())
			{
				auto type = *magic_enum::enum_cast<mmo::ESkillType>(stmt.getColumn(1).getString());
				SkillData data(
					stmt.getColumn(0).getUInt(),
					type,
					stmt.getColumn(2).getUInt(),
					Vector2DF(
						stmt.getColumn(3).getDouble(),
						stmt.getColumn(4).getDouble()
					)
				);
				m_skillData[type] = data;
			}
		}
		Console::Log(Category::MMOServer, TEXT("'DataManager' initialized"));
	}
	catch (std::exception&)
	{
		Console::Error(Category::MMOServer, TEXT("Can't load datasheet properly. please check datasheet."));
	}

}

const MonsterData& DataManager::GetMonsterData(gen::mmo::EObjectType type)
{
	return m_monsterData[type];
}

const List<ItemData>& DataManager::GetDropsData(gen::mmo::EObjectType type)
{
	return m_dropData[type];
}

const SkillData& DataManager::GetSkillData(gen::mmo::ESkillType type)
{
	return m_skillData[type];
}

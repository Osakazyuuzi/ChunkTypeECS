#include "World.h"

#include "EntityManager.h"
#include "SystemBase.h"

#include "EntityManager.h"


namespace ECS
{
	World::World()
	{
		m_pEntityManager = std::make_shared<EntityManager>(this);
	}

	void World::Init()
	{

	}

	/**
	* @brief 更新処理を行います。
	* @param _deltaTime 前フレームからの経過時間。
	*/
	void World::Update(float _deltaTime)
	{
		for (auto systems : m_SystemList)
		{
			for (auto&& system : systems)
			{
				system->Update(_deltaTime);
			}
		}
	}
}
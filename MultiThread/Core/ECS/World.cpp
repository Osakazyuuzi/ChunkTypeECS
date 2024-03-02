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
	* @brief �X�V�������s���܂��B
	* @param _deltaTime �O�t���[������̌o�ߎ��ԁB
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
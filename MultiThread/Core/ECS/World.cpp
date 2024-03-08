#include "World.h"

#include "EntityManager.h"
#include "SystemBase.h"

#include "EntityManager.h"

#include "../../AsyncFunctionManager.h"
#include <iostream>

namespace ECS
{
	World::World()
	{
		m_pEntityManager = std::make_shared<EntityManager>(this);
		m_pAsyncFunctionManager = std::make_shared<AsyncFunctionManager>();
	}

	World::~World()
	{
		m_pAsyncFunctionManager->Shutdown();
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
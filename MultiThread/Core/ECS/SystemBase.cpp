#include "SystemBase.h"

#include "World.h"
#include "EntityManager.h"
#include "Chunk.h"


namespace ECS
{
	std::shared_ptr<EntityManager> SystemBase::GetEntityManager()
	{
		return m_pWorld->GetEntityManager();
	}
}

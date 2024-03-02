#pragma once

#include "Common/Id.h"
#include "IComponentData.h"
#include "Archetype.h"
#include "World.h"

namespace ECS
{
	/**
	* @class EntityManager
	* @brief �G���e�B�e�B�̍쐬�A�j���Ȃǂ̊Ǘ��A
	*		 �R���|�[�l���g�̒ǉ��A�폜�A�ݒ�Ȃǂ̑�����s���B
	*/
	class EntityManager
	{
		using ChunkIndex = std::uint32_t;;
		using ChunkInIndex = std::uint32_t;
		using EntityInfo = std::pair<ChunkIndex, ChunkInIndex>;

	public:
		/**
		* @brief �R���X�g���N�^
		* @param _pWorld �������郏�[���h�̃|�C���^�B
		*/
		EntityManager(World* _pWorld)
			: m_pWorld(_pWorld)
		{}

		/**
		* @brief �V�����G���e�B�e�B���쐬���܂��B
		* @return �쐬���ꂽ�G���e�B�e�B�̃C���X�^���X�B
		*/
		inline Entity CreateEntity()
		{
			Archetype archetype;
			return CreateEntity(archetype);
		}

		/**
		* @brief �w�肳�ꂽ�A�[�L�^�C�v�̊�Â��ĐV�����G���e�B�e�B���쐬���܂��B
		* @param _archetype �V�����G���e�B�e�B�Ɏg�p����A�[�L�^�C�v�B
		* @return �쐬���ꂽ�V�����G���e�B�e�B�B
		*/
		inline Entity CreateEntity(const Archetype& _archetype)
		{
			auto entityInfo = m_vRecycleEntityIndices.size() == 0 ?
				CreateNewEntity() : CreateRecycleEntity();
			const std::uint32_t chunkIndex = GetAndCreateChunkIndex(_archetype, true);
			std::uint32_t chunkInIndex = m_pWorld->m_ChunkList[chunkIndex].
				CreateEntity(entityInfo.first, entityInfo.second);
			m_vEntities[entityInfo.first].first =
				EntityInfo(chunkIndex, chunkInIndex);
			return m_vEntities[entityInfo.first].second;
		}

		/**
		* @brief �w�肳�ꂽ�G���e�B�e�B��j�����܂��B
		* @param _entity �j������G���e�B�e�B�B
		*/
		inline void DestroyEntity(const Entity& _entity)
		{
			if (!ExistEntity(_entity)) return;

			const std::uint32_t entityIndex = GetIndex(_entity.m_Identifier);
			EntityInfo& entityInfo = m_vEntities[entityIndex].first;

			m_pWorld->m_ChunkList[entityInfo.first].DestroyEntity(entityInfo.second);

			SetVersion(
				m_vEntities[entityIndex].second.m_Identifier,
				GetVersion(m_vEntities[entityIndex].second.m_Identifier)
			);

			m_vRecycleEntityIndices.push_back(entityIndex);
		}

		/**
		* @brief �w�肳�ꂽ�G���e�B�e�B�����݂��邩�ǂ����𔻒肵�܂��B
		* @param _entity ���݂��m�F����G���e�B�e�B�B
		* @return �G���e�B�e�B�����݂���ꍇ��true, �����łȂ��ꍇ��false�B
		*/
		inline const bool ExistEntity(const Entity& _entity)
		{
			const std::uint32_t entityIndex = GetIndex(_entity.m_Identifier);
			return _entity.m_Identifier
				== m_vEntities[entityIndex].second.m_Identifier;
		}

		/**
		* @brief �w�肳�ꂽ�G���e�B�e�B�ɃR���|�[�l���g��ǉ����܂��B
		* @param _entity �R���|�[�l���g��ǉ�����G���e�B�e�B�B
		*/
		template <typename CompT>
		inline void AddComponent(Entity& _entity)
		{
			if (!ExistEntity(_entity)) return;

			const std::uint32_t entityIndex = GetIndex(_entity.m_Identifier);
			EntityInfo entityInfo = m_vEntities[entityIndex].first;
			auto newArchetype =
				m_pWorld->m_ChunkList[entityInfo.first].GetArchetype();
			newArchetype.AddType<CompT>();

			const std::uint32_t newChunkIndex =
				GetAndCreateChunkIndex(newArchetype);
			Chunk& chunk = m_pWorld->m_ChunkList[newChunkIndex];

			std::size_t chunkInIndex = entityInfo.second;
			m_pWorld->m_ChunkList[entityInfo.first].MoveEntity(
				chunkInIndex, _entity, chunk
			);
			m_vEntities[entityIndex].first = std::pair<ChunkIndex, ChunkInIndex>(newChunkIndex, chunkInIndex);
		}

		/**
		* @brief �w�肳�ꂽ�G���e�B�e�B����R���|�[�l���g�����O���܂��B
		* @oaram _entity �R���|�[�l���g�����O����G���e�B�e�B�B
		*/
		template <typename CompT>
		inline void RemoveComponent(Entity& _entity)
		{
			if (!ExistEntity(_entity)) return;

			const std::uint32_t entityIndex = GetIndex(_entity.m_Identifier);
			EntityInfo& entityInfo = m_vEntities[entityIndex].first;
			Archetype newArchetype =
				m_pWorld->m_ChunkList[entityInfo.first].GetArchetype();
			newArchetype.RemoveType<CompT>();

			const std::uint32_t newChunkIndex =
				GetAndCreateChunkIndex(newArchetype);
			Chunk& chunk = m_pWorld->m_ChunkList[newChunkIndex];

			m_pWorld->m_ChunkList[entityInfo.first].MoveEntity(
				entityInfo.second, _entity, chunk
			);
			entityInfo.first = newChunkIndex;
		}

		/**
		* @brief �w�肳�ꂽ�G���e�B�e�B�̃R���|�[�l���g�f�[�^��ݒ肵�܂��B
		* @param _entity �R���|�[�l���g�f�[�^��ݒ肷��G���e�B�e�B�B
		* @param _data �ݒ肷��R���|�[�l���g�f�[�^�B
		*/
		template <typename CompT>
		inline void SetComponent(const Entity& _entity, const CompT& _data)
		{
			if (!ExistEntity(_entity)) return;

			const std::uint32_t entityIndex = GetIndex(_entity.m_Identifier);
			EntityInfo& entityInfo = m_vEntities[entityIndex].first;

			m_pWorld->m_ChunkList[entityInfo.first].SetComponentData(
				entityInfo.second, _data
			);
		}

		/**
		* @brief �w�肳�ꂽ�G���e�B�e�B�̃R���|�[�l���g�f�[�^���擾���܂��B
		* @param _entity �R���|�[�l���g�f�[�^���擾����G���e�B�e�B�B
		* @return �w�肳�ꂽ�R���|�[�l���g�̃|�C���^�B�G���e�B�e�B�����݂��Ȃ��ꍇ��nullptr�B
		*/
		template <typename CompT>
		inline CompT* GetComponent(const Entity& _entity)
		{
			if (!ExistEntity(_entity)) return nullptr;

			const std::uint32_t entityIndex = GetIndex(_entity.m_Identifier);
			EntityInfo& entityInfo = m_vEntities[entityIndex].first;
			Chunk* chunk = &m_pWorld->m_ChunkList[entityInfo.first];
			return &chunk->GetComponentList<CompT>()[entityInfo.second];
		}

		/**
		* @brief �w�肳�ꂽ�A�[�L�^�C�v���܂ރ`�����N�̃��X�g���擾���܂��B
		* @param _archetype �擾����A�[�L�^�C�v�B
		* @return �A�[�L�^�C�v���܂ރ`�����N�̃��X�g�B
		*/
		inline std::vector<Chunk*> GetContainChunkList(const Archetype& _archetype) const
		{
			std::vector<Chunk*> result;
			result.reserve(8);
			for (auto&& chunk : m_pWorld->m_ChunkList)
			{
				if (chunk.GetArchetype().IsContain(_archetype))
				{
					result.push_back(&chunk);
				}
			}
			return result;
		}

		/**
		* @brief �w�肳�ꂽ�A�[�L�^�C�v�ɑΉ�����`�����N�̃C���f�b�N�X���擾�܂��͍쐬���܂��B
		* @param _archetype �Ώۂ̃A�[�L�^�C�v�B
		* @param _maxCheck �`�����N�̍ő�T�C�Y���`�F�b�N���邩�ǂ����B
		* @return �Ή�����`�����N�̃C���f�b�N�X�B
		*/
		inline const std::uint32_t GetAndCreateChunkIndex(const Archetype& _archetype, bool _maxCheck = false) const
		{
			std::uint32_t chunkIndex = 0;
			for (auto&& chunk : m_pWorld->m_ChunkList)
			{
				if (chunk.GetArchetype().GetSignature()
					== _archetype.GetSignature())
					if (!_maxCheck || chunk.IsMax())
						return chunkIndex;
				++chunkIndex;
			}

			m_pWorld->m_ChunkList.push_back(Chunk(_archetype));
			return chunkIndex;
		}

		/**
		* @brief �G���e�B�e�B�z����擾���܂��B
		* @return std::vector<std::pair<EntityInfo, Entity>>> �G���e�B�e�B�z��B
		*/
		std::vector<std::pair<EntityInfo, Entity>>& GetEntities()
		{
			return m_vEntities;
		}

		/**
		* @brief �w�肳�ꂽ�G���e�B�e�B�̃A�[�L�^�C�v���擾���܂��B
		* @param _entity �A�[�L�^�C�v���擾����G���e�B�e�B�B
		* @return Archetype �w��̃G���e�B�e�B�̃A�[�L�^�C�v�B
		*/
		Archetype GetArchetype(const Entity& _entity)
		{
			const std::uint32_t entityIndex = GetIndex(_entity.m_Identifier);
			return m_pWorld->m_ChunkList[m_vEntities[entityIndex].first.first].GetArchetype();
		}

	private:
		/**
		* @brief �V�����G���e�B�e�B���쐬���܂��B
		* @return �쐬���ꂽ�G���e�B�e�B�̃C���f�b�N�X�ƃo�[�W�����̃y�A�B
		*/
		std::pair<std::uint32_t, std::uint32_t> CreateNewEntity()
		{
			if (m_vRecycleEntityIndices.size() != 0)
				std::abort();

			m_vEntities.push_back(
				std::pair<EntityInfo, Entity>(
					std::pair<ChunkIndex, ChunkInIndex>(0, 0),
					Entity(m_vEntities.size(), 0))
			);
			return std::pair<std::uint32_t, std::uint32_t>(
				m_vEntities.size() - 1, 0
			);
		}

		/**
		* @brief �ė��p�\�ȃG���e�B�e�B���쐬���܂��B
		* @return �ė��p���ꂽ�G���e�B�e�B�̃C���f�b�N�X�ƐV�����o�[�W�����̃y�A�B
		*/
		std::pair<std::uint32_t, std::uint32_t> CreateRecycleEntity()
		{
			if (m_vRecycleEntityIndices.size() == 0)
				std::abort();
			std::uint32_t index = m_vRecycleEntityIndices[0];
			m_vRecycleEntityIndices.erase(m_vRecycleEntityIndices.begin());
			std::uint32_t version = GetVersion(m_vEntities[index].second.m_Identifier);
			version++;
			SetVersion(m_vEntities[index].second.m_Identifier, version);
			return std::pair<std::uint32_t, std::uint32_t>(index, version);
		}

	private:
		//! �G���e�B�e�B�Ƃ��̏���ێ�����z��B
		std::vector<std::pair<EntityInfo, Entity>> m_vEntities;
		//! �ė��p�\�ȃG���e�B�e�B�C���f�b�N�X�̔z��B
		std::vector<std::uint32_t> m_vRecycleEntityIndices;
		//! �����郏�[���h�ւ̃|�C���^�B
		World* m_pWorld = nullptr;
	};
}
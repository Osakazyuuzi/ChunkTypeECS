#pragma once

#include "Common/Id.h"
#include "IComponentData.h"
#include "Archetype.h"
#include "World.h"

namespace ECS
{
	/**
	* @class EntityManager
	* @brief エンティティの作成、破棄などの管理、
	*		 コンポーネントの追加、削除、設定などの操作を行う。
	*/
	class EntityManager
	{
		using ChunkIndex = std::uint32_t;;
		using ChunkInIndex = std::uint32_t;
		using EntityInfo = std::pair<ChunkIndex, ChunkInIndex>;

	public:
		/**
		* @brief コンストラクタ
		* @param _pWorld 所属するワールドのポインタ。
		*/
		EntityManager(World* _pWorld)
			: m_pWorld(_pWorld)
		{}

		/**
		* @brief 新しいエンティティを作成します。
		* @return 作成されたエンティティのインスタンス。
		*/
		inline Entity CreateEntity()
		{
			Archetype archetype;
			return CreateEntity(archetype);
		}

		/**
		* @brief 指定されたアーキタイプの基づいて新しいエンティティを作成します。
		* @param _archetype 新しいエンティティに使用するアーキタイプ。
		* @return 作成された新しいエンティティ。
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
		* @brief 指定されたエンティティを破棄します。
		* @param _entity 破棄するエンティティ。
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
		* @brief 指定されたエンティティが存在するかどうかを判定します。
		* @param _entity 存在を確認するエンティティ。
		* @return エンティティが存在する場合はtrue, そうでない場合はfalse。
		*/
		inline const bool ExistEntity(const Entity& _entity)
		{
			const std::uint32_t entityIndex = GetIndex(_entity.m_Identifier);
			return _entity.m_Identifier
				== m_vEntities[entityIndex].second.m_Identifier;
		}

		/**
		* @brief 指定されたエンティティにコンポーネントを追加します。
		* @param _entity コンポーネントを追加するエンティティ。
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
		* @brief 指定されたエンティティからコンポーネントを除外します。
		* @oaram _entity コンポーネントを除外するエンティティ。
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
		* @brief 指定されたエンティティのコンポーネントデータを設定します。
		* @param _entity コンポーネントデータを設定するエンティティ。
		* @param _data 設定するコンポーネントデータ。
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
		* @brief 指定されたエンティティのコンポーネントデータを取得します。
		* @param _entity コンポーネントデータを取得するエンティティ。
		* @return 指定されたコンポーネントのポインタ。エンティティが存在しない場合はnullptr。
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
		* @brief 指定されたアーキタイプを含むチャンクのリストを取得します。
		* @param _archetype 取得するアーキタイプ。
		* @return アーキタイプを含むチャンクのリスト。
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
		* @brief 指定されたアーキタイプに対応するチャンクのインデックスを取得または作成します。
		* @param _archetype 対象のアーキタイプ。
		* @param _maxCheck チャンクの最大サイズをチェックするかどうか。
		* @return 対応するチャンクのインデックス。
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
		* @brief エンティティ配列を取得します。
		* @return std::vector<std::pair<EntityInfo, Entity>>> エンティティ配列。
		*/
		std::vector<std::pair<EntityInfo, Entity>>& GetEntities()
		{
			return m_vEntities;
		}

		/**
		* @brief 指定されたエンティティのアーキタイプを取得します。
		* @param _entity アーキタイプを取得するエンティティ。
		* @return Archetype 指定のエンティティのアーキタイプ。
		*/
		Archetype GetArchetype(const Entity& _entity)
		{
			const std::uint32_t entityIndex = GetIndex(_entity.m_Identifier);
			return m_pWorld->m_ChunkList[m_vEntities[entityIndex].first.first].GetArchetype();
		}

	private:
		/**
		* @brief 新しいエンティティを作成します。
		* @return 作成されたエンティティのインデックスとバージョンのペア。
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
		* @brief 再利用可能なエンティティを作成します。
		* @return 再利用されたエンティティのインデックスと新しいバージョンのペア。
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
		//! エンティティとその情報を保持する配列。
		std::vector<std::pair<EntityInfo, Entity>> m_vEntities;
		//! 再利用可能なエンティティインデックスの配列。
		std::vector<std::uint32_t> m_vRecycleEntityIndices;
		//! 属するワールドへのポインタ。
		World* m_pWorld = nullptr;
	};
}
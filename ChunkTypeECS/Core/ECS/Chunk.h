#pragma once

#include <memory>
#include <cstdint>
#include "IComponentData.h"
#include "Entity.h"
#include "Archetype.h"
#include "ComponentArray.h"


namespace ECS
{
	/**
	 * @class Chunk
	 * @brief ECSにおけるエンティティとそのコンポーネントを格納するチャンクを表すクラス。
	 */
	class Chunk
	{
	public:
		/**
		 * @brief コンストラクタ。
		 * @param _archetype このチャンクに関連付けられるアーキタイプ。
		 */
		explicit Chunk(const Archetype& _archetype)
			: m_Size(0)
		{
			m_Archetype = _archetype;
			m_pBegin = std::make_shared<std::byte[]>(mc_Capacity);
			m_MaxSize = mc_Capacity /
				(sizeof Entity + m_Archetype.GetArchetypeMemorySize());
		}

		Chunk(const Chunk& _other)
		{
			m_Archetype = _other.m_Archetype;
			m_MaxSize = _other.m_MaxSize;
			m_Size = _other.m_Size;
			m_pBegin = _other.m_pBegin;
		}

		/**
		 * @brief 新しいエンティティを作成し、チャンクに追加します。
		 * @param _chunkIndex エンティティのインデックス。
		 * @param _version エンティティのバージョン。
		 * @return 作成されたチャンク内インデックス。
		 */
		std::uint32_t CreateEntity(const EntityIndex& _index, const EntityVersion& _version)
		{
			if (m_Size == m_MaxSize)
				std::abort();

			const std::size_t indexOffset =
				sizeof(Entity) * m_Size;
			new (m_pBegin.get() + indexOffset) Entity(_index, _version);

			m_Size++;
			return m_Size - 1;
		}

		/**
		 * @brief 指定されたインデックスのエンティティを破棄します。
		 * @param _chunkIndex 破棄するエンティティのチャンク内インデックス。
		 */
		void DestroyEntity(const std::size_t& _chunkIndex)
		{
			if (m_Size == 0)
				std::abort();

			std::size_t sourceIndexOffset =
				sizeof(Entity) * m_Size;
			std::size_t destinationIndexOffset =
				sizeof(Entity) * _chunkIndex;

			void* sourceAddress = static_cast<void*>
				(m_pBegin.get() + sourceIndexOffset);
			void* destinationAddress = static_cast<void*>
				(m_pBegin.get() + destinationIndexOffset);

			memcpy(destinationAddress, sourceAddress, sizeof(Entity));

			const std::size_t archetypeSize =
				m_Archetype.GetArchetypeSize();
			for (std::size_t i = 0; i < archetypeSize; i++)
			{
				const std::size_t componentOffset =
					(sizeof(Entity) + m_Archetype.GetMemroyOffsetByIndex(i)) * m_MaxSize;
				sourceIndexOffset =
					m_Archetype.GetMemorySizeByIndex(i) * m_Size;
				destinationIndexOffset =
					m_Archetype.GetMemorySizeByIndex(i) * _chunkIndex;

				sourceAddress = static_cast<void*>
					(m_pBegin.get() + componentOffset + sourceIndexOffset);
				destinationAddress = static_cast<void*>
					(m_pBegin.get() + componentOffset + destinationIndexOffset);

				memcpy(destinationAddress, sourceAddress,
					m_Archetype.GetMemorySizeByIndex(i));
			}

			m_Size--;
		}

		/**
		 * @brief エンティティを現在のチャンクから別のチャンクに移動します。
		 * @param _chunkIndex 移動するエンティティのチャンク内インデックス。
		 * @param _entity 移動するエンティティ。
		 * @param _other 移動先のチャンク。
		 */
		void MoveEntity(std::size_t& _chunkIndex, Entity& _entity, Chunk& _other)
		{
			if (_chunkIndex >= m_Size)
				std::abort();

			if (m_Size == m_MaxSize && (m_MaxSize != 0))
				std::abort();

			EntityIndex oldChunkIndex = _chunkIndex;
			EntityIndex newChunkIndex = _other.m_Size++;

			std::size_t oldArchetypeSize = m_Archetype.GetArchetypeSize();
			std::size_t newArchetypeSize = _other.m_Archetype.GetArchetypeSize();
			std::size_t oldCompIndex = 0;
			std::size_t newCompIndex = 0;
			std::size_t moveCompCount = 0;

			std::size_t sourceIndexOffset =
				sizeof(Entity) * oldChunkIndex;
			std::size_t destinationIndexOffset =
				sizeof(Entity) * newChunkIndex;

			void* sourceAddress = static_cast<void*>
				(m_pBegin.get() + sourceIndexOffset);
			void* destinationAddress = static_cast<void*>
				(_other.m_pBegin.get() + destinationIndexOffset);

			memcpy(destinationAddress, sourceAddress, sizeof(Entity));


			for (newCompIndex = 0; newCompIndex < newArchetypeSize;)
			{
				std::size_t oldCompId = m_Archetype.GetComponentIdByIndex(oldCompIndex);
				std::size_t newCompId = _other.m_Archetype.GetComponentIdByIndex(newCompIndex);

				while (1)
				{
					if (oldCompId == newCompId)
					{
						const std::size_t sourceOffset =
							(sizeof(Entity) + m_Archetype.GetMemroyOffsetByIndex(oldCompIndex)) * m_MaxSize;
						const std::size_t destinationOffset =
							(sizeof(Entity) + _other.m_Archetype.GetMemroyOffsetByIndex(newCompIndex)) * _other.m_MaxSize;
						sourceIndexOffset =
							m_Archetype.GetMemorySizeByIndex(oldCompIndex) * oldChunkIndex;
						destinationIndexOffset =
							_other.m_Archetype.GetMemorySizeByIndex(newCompIndex) * newChunkIndex;

						sourceAddress = static_cast<void*>
							(m_pBegin.get() + sourceOffset + sourceIndexOffset);
						destinationAddress = static_cast<void*>
							(_other.m_pBegin.get() + destinationOffset + destinationIndexOffset);

						memcpy(destinationAddress, sourceAddress,
							m_Archetype.GetMemorySizeByIndex(oldCompIndex));


						moveCompCount++;
						newCompIndex++;
						break;
					}
					else if (oldCompId < newCompId)
					{
						oldCompIndex++;
						break;
					}
					else
					{
						newCompIndex++;
						break;
					}
				}

				if (moveCompCount == oldArchetypeSize) break;
			}

			_chunkIndex = newChunkIndex;

			sourceIndexOffset =
				sizeof(Entity) * m_Size;
			destinationIndexOffset =
				sizeof(Entity) * oldChunkIndex;

			sourceAddress = static_cast<void*>
				(m_pBegin.get() + sourceIndexOffset);
			destinationAddress = static_cast<void*>
				(m_pBegin.get() + destinationIndexOffset);

			memcpy(destinationAddress, sourceAddress, sizeof(Entity));

			const std::size_t archetypeSize =
				m_Archetype.GetArchetypeSize();
			for (std::size_t i = 0; i < archetypeSize; i++)
			{
				const std::size_t componentOffset =
					(sizeof(Entity) + m_Archetype.GetMemroyOffsetByIndex(i)) * m_MaxSize;
				sourceIndexOffset =
					m_Archetype.GetMemorySizeByIndex(i) * m_Size;
				destinationIndexOffset =
					m_Archetype.GetMemorySizeByIndex(i) * oldChunkIndex;

				sourceAddress = static_cast<void*>
					(m_pBegin.get() + componentOffset + sourceIndexOffset);
				destinationAddress = static_cast<void*>
					(m_pBegin.get() + componentOffset + destinationIndexOffset);

				memcpy(destinationAddress, sourceAddress,
					m_Archetype.GetMemorySizeByIndex(i));
			}

			m_Size--;
		}

		/**
		 * @brief 指定されたインデックスのエンティティにコンポーネントデータを設定します。
		 * @param _chunkIndex コンポーネントデータを設定するエンティティのチャンク内インデックス。
		 * @param _data 設定するコンポーネントデータ。
		 */
		template <typename CompT>
		void SetComponentData(const std::size_t& _chunkIndex, const CompT& _data)
		{
			if (_chunkIndex >= m_Size)
				std::abort();

			const std::size_t componentOffset =
				(sizeof(Entity) + m_Archetype.GetMemoryOffset<CompT>()) * m_MaxSize;
			const std::size_t indexOffset =
				sizeof(CompT) * _chunkIndex;
			std::memcpy(m_pBegin.get() + componentOffset
				+ indexOffset, &_data, sizeof(CompT));
		}

		/**
		 * @brief チャンクが最大サイズに達しているかどうかを判断します。
		 * @return bool チャンクが最大サイズに達している場合はtrue。
		 */
		const bool IsMax() const noexcept
		{
			return m_Size == m_MaxSize;
		}

		/**
		 * @brief アーキタイプを取得します。
		 * @return Archetype& 関連付けられたアーキタイプ。
		 */
		Archetype& GetArchetype()
		{
			return m_Archetype;
		}

		/**
		* @brief 指定のコンポーネントリストを取得します。
		* @return ComponentArray<CompT> 指定の型のコンポーネントリスト。
		*/
		template <typename CompT>
		ComponentArray<CompT> GetComponentList()
		{
			Archetype arche;
			arche.AddType<CompT>();
			if (!m_Archetype.IsContain(arche))
				std::abort();

			using TType = std::remove_const_t<std::remove_reference_t<CompT>>;

			auto offset =
				(sizeof(Entity) + m_Archetype.GetMemoryOffset<CompT>()) * m_MaxSize;

			return ComponentArray<CompT>(reinterpret_cast<TType*>(m_pBegin.get() + (std::size_t)offset), m_Size);
		}

		/**
		 * @brief チャンク内のエンティティ数を取得します。
		 * @return std::uint32_t チャンク内のエンティティ数。
		 */
		const std::uint32_t GetSize() const noexcept
		{
			return m_Size;
		}

	private:
		/**
		 * @brief チャンクが空かどうかを判断します。
		 * @return bool チャンクが空の場合はtrue。
		 */
		const bool IsEmpty() const noexcept
		{
			return m_Size == 0;
		}

	private:
		//! このチャンクに関連付けられるアーキタイプ。
		Archetype m_Archetype;
		//! チャンクのデータを格納するポインタ。
		std::shared_ptr<std::byte[]> m_pBegin = nullptr;
		//! チャンク内のエンティティ数。
		std::uint32_t m_Size;
		//! チャンクの最大サイズ。
		std::uint32_t m_MaxSize;
		//! チャンクの容量。
		static constexpr std::uint32_t mc_Capacity = 4096;
	};
}
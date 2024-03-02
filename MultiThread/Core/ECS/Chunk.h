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
	 * @brief ECS�ɂ�����G���e�B�e�B�Ƃ��̃R���|�[�l���g���i�[����`�����N��\���N���X�B
	 */
	class Chunk
	{
	public:
		/**
		 * @brief �R���X�g���N�^�B
		 * @param _archetype ���̃`�����N�Ɋ֘A�t������A�[�L�^�C�v�B
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
		 * @brief �V�����G���e�B�e�B���쐬���A�`�����N�ɒǉ����܂��B
		 * @param _chunkIndex �G���e�B�e�B�̃C���f�b�N�X�B
		 * @param _version �G���e�B�e�B�̃o�[�W�����B
		 * @return �쐬���ꂽ�`�����N���C���f�b�N�X�B
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
		 * @brief �w�肳�ꂽ�C���f�b�N�X�̃G���e�B�e�B��j�����܂��B
		 * @param _chunkIndex �j������G���e�B�e�B�̃`�����N���C���f�b�N�X�B
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
		 * @brief �G���e�B�e�B�����݂̃`�����N����ʂ̃`�����N�Ɉړ����܂��B
		 * @param _chunkIndex �ړ�����G���e�B�e�B�̃`�����N���C���f�b�N�X�B
		 * @param _entity �ړ�����G���e�B�e�B�B
		 * @param _other �ړ���̃`�����N�B
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
		 * @brief �w�肳�ꂽ�C���f�b�N�X�̃G���e�B�e�B�ɃR���|�[�l���g�f�[�^��ݒ肵�܂��B
		 * @param _chunkIndex �R���|�[�l���g�f�[�^��ݒ肷��G���e�B�e�B�̃`�����N���C���f�b�N�X�B
		 * @param _data �ݒ肷��R���|�[�l���g�f�[�^�B
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
		 * @brief �`�����N���ő�T�C�Y�ɒB���Ă��邩�ǂ����𔻒f���܂��B
		 * @return bool �`�����N���ő�T�C�Y�ɒB���Ă���ꍇ��true�B
		 */
		const bool IsMax() const noexcept
		{
			return m_Size == m_MaxSize;
		}

		/**
		 * @brief �A�[�L�^�C�v���擾���܂��B
		 * @return Archetype& �֘A�t����ꂽ�A�[�L�^�C�v�B
		 */
		Archetype& GetArchetype()
		{
			return m_Archetype;
		}

		/**
		* @brief �w��̃R���|�[�l���g���X�g���擾���܂��B
		* @return ComponentArray<CompT> �w��̌^�̃R���|�[�l���g���X�g�B
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
		 * @brief �`�����N���̃G���e�B�e�B�����擾���܂��B
		 * @return std::uint32_t �`�����N���̃G���e�B�e�B���B
		 */
		const std::uint32_t GetSize() const noexcept
		{
			return m_Size;
		}

	private:
		/**
		 * @brief �`�����N���󂩂ǂ����𔻒f���܂��B
		 * @return bool �`�����N����̏ꍇ��true�B
		 */
		const bool IsEmpty() const noexcept
		{
			return m_Size == 0;
		}

	private:
		//! ���̃`�����N�Ɋ֘A�t������A�[�L�^�C�v�B
		Archetype m_Archetype;
		//! �`�����N�̃f�[�^���i�[����|�C���^�B
		std::shared_ptr<std::byte[]> m_pBegin = nullptr;
		//! �`�����N���̃G���e�B�e�B���B
		std::uint32_t m_Size;
		//! �`�����N�̍ő�T�C�Y�B
		std::uint32_t m_MaxSize;
		//! �`�����N�̗e�ʁB
		static constexpr std::uint32_t mc_Capacity = 4096;
	};
}
#pragma once

#include <memory>
#include <typeinfo>
#include <bitset>
#include <map>
#include <cstdlib>

#include "Common/Id.h"
#include "Utilities/TypeInfo.h"

namespace ECS
{
	/**
	* @struct Archetype
	* @brief ECS�A�[�L�e�N�`���ɂ�����A�[�L�^�C�v��\���N���X�B
	*/
	struct Archetype
	{
	public:
		/**
		* @brief �R���X�g���N�^�B
		*/
		Archetype()
			: m_Signature(0)
			, m_ArchetypeSize(0)
			, m_ArchetypeMemorySize(0)
		{}

		/**
		* @brief �R�s�[�R���X�g���N�^�B
		* @param _other �R�s�[����A�[�L�^�C�v�B
		*/
		Archetype(const Archetype& _other)
		{
			m_Signature = _other.m_Signature;
			m_ComponentMemorySize = _other.m_ComponentMemorySize;
			m_ArchetypeMemorySize = _other.m_ArchetypeMemorySize;
			m_ArchetypeSize = _other.m_ArchetypeSize;
		}

		/**
		* @brief ����̃R���|�[�l���g�^�C�v���A�[�L�^�C�v�ɒǉ����܂��B
		* @tparam CompT �ǉ�����R���|�[�l���g�̌^�B
		* @return �A�[�L�^�C�v���g�ւ̎Q�ƁB
		*/
		template <typename CompT>
		inline const Archetype& AddType()
		{
			std::size_t compId = TypeManager::TypeInfo<CompT>::GetID();
			if (!m_Signature.test(compId))
			{
				m_Signature.set(compId);
				m_ComponentMemorySize[compId] = sizeof(CompT);
				m_ArchetypeMemorySize += sizeof(CompT);
				m_ArchetypeSize++;
			}
			return *this;
		}

		/**
		* @brief ����̃R���|�[�l���g�^�C�v���A�[�L�^�C�v���珜�O���܂��B
		* @tparam CompT ���O����R���|�[�l���g�̌^�B
		* @return �A�[�L�^�C�v���g�ւ̎Q�ƁB
		*/
		template <typename CompT>
		inline const Archetype& RemoveType()
		{
			std::size_t compId = TypeManager::TypeInfo<CompT>::GetID();
			if (m_Signature.test(compId))
			{
				m_Signature.reset(compId);
				m_ComponentMemorySize.erase(compId);
				m_ArchetypeMemorySize -= sizeof(CompT);
				m_ArchetypeSize--;
			}
			return *this;
		}

		/**
		 * @brief ���̃A�[�L�^�C�v�����݂̃A�[�L�^�C�v�Ɋ܂܂�邩�ǂ����𔻒f���܂��B
		 * @param _other ��r�Ώۂ̃A�[�L�^�C�v�B
		 * @return bool ���̃A�[�L�^�C�v���܂܂�Ă���ꍇ��true�B
		 */
		inline const bool IsContain(const Archetype& _other) const noexcept
		{
			return (m_Signature & _other.m_Signature) == _other.m_Signature;
		}

		/**
		 * @brief �w�肳�ꂽ�R���|�[�l���g�^�C�v�̃������I�t�Z�b�g���擾���܂��B
		 * @tparam CompT �I�t�Z�b�g���擾����R���|�[�l���g�̌^�B
		 * @return std::size_t �w�肳�ꂽ�R���|�[�l���g�̃������I�t�Z�b�g�B
		 */
		template <typename CompT>
		inline const std::size_t GetMemoryOffset()
		{
			std::size_t result = 0;
			std::size_t compId = TypeManager::TypeInfo<CompT>::GetID();
			for (std::size_t i = 0; i < compId; i++)
			{
				if (m_Signature.test(i))
				{
					result += m_ComponentMemorySize[i];
				}
			}
			return result;
		}

		/**
		 * @brief �w�肳�ꂽ�C���f�b�N�X�܂ł̃������I�t�Z�b�g���擾���܂��B
		 * @param _index �I�t�Z�b�g���擾����C���f�b�N�X�B
		 * @return std::size_t �w�肳�ꂽ�R���|�[�l���g�̃������I�t�Z�b�g�B
		 */
		inline const std::size_t GetMemroyOffsetByIndex(std::size_t _index)
		{
			if (_index == 0) return 0;

			std::size_t result = 0;
			auto it = m_ComponentMemorySize.begin();
			for (std::size_t i = 0; i < _index; i++, ++it)
			{
				result += it->second;
			}
			return result;
		}

		/**
		 * @brief �w�肳�ꂽ�C���f�b�N�X�̃������T�C�Y���擾���܂��B
		 * @param _index �T�C�Y���擾����C���f�b�N�X�B
		 * @return std::size_t �w�肳�ꂽ�R���|�[�l���g�̃������T�C�Y�B
		 */
		inline const std::size_t GetMemorySizeByIndex(std::size_t _index)
		{
			if ((_index + 1) > m_ArchetypeSize)
				std::abort();

			auto it = std::next(m_ComponentMemorySize.begin(), _index);
			return it->second;
		}

		/**
		 * @brief �w�肳�ꂽ�C���f�b�N�X��ID���擾���܂��B
		 * @param _index ID���擾����C���f�b�N�X�B
		 * @return std::size_t �w�肳�ꂽ�R���|�[�l���g��ID�B
		 */
		inline const std::size_t GetComponentIdByIndex(std::size_t _index)
		{
			if ((_index + 1) > m_ArchetypeSize)
				std::abort();

			auto it = std::next(m_ComponentMemorySize.begin(), _index);
			return it->first;
		}

		/**
		* @brief �A�[�L�^�C�v�Ɋ܂܂��R���|�[�l���g�̍��v�������T�C�Y���擾����B
		* @return �A�[�L�^�C�v�Ɋ܂܂��R���|�[�l���g�̍��v�������T�C�Y�B
		*/
		inline const std::size_t GetArchetypeMemorySize() const noexcept
		{
			return m_ArchetypeMemorySize;
		}

		/**
		* @brief �A�[�L�^�C�v�Ɋ܂܂��R���|�[�l���g�����擾����B
		* @return �A�[�L�^�C�v�Ɋ܂܂��R���|�[�l���g���B
		*/
		inline const std::size_t GetArchetypeSize() const noexcept
		{
			return m_ArchetypeSize;
		}

		/**
		* @brief �A�[�L�^�C�v�̃V�O�l�`�����擾����B
		* @return �A�[�L�^�C�v�̃V�O�l�`���B
		*/
		inline const std::bitset<cMaxComponentSize> GetSignature() const
		{
			return m_Signature;
		}

		/**
		* @brief ������Z�q�̃I�[�o�[���[�h
		*/
		Archetype& operator=(const Archetype& _other)
		{
			// ���g�̏ꍇ�͍s��Ȃ�
			if (this == &_other)
				return *this;

			m_Signature = _other.m_Signature;
			m_ComponentMemorySize = _other.m_ComponentMemorySize;
			m_ArchetypeMemorySize = _other.m_ArchetypeMemorySize;
			m_ArchetypeSize = _other.m_ArchetypeSize;

			return *this;
		}
	private:
		//! �R���|�[�l���g�̎�ނ�ǐՂ��邽�߂̃r�b�g�Z�b�g�B
		std::bitset<cMaxComponentSize> m_Signature;
		//! �e�R���|�[�l���g�^�C�v�̃������T�C�Y���i�[����}�b�v�B
		std::map<std::size_t, std::size_t> m_ComponentMemorySize;
		//! �A�[�L�^�C�v�Ɋ܂܂��R���|�[�l���g�̍��v�������T�C�Y�B
		std::size_t m_ArchetypeMemorySize = 0;
		//! �A�[�L�^�C�v�Ɋ܂܂��R���|�[�l���g�̎�ސ��B
		std::size_t m_ArchetypeSize = 0;
	};
}
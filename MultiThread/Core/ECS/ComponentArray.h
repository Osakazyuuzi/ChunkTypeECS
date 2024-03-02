#pragma once

#include <cstddef>

namespace ECS
{

	/**
	* @class ComponentArray
	* @brief �R���|�[�l���g�̔z����Ǘ�����N���X�ł��B
	* @tparam CompT �z��Ɋi�[�����R���|�[�l���g�̌^�B
	*/
	template <typename CompT>
	class ComponentArray
	{
	public:
		/**
		* @brief �R���X�g���N�^�B
		* @param _pBegin �R���|�[�l���g�z��̊J�n�|�C���^�B
		* @param _size �z��̃T�C�Y�B
		*/
		ComponentArray(CompT* _pBegin, const std::size_t _size)
		{
			m_pBegin = _pBegin;
			m_Size = _size;
		}

		/**
		* @brief �w�肳�ꂽ�C���f�b�N�X�̃R���|�[�l���g�ւ̎Q�Ƃ�Ԃ��܂��B
		* @param _index �擾�������R���|�[�l���g�̃C���f�b�N�X�B
		* @return �w�肳�ꂽ�C���f�b�N�X�̃R���|�[�l���g�ւ̎Q�ƁB
		*/
		inline CompT& operator[](const int _index)
		{
			if (_index < 0 || m_Size <= _index)
				std::abort();

			return m_pBegin[_index];
		}

		/**
		* @brief �z��̊J�n�|�C���^��Ԃ��܂��B
		* @return �R���|�[�l���g�z��̊J�n�|�C���^�B
		*/
		inline CompT* Begin()
		{
			return m_pBegin;
		}

		/**
		* @brief �z��̏I���|�C���^��Ԃ��܂��B
		* @return �R���|�[�l���g�z��̏I���|�C���^�B
		*/
		inline CompT* End()
		{
			return m_pBegin + m_Size;
		}

	private:
		//! �z��̊J�n�|�C���^�B
		CompT* m_pBegin = nullptr;
		//! �z��̃T�C�Y�B
		std::size_t m_Size = 0;
	};
}
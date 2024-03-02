#pragma once

#include "Common/Id.h"

namespace ECS
{
	/**
	* @struct Entity
	* @brief ECS�A�[�L�e�N�`���ɂ�����G���e�B�e�B��\���\���́B
	*/
	struct Entity
	{
		/**
		* @brief �R���X�g���N�^
		* @param _index �G���e�B�e�B�̃C���f�b�N�X�B
		* @param _version �G���e�B�e�B�̃o�[�W����
		*/
		Entity(const std::uint32_t _index, const std::uint32_t _version)
			: m_Identifier(NULL)
		{
			SetIndex(m_Identifier, _index);
			SetVersion(m_Identifier, _version);
		}

		/**
		* @brief �R���X�g���N�^
		* @param _Identifier �G���e�B�e�B�̎��ʃr�b�g�B
		*/
		Entity(const EntityIdentifier _Identifier)
			: m_Identifier(_Identifier)
		{}

		//! �G���e�B�e�B�̈�ӂ̎��ʃr�b�g�B
		EntityIdentifier m_Identifier;
	};
}
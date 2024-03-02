#pragma once

#include <cstdint>  // int�n��
#include <cstdlib>  // std::size_t

//! �R���|�[�l���g�̎�ނ̍ő吔�B
constexpr std::size_t cMaxComponentSize = 128;

//! �G���e�B�e�B�̊Ǘ��C���f�b�N�X�̌^�B
using EntityIndex = std::uint32_t;
//! �G���e�B�e�B�̊Ǘ��o�[�W�����̌^�B
using EntityVersion = std::uint32_t;
//! �G���e�B�e�B�̎���ID�̌^�B
using EntityIdentifier = std::uint64_t;

/**
* @brief ����ID�ɃC���f�b�N�X��ݒ肷��B
* @param _identifier �C���f�b�N�X��K�p����G���e�B�e�B�̎���ID�B
* @param _index �ݒ肷��C���f�b�N�X�l�B
*/
static inline void SetIndex(EntityIdentifier& _identifier, EntityIndex _index)
{
    constexpr EntityIdentifier versionMask = static_cast<EntityIdentifier>(EntityVersion(-1));
    _identifier = (_identifier & versionMask) | (static_cast<EntityIdentifier>(_index) << 32);
}

/**
* @brief ����ID�Ƀo�[�W������ݒ肷��B
* @param _identifier& �o�[�W������K�p����G���e�B�e�B�̎���ID�B
* @param _version �ݒ肷��o�[�W�����l�B
*/
static inline void SetVersion(EntityIdentifier& _identifier, EntityVersion _version)
{
    constexpr EntityIdentifier indexMask = ~static_cast<EntityIdentifier>(EntityVersion(-1));
    _identifier = (_identifier & indexMask) | _version;
}

/**
* @brief ����ID����C���f�b�N�X���擾����B
* @param _identifier �G���e�B�e�B�̎���ID�B
* @return �G���e�B�e�B�̃C���f�b�N�X�B
*/
static inline EntityIndex GetIndex(const EntityIdentifier& _identifier)
{
    return static_cast<EntityIndex>(_identifier >> 32);
}

/**
* @brief ����ID����o�[�W�������擾����B
* @param _identifier �G���e�B�e�B�̎���ID�B
* @return �G���e�B�e�B�̃o�[�W�����B
*/
static inline EntityVersion GetVersion(const EntityIdentifier& _identifier)
{
    return static_cast<EntityVersion>(_identifier);
}
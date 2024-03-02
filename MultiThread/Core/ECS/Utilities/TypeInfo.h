#pragma once

#include <typeinfo>
#include <string>

/**
* @class TypeManager
* @brief �^�����Ǘ�����N���X
*/
class TypeManager
{
public:
	/**
	* @brief �R���X�g���N�^
	* @note �R���|�[�l���g��ID���m�肳����B
	*/
	TypeManager() {}

	/**
	* @class TypeInfo
	* @brief ����̌^�̏���ێ�����N���X�B
	*/
	template <typename T>
	class TypeInfo
	{
	private:
		//! �^ID
		static std::size_t m_Id;
		//! �^��
		static std::string m_szName;

	private:
		/**
		* @brief �^����o�^����B
		*/
		static inline void Register()
		{
			TypeInfo<T>::m_Id = TypeManager::m_NextId++;
			TypeInfo<T>::m_szName = typeid(T).name();
		}

	public:

		/**
		* @brief �^ID���擾����B
		* @return const std::size_t& �^ID
		*/
		static inline const std::size_t& GetID()
		{
			if (TypeInfo<T>::m_Id == 0) Register();

			return m_Id;
		}

		/**
		* @brief �^�����擾����B
		* @return const std::string& �^��
		*/
		static inline const std::string& GetName()
		{
			if (TypeInfo<T>::m_Id == 0) Register();
			return m_szName;
		}
	};

private:
	//! ���Ɋ��蓖�Ă�^ID
	static std::size_t m_NextId;
};

template <typename T>
std::size_t TypeManager::TypeInfo<T>::m_Id = 0;

template <typename T>
std::string TypeManager::TypeInfo<T>::m_szName;
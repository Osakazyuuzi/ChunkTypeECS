#pragma once

#include "Archetype.h"
#include "Chunk.h"
#include "World.h"
#include "EntityManager.h"

#include "../../AsyncFunctionManager.h"

namespace ECS
{
	/**
	* @class SystemBase
	* @brief �V�X�e���x�[�X�N���X�B�S�ẴV�X�e���͂�����p������B
	*/
	class SystemBase
	{
	public:
		/**
		* @brief �R���X�g���N�^�B
		* @param _pWorld �����郏�[���h�ւ̃|�C���^�B
		* @param _Id �V�X�e�������ʂ����ӂ�ID�B
		*/
		SystemBase(World* _pWorld, std::size_t _Id)
			: m_pWorld(_pWorld)
			, m_SystemId(_Id)
		{}

		/**
		* @brief �ŏ��Ɉ�x�̂ݏ��������s���܂��B
		*/
		virtual void Init() {}

		/**
		* @brief ���������s���܂��B
		* @param _deltaTime �O�t���[���Ƃ̍�[ms]
		*/
		virtual void Update(float _deltaTime) {}

		/**
		* @brief �`������s���܂��B
		*/
		virtual void Draw() {}

		/**
		* @brief ���s����̂ɕK�v�Ƃ���A�[�L�^�C�v���擾���܂��B
		* @return const Archetype& ���s����̂ɕK�v�ȃA�[�L�^�C�v�B
		*/
		inline const Archetype& GetArchetype() const noexcept
		{
			return m_Archetype;
		}

		/**
		* @brief ���s����̂ɕK�v�Ƃ���A�[�L�^�C�v���擾���܂��B
		* @return Archetype* ���s����̂ɕK�v�ȃA�[�L�^�C�v�B
		*/
		inline Archetype* GetArchetype()
		{
			return &m_Archetype;
		}

		/**
		* @brief �����Ă��郏�[���h�̃G���e�B�e�B�}�l�[�W���[���擾���܂��B
		* @return std::shared_ptr<EntityManager> �����Ă��郏�[���h�̃G���e�B�e�B�}�l�[�W���[�B
		*/
		std::shared_ptr<EntityManager> GetEntityManager();

		/**
		* @brief �V�X�e�������ʂ����ӂ�ID���擾���܂��B
		* @return std::size_t ��ӂ�ID
		*/
		std::size_t GetSystemId() const noexcept
		{
			return m_SystemId;
		}

	protected:
		/**
		* @brief �K�v�ȃA�[�L�^�C�v���܂�ł���G���e�B�e�B�Ɋ֐������s���܂��B
		* @tparam Components �R���|�[�l���g�̌^�̃��X�g�B
		* @param _func ���s����֐��B
		*/
		template <class... Components, typename Func>
		void ExecuteForEntitiesMatching(Func&& _func)
		{
			// �A�[�L�^�C�v���܂܂�Ă���`�����N���X�g���擾
			auto pChunkList = m_pWorld->GetEntityManager()->GetContainChunkList(m_Archetype);

			for (auto&& pChunk : pChunkList)
			{
				auto func = std::forward<Func>(_func);


				// �K�v�ȃR���|�[�l���g�Q�𔲂��o���āA���������s
				ExecuteForEntitiesMatchingImpl(pChunk, func, pChunk->GetComponentList<Components>()...);
			}
		}

	private:
		/**
		* @brief �S�Ă̊Y���G���e�B�e�B�Ɋ֐������s���܂��B
		* @param _pChunk �Y���`�����N�B
		* @param _func ���s����֐��B
		* @param _args �֐��Ɉ����n���R���|�[�l���g�Q�B
		*/
		template <typename Func, class... Args>
		static void ExecuteForEntitiesMatchingImpl(
			Chunk* _pChunk,
			Func&& _func,
			Args... _args)
		{
			for (std::uint32_t i = 0; i < _pChunk->GetSize(); ++i)
			{
				_func(_args[i]...);
			}
		}

	protected:
		//! ���s����̂ɕK�v�Ƃ���A�[�L�^�C�v�B
		Archetype m_Archetype;
	protected:
		//! �V�X�e��ID
		std::size_t m_SystemId;
	private:
		//! �����Ă��郏�[���h�ւ̃|�C���^�B
		World* m_pWorld = nullptr;
	};
}
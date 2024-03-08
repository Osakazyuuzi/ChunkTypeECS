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
	* @brief ECSアーキテクチャにおけるアーキタイプを表すクラス。
	*/
	struct Archetype
	{
	public:
		/**
		* @brief コンストラクタ。
		*/
		Archetype()
			: m_Signature(0)
			, m_ArchetypeSize(0)
			, m_ArchetypeMemorySize(0)
		{}

		/**
		* @brief コピーコンストラクタ。
		* @param _other コピーするアーキタイプ。
		*/
		Archetype(const Archetype& _other)
		{
			m_Signature = _other.m_Signature;
			m_ComponentMemorySize = _other.m_ComponentMemorySize;
			m_ArchetypeMemorySize = _other.m_ArchetypeMemorySize;
			m_ArchetypeSize = _other.m_ArchetypeSize;
		}

		/**
		* @brief 特定のコンポーネントタイプをアーキタイプに追加します。
		* @tparam CompT 追加するコンポーネントの型。
		* @return アーキタイプ自身への参照。
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
		* @brief 特定のコンポーネントタイプをアーキタイプから除外します。
		* @tparam CompT 除外するコンポーネントの型。
		* @return アーキタイプ自身への参照。
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
		 * @brief 他のアーキタイプが現在のアーキタイプに含まれるかどうかを判断します。
		 * @param _other 比較対象のアーキタイプ。
		 * @return bool 他のアーキタイプが含まれている場合はtrue。
		 */
		inline const bool IsContain(const Archetype& _other) const noexcept
		{
			return (m_Signature & _other.m_Signature) == _other.m_Signature;
		}

		/**
		 * @brief 指定されたコンポーネントタイプのメモリオフセットを取得します。
		 * @tparam CompT オフセットを取得するコンポーネントの型。
		 * @return std::size_t 指定されたコンポーネントのメモリオフセット。
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
		 * @brief 指定されたインデックスまでのメモリオフセットを取得します。
		 * @param _index オフセットを取得するインデックス。
		 * @return std::size_t 指定されたコンポーネントのメモリオフセット。
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
		 * @brief 指定されたインデックスのメモリサイズを取得します。
		 * @param _index サイズを取得するインデックス。
		 * @return std::size_t 指定されたコンポーネントのメモリサイズ。
		 */
		inline const std::size_t GetMemorySizeByIndex(std::size_t _index)
		{
			if ((_index + 1) > m_ArchetypeSize)
				std::abort();

			auto it = std::next(m_ComponentMemorySize.begin(), _index);
			return it->second;
		}

		/**
		 * @brief 指定されたインデックスのIDを取得します。
		 * @param _index IDを取得するインデックス。
		 * @return std::size_t 指定されたコンポーネントのID。
		 */
		inline const std::size_t GetComponentIdByIndex(std::size_t _index)
		{
			if ((_index + 1) > m_ArchetypeSize)
				std::abort();

			auto it = std::next(m_ComponentMemorySize.begin(), _index);
			return it->first;
		}

		/**
		* @brief アーキタイプに含まれるコンポーネントの合計メモリサイズを取得する。
		* @return アーキタイプに含まれるコンポーネントの合計メモリサイズ。
		*/
		inline const std::size_t GetArchetypeMemorySize() const noexcept
		{
			return m_ArchetypeMemorySize;
		}

		/**
		* @brief アーキタイプに含まれるコンポーネント数を取得する。
		* @return アーキタイプに含まれるコンポーネント数。
		*/
		inline const std::size_t GetArchetypeSize() const noexcept
		{
			return m_ArchetypeSize;
		}

		/**
		* @brief アーキタイプのシグネチャを取得する。
		* @return アーキタイプのシグネチャ。
		*/
		inline const std::bitset<cMaxComponentSize> GetSignature() const
		{
			return m_Signature;
		}

		/**
		* @brief 代入演算子のオーバーロード
		*/
		Archetype& operator=(const Archetype& _other)
		{
			// 自身の場合は行わない
			if (this == &_other)
				return *this;

			m_Signature = _other.m_Signature;
			m_ComponentMemorySize = _other.m_ComponentMemorySize;
			m_ArchetypeMemorySize = _other.m_ArchetypeMemorySize;
			m_ArchetypeSize = _other.m_ArchetypeSize;

			return *this;
		}
	private:
		//! コンポーネントの種類を追跡するためのビットセット。
		std::bitset<cMaxComponentSize> m_Signature;
		//! 各コンポーネントタイプのメモリサイズを格納するマップ。
		std::map<std::size_t, std::size_t> m_ComponentMemorySize;
		//! アーキタイプに含まれるコンポーネントの合計メモリサイズ。
		std::size_t m_ArchetypeMemorySize = 0;
		//! アーキタイプに含まれるコンポーネントの種類数。
		std::size_t m_ArchetypeSize = 0;
	};
}
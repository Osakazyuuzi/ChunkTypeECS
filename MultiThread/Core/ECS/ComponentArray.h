#pragma once

#include <cstddef>

namespace ECS
{

	/**
	* @class ComponentArray
	* @brief コンポーネントの配列を管理するクラスです。
	* @tparam CompT 配列に格納されるコンポーネントの型。
	*/
	template <typename CompT>
	class ComponentArray
	{
	public:
		/**
		* @brief コンストラクタ。
		* @param _pBegin コンポーネント配列の開始ポインタ。
		* @param _size 配列のサイズ。
		*/
		ComponentArray(CompT* _pBegin, const std::size_t _size)
		{
			m_pBegin = _pBegin;
			m_Size = _size;
		}

		/**
		* @brief 指定されたインデックスのコンポーネントへの参照を返します。
		* @param _index 取得したいコンポーネントのインデックス。
		* @return 指定されたインデックスのコンポーネントへの参照。
		*/
		inline CompT& operator[](const int _index)
		{
			if (_index < 0 || m_Size <= _index)
				std::abort();

			return m_pBegin[_index];
		}

		/**
		* @brief 配列の開始ポインタを返します。
		* @return コンポーネント配列の開始ポインタ。
		*/
		inline CompT* Begin()
		{
			return m_pBegin;
		}

		/**
		* @brief 配列の終了ポインタを返します。
		* @return コンポーネント配列の終了ポインタ。
		*/
		inline CompT* End()
		{
			return m_pBegin + m_Size;
		}

	private:
		//! 配列の開始ポインタ。
		CompT* m_pBegin = nullptr;
		//! 配列のサイズ。
		std::size_t m_Size = 0;
	};
}
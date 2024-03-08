#pragma once

#include <chrono>
#include <iostream>

class Profiler {
public:
	/**
	* @brief 計測開始関数
	*/
	void ProfileStart()
	{
		// 現在時間で初期化して計測開始する
		m_StartTime = std::chrono::high_resolution_clock::now();
	}
	
	/**
	* @brief 経過時間出力関数
	*/
	void OutputElapsedTime()
	{
		// 現在時間から計測開始時間の差で経過時間を割り出す
		long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>
			(std::chrono::high_resolution_clock::now() - m_StartTime).count();
		
		// 出力する
		std::cout << "経過時間: " << duration << " ns " <<
			"FPS値: " << (1000000000 / duration) << std::endl;
	}

private:
	//! 計測開始時間
	std::chrono::high_resolution_clock::time_point m_StartTime = {};
};
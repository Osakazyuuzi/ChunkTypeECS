#pragma once

#include <chrono>
#include <iostream>

class Profiler {
public:
	/**
	* @brief �v���J�n�֐�
	*/
	void ProfileStart()
	{
		// ���ݎ��Ԃŏ��������Čv���J�n����
		m_StartTime = std::chrono::high_resolution_clock::now();
	}
	
	/**
	* @brief �o�ߎ��ԏo�͊֐�
	*/
	void OutputElapsedTime()
	{
		// ���ݎ��Ԃ���v���J�n���Ԃ̍��Ōo�ߎ��Ԃ�����o��
		long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>
			(std::chrono::high_resolution_clock::now() - m_StartTime).count();
		
		// �o�͂���
		std::cout << "�o�ߎ���: " << duration << " ns " <<
			"FPS�l: " << (1000000000 / duration) << std::endl;
	}

private:
	//! �v���J�n����
	std::chrono::high_resolution_clock::time_point m_StartTime = {};
};
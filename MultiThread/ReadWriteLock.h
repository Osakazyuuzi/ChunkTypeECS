#pragma once

#include <mutex>
#include <condition_variable>

/**
 * @class ReadWriteLock
 * @brief �ǂݎ��Ə������݃A�N�Z�X�𓯊����邽�߂̃��b�N�@�\��񋟂��܂��B
 *
 * ���̃N���X�́A�����̓ǂݎ��A�N�Z�X�������ɍs���邱�Ƃ������Ȃ���A
 * �������݃A�N�Z�X���r���I�ɍs���邱�Ƃ�ۏ؂��܂��B����ɂ��A
 * �ǂݎ��A�N�Z�X���������������݃A�N�Z�X�����Ȃ��V�i���I�ł̐��\�����サ�܂��B
 */
class ReadWriteLock {
public:
    ReadWriteLock()
        : m_RefCount(0), m_bExistsWriter(false) {}

    /**
     * @brief �ǂݎ��A�N�Z�X�p�̃��b�N���擾���܂��B
     *
     * ���̃X���b�h���������ݒ��łȂ��ꍇ�ɂ̂݁A�ǂݎ��A�N�Z�X�������܂��B
     * �������݃X���b�h�����݂���ꍇ�A���̃X���b�h�����b�N���������܂őҋ@���܂��B
     */
    void LockRead() {
        std::unique_lock<std::mutex> lock(m_Mutex);

        m_ReadCondition.wait(lock, [this]() { return !m_bExistsWriter; });
        ++m_RefCount;
    }

    /**
     * @brief �ǂݎ��A�N�Z�X�p�̃��b�N��������܂��B
     *
     * �ǂݎ��J�E���^���f�N�������g���A���̏������ݑ҂��X���b�h������ꍇ�A
     * ���̈���N�����܂��B
     */
    void UnlockRead() {
        std::unique_lock<std::mutex> lock(m_Mutex);
        if (--m_RefCount == 0) {
            m_WriteCondition.notify_one();
        }
    }

    /**
     * @brief �������݃A�N�Z�X�p�̃��b�N���擾���܂��B
     *
     * ���̃X���b�h���������݂܂��͓ǂݎ�蒆�łȂ��ꍇ�ɂ̂݁A�������݃A�N�Z�X�������܂��B
     * �����̏�������������Ȃ��ꍇ�A���������������܂őҋ@���܂��B
     */
    void LockWrite() {
        std::unique_lock<std::mutex> lock(m_Mutex);

        m_WriteCondition.wait(lock, [this]() { return !m_bExistsWriter && m_RefCount == 0; });
        m_bExistsWriter = true;
    }

    /**
     * @brief �������݃A�N�Z�X�p�̃��b�N��������܂��B
     *
     * �������݃t���O�����Z�b�g���A�ҋ@���̏������݂܂��͓ǂݎ��X���b�h���N�����܂��B
     */
    void UnlockWrite() {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_bExistsWriter = false;

        // �������ݑ҂��̃X���b�h������΁A��������N�����܂��B
        m_WriteCondition.notify_one();
        // �ǂݎ��҂��̃X���b�h������΁A�S�Ă��N�����܂��B
        m_ReadCondition.notify_all();
    }

private:
    std::mutex m_Mutex;  ///< ���b�N��Ԃ̓�������邽�߂̃~���[�e�b�N�X�B
    std::condition_variable m_ReadCondition;  ///< �ǂݎ��X���b�h�̑ҋ@�����ϐ��B
    std::condition_variable m_WriteCondition; ///< �������݃X���b�h�̑ҋ@�����ϐ��B
    int m_RefCount;  ///< ���݂̓ǂݎ��X���b�h�̐��B
    bool m_bExistsWriter;  ///< ���ݏ������݂��s���Ă���X���b�h�����݂��邩�ǂ����B
};

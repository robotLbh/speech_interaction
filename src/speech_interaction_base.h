#ifndef SPEECH_INTERACTION_BASE_H_
#define SPEECH_INTERACTION_BASE_H_ 
#include <thread>
#include <mutex>
#include <condition_variable>
//#include <queue>
#include <vector>
#include <memory>
#include "speech_interaction_strategy.h"
namespace speech_interaction
{
    typedef std::shared_ptr<SpeechInteractionStrategy> Sp;

    /**@ brief 表示交互所处的阶段
     * 0 交互停止     1 交互正常进行
     * 2 语音播放完成 3 语音播放中断
     * 4 语音识别阶段 5 语义理解阶段
     * 6 指令识别阶段 7 语音合成阶段
     * 8 语音播放阶段
     * 其中2,3表示瞬间结果，其他是状态过程。
     */
    typedef enum InteractionState
    {
        stopped,running,finished,interrupted,
        SR,LU,CR,SS,SB,s_end,
    } State;

    class SpeechInteractionBase
    {
        public:
            /**@brief 语音交互类的构造函数。
             * @param vis SpeechInteractionStrategy对象指针。
             * 此次使用智能指针，方便内存管理。
             */
            SpeechInteractionBase(Sp vis);
            ~SpeechInteractionBase();

            /**@brief 开始语音交互，完成人与机器语音互动功能。
             * @return 0表示成功，xx代表两位错误码。
             */
            int startSpeechInteraction(std::string ask="noask");

            /**@brief 结束语音交互。
             * @return 0表示成功，xx代表两位错误码。
             */
            int stopSpeechInteraction(void);

            /**@brief 合成文本字符串的语音，然后播放。
             * @ param text 需要播放的文本字符串。
             * @ param type 0依次播放，1将播放加到播放队列头部，2立刻播放
             * @return 0表示成功，xx代表两位错误码。
             */
            int textToPlay(const std::string& text,int type=0);

            /**@brief 问答后播放答案。
             * @ param ask 需要问答的文本字符串。
             * @return 0表示成功，xx代表两位错误码。
             */
            int askToPlay(const std::string& ask);

            /**@brief 将文本合成语音。
             * @param text 需要合成语音的文本。
             * @param speech 合成的语音文件。
             *@return 0表示成功，xx代表两位错误码。
             */
            int textToSpeech(const std::string& text,const std::string& speech);

            /**@brief 音频文件加入播放队列。
             * @ param speech 需要播放的音频文件。
             * @ param type 0依次播放，1将播放加到播放队列头部，2立刻播放
             * @return 0表示成功，xx代表两位错误码。
             */
            int speechBroadcasting(const std::string& speech,int type=0);

            /**@brief 清空播放队列，停止语音播放。
             * @return 0表示成功，xx代表两位错误码。
             */
            int speechBreaking();

            /**@brief 获取交互当前所在的过程。
             * @return 参见enum State
             */
            State getState();

        protected:
            /**@brief 控制指令的识别函数
             * @param cmd 待识别文本
             * @param key 指令识别符
             * @return 0语音停止，1语音继续。
             */
            virtual int cmdRecognition(const std::vector<std::string>& cmd);

            /**@brief 交互过程状态改变会调用一次此函数
             */
            virtual int stateProcessing(State state);

            Sp strategy_;//@brief 指向SpeechInteractionStrategy对象。
            int control_;//@brief 开始停止语音交互标志	

        private:
            /**@brief 语音识别到语音合成线程。
            */
            int threadSI();

            /**@brief 音频播放管理线程。
            */
            int threadSB();

            /**@brief 改变当前的交互状态
             * param state 将当前状态改成state
             */
            int changeState(State state);

            /**@brief 随机生成文件名
            */
            std::string randFilename();

        private:
            std::shared_ptr<std::thread> thread_si_;//@brief 语音交互线程指针。
            std::shared_ptr<std::thread> thread_sb_;//@brief 语音播放线程指针。

            std::mutex mutex_playlist_;//@brief 音频播放队列互斥量。
            std::mutex mutex_BandR_;//@brief broadcast 和 recognition互斥锁。

            std::condition_variable cv_sb_;//@brief 音频播放的添加变量，有队列加入就触发一次
            std::mutex mutex_sb_;

            std::condition_variable cv_ss_;//@brief 开始停止语音开关同步。
            std::mutex mutex_ss_;

            std::mutex mutex_state_;//@brief 交互过程状态state_改变互斥。

        private:
            int stop_thread_;//@brief 线程退出标志
            State state_;//@brief 交互所处过程 
            //std::queue<std::string> playlist_;//@brief 语音播放队列
            std::vector<std::string> playlist_;//@brief 语音播放队列
    };
}
#endif //SPEECH_INTERATION_BASE_H_

#ifndef SPEECH_INTERACTION_STRATEGY_H_
#define SPEECH_INTERACTION_STRATEGY_H_
#include <iostream>
#include <vector>
#include <glog/logging.h>
#include <mutex>
namespace speech_interaction
{
    class SpeechInteractionStrategy 
    {
        public:
            /**@brief 麦克输入的语音转化成文本。
             * @param text 存储转化后的文本字符串。
             * @return 0表示成功，xx为两位异常码。
             */
            virtual int speechRecognition(std::string& text)=0;

            /**@brief 实现问答功能。
             * @param ask 对话问答的输入字符串。
             * @param answer 对话问答的输出字符串,[0]答案，【1】类型，【2】类型的附件内容。
             * @return 0表示成功，xx为两位异常码。
             */
            virtual int languageUnderstanding(const std::string& ask,std::vector<std::string>& answer)=0;

            /**
             * @brief 文本转化成音频文件。 
             * @param text 需要生成语音的文本字符串。
             * @param speech 生成的音频的存储位置。
             * @return 0表示成功，xx为两位异常码。
             */
            virtual int speechSynthesizer(const std::string& text,const std::string& speech)=0;

            /**@brief 播放音频。
             * @param speech 音频文件（路径/名字）。
             * @return 0表示成功，xx为两位异常码。
             */
            virtual int speechBroadcasting(const std::string& filename)=0;

            /**@brief 打断播放音频。
             * @return 0表示成功，xx为两位异常码。
             */ 
            virtual int speechBreaking()=0;

            /**@brief 中断某个过程
             * param procedure 代表某个过程
             */
            virtual int sessionEnd(int procedure)=0;

        public:
            /**
             * @brief 基类虚析构函数
             */
            virtual ~SpeechInteractionStrategy()
            {
            }

    };

    /**
     * @class IflyTuling 
     * @brief 科大讯飞+图灵机器人实现语音基类，
     * 科大讯飞语音识别与语音合成技术+图灵问答技术。
     * @function 成员函数中音频文件为WAV格式。
     */
    class IflyTuling: public SpeechInteractionStrategy 
    {
        public:
            IflyTuling(std::vector<std::string>& params);
            virtual ~IflyTuling(void);
        public:
            int speechRecognition(std::string& text);
            int languageUnderstanding(const std::string& ask,std::vector<std::string>& answer);
            int speechSynthesizer(const std::string& text,const std::string& speech);
            int speechBroadcasting(const std::string& filename);
            int speechBroadcasting();
            int speechBreaking();
            int sessionEnd(int procedure);
        private:
            /**@brief 对语音识别出的文字进行过滤。
             * @param ask 语音识别出的文字
             * @return 0表示文本可继续下一步问答，1表示无效识别
             */
            int askFilter(std::string& ask);

            /**@ vars
             * @brief 四个主要过程的互斥，每个过程不能同时调用
             */
            std::mutex mutex_sr_;
            std::mutex mutex_lu_;
            std::mutex mutex_ss_;
            std::mutex mutex_sb_;
    };
}
#endif //SPEECH_INTERACTION_STRATEGY_H_

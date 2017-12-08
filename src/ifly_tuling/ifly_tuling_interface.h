#ifndef IFLY_TULING_INTERFACE_H_
#define IFLY_TULING_INTERFACE_H_

/**@extern "C" IflyTuling类成员函数调用的第三方库提供的C函数。
*/
#ifdef __cplusplus
extern "C" {
#endif 

    /**@brief 科大讯飞账号登入。
     * @return 0代表成功，xx代表两位错误码。
     */
    int iflyInitC();

    /**@brief 科大讯飞语音识别功能，语音转文本。
     * @return 0代表成功，xx代表两位错误码。
     */ 
    int speechRecognitionC(char* text);

    /**@brief 科大讯飞语音合成函数。
     * @param text 需要合成语音的文本；
     * @param speech 合成的音频文件；
     * @return 0代表成功，xx代表两位错误码。
     */ 
    int speechSynthesizerC(const char* text, const char* speech);

    /**@brief 播放音频函数。
     * @param speech 需要播放的音频文件；
     * @return 0代表成功，xx代表两位错误码。
     */ 
    int speechBroadcastingC(const char* speech);

    /**@brief 终端音频播放，与speechBroadcasting配对。
     * @return 0代表成功，xx代表两位错误码。
     */
    int speechBreakingC();

    /**@brief 退出讯飞
     */
    int iflyLogout();

    /**@brief 停止科大讯飞语音识别语音输入。
     * @return 0代表成功，xx代表两位错误码。
     */ 
    int speechRecognitionEndC();

    /**@brief 停止科大讯飞语音合成。
     * @return 0代表成功，xx代表两位错误码。
     */ 
    int speechSynthesizerEndC();

#ifdef __cplusplus
}
#endif

/**@C++ IflyTuling类成员函数调用的第三方库提供的C++接口。
*/ 
//C++

/**@brief 将文本传给公司后台，进行图灵问答，然后返回答案文本。
 * @param ask 问题文本；
 * @param answer 答案文本；
 * return 0代表成功，xx代表两位错误码。
 */
int languageUnderstandingCpp(const std::string& ask,std::vector<std::string>& answer);

#endif // IFLY_TULING_INTERFACE_H_


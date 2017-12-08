#ifndef IFLY_TULING_PARAMS_H_
#define IFLY_TULING_PARAMS_H_

/**@ struct 
 * @brief 科大讯飞和图灵问答的参数结构体。
 */
typedef struct initParmas
{
    const char* appid_ifly;//@brief 讯飞MSGPLogin的appid；
    const char* params_sr;//@brief 讯飞语音识别模块初始化参数；
    const char* params_ss;//@briedf 讯飞语音合成模块初始化参数；
    const char* robotid_tuling;
    const char* http_tuling;
} params_t;

extern params_t params_;//@brief 参数结构体的全局变量；

#endif // IFLY_TULING_PARAMS_H_


#include <speech_interaction_base.h>
#include "glog_speech.h"
#include <unistd.h>
using namespace std;
using namespace speech_interaction;
int main ()
{
    std::string name="test";
    glogInit(name);
    std::vector<std::string> params;
    string appid_ifly="appid = 5a039caa, work_dir = .";
    string params_sr ="sub = iat, domain = iat, language = zh_cn,accent = mandarin, "
        "sample_rate = 16000, result_type = plain, result_encoding = utf8";
    string params_ss = "voice_name = xiaoyan, text_encoding = utf8, sample_rate = 16000,"
        "speed = 50, volume = 50, pitch = 50, rdn = 0"; 
    string http="http://www.tuling123.com/openapi/api";
    string robot_key="4931056ad6e44e29aea56ef942b3b411";
    params.push_back(appid_ifly);
    params.push_back(params_sr);
    params.push_back(params_ss);
    params.push_back(http);
    params.push_back(robot_key);
    std::shared_ptr<SpeechInteractionStrategy> vis=std::make_shared<IflyTuling>(params);
    SpeechInteractionBase vi(vis);
    cout<<"**************************"<<endl;
    cout<<"现在你可以通过键盘数字键控制交互方式："<<endl;
    cout<<"按1开始语音交互，0停止语音交互。2打断播放，";
    cout<<"3进行一次文本播放，4进行一次文本对话播放，5退出程序"<<endl;
    cout<<"**************************"<<endl;
    string key,text;
    while(key!="5"){
        cin>>key;
        if(key=="0")
            vi.stopSpeechInteraction();
        if(key=="1")
            vi.startSpeechInteraction("#开始");
        if(key=="2")
            vi.speechBreaking();
        if(key=="3"){
            cout<<"-----------------"<<endl;
            cout<<"请输入要播放的文字："<<endl;
            cin>>text;
            vi.textToPlay(text);
        }
        if(key=="4"){
            cout<<"-----------------"<<endl;
            cout<<"请输入要问答的问题："<<endl;
            cin>>text;
            vi.askToPlay(text);
        }
    }
    glogShutdown();
    return 0;
}

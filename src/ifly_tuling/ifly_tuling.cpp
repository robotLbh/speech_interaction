#include "../speech_interaction_strategy.h"
#include "ifly_tuling_interface.h"
#include "ifly_tuling_params.h"
namespace speech_interaction 
{
    int glog_;
    IflyTuling::IflyTuling(std::vector<std::string>& params)
    {
        params_.appid_ifly=params[0].c_str();
        params_.params_sr=params[1].c_str(); 
        params_.params_ss=params[2].c_str(); 
        params_.http_tuling=params[3].c_str();
        params_.robotid_tuling=params[4].c_str();
        int ret=iflyInitC();
        CHECK(ret==0)<<"ifly MSPLogin failed , Error code:"<<ret;
        LOG(INFO)<<"ifly MSPLogin succeed";
    }
    IflyTuling::~IflyTuling()
    {
    }
    int IflyTuling::sessionEnd(int procedure)
    {
        switch (procedure){
            case 0:{
                       int ret=speechRecognitionEndC();
                       ret=speechSynthesizerEndC();
                       ret = iflyLogout();
                       return ret;
                   }
            case 1:{
                       int ret=speechRecognitionEndC();
                       return ret;
                   }
            case 2:{
                       int ret=speechSynthesizerEndC();
                       return ret;
                   }
            case 3:{
                       int ret = iflyLogout();
                       return ret;
                   }
            default:
                   return 10;
        }
    }
    int IflyTuling::speechRecognition(std::string& text)
    {
        std::unique_lock<std::mutex> ul(mutex_sr_);
        char* text_c=new char[5000];
        LOG(INFO)<<"speechRecognition......";
        int ret=speechRecognitionC(text_c);
        text=text_c;
        delete[] text_c;
        if(ret==0){
            if(askFilter(text)){
                LOG(WARNING)<<"speechRecognition succeed but invalid char. ask:"<<text;
                return 11;
            }
            LOG(INFO)<<"speechRecognition succeed. ask.size():"<<text.size();
            LOG(INFO)<<"speechRecognition result:"<<text;
            return 0;
        }else{
            LOG(ERROR)<<"speechRecognition failed. erroecode:"<<ret;
            sessionEnd(3);
            iflyInitC();
            return 10;
        }
    }
    int IflyTuling::languageUnderstanding(const std::string& ask,std::vector<std::string>& answer)
    {
        std::unique_lock<std::mutex> ul(mutex_lu_);
        LOG(INFO)<<"languageUnderstanding......";
        int ret=languageUnderstandingCpp(ask,answer);
        if(ret)
            LOG(ERROR)<<"languageUnderstanding failed. erroecode:"<<ret;
        else 
            LOG(INFO)<<"languageUnderstanding succeed";   
        return ret;
    }
    int IflyTuling::speechSynthesizer(const std::string& text,const std::string& speech)
    {
        std::unique_lock<std::mutex> ul(mutex_ss_);
        LOG(INFO)<<"speechSynthesizer......";
        int ret=speechSynthesizerC(text.c_str(),speech.c_str());
        if(ret)
            LOG(ERROR)<<"speechSynthesizer failed. erroecode:"<<ret;
        else
            LOG(INFO)<<"speechSynthesizer succeed";
        return ret;
    }
    int IflyTuling::speechBroadcasting(const std::string& filename)
    {
        std::unique_lock<std::mutex> ul(mutex_sb_);
        LOG(INFO)<<"speechBroadcasting......";
        int ret=speechBroadcastingC(filename.c_str());
        if(ret)
            LOG(WARNING)<<"speechBroadcasting failed, erroecode:"<<ret;
        else
            LOG(INFO)<<"speechBroadcasting succeed";
        return ret;
    }
    int IflyTuling::speechBreaking()
    {
        LOG(INFO)<<"speechBreaking......";
        int ret=speechBreakingC();
        if(ret)
            LOG(WARNING)<<"speechBreaking failed";
        else
            LOG(INFO)<<"speechBreaking succeed";
        return ret;
    }
    int IflyTuling::askFilter(std::string& ask)
    {
        if(ask.size()<=2*3) return 1;
        if(abs(ask.find("，",0)-ask.find("。",0))==3) return 0;
        if(ask.find("，",0)!=std::string::npos&&
                ask.find("。",0)!=std::string::npos&&
                ask.size()<=4*3)
            return 1;
        return 0;
    }
}

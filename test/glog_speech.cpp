#include "glog_speech.h"
namespace speech_interaction
{
    void glogInit(std::string& ns)
    {
        google::InitGoogleLogging(ns.c_str());
        std::string path=std::string(getenv("HOME"))+"/log_speech";
        if(access(path.c_str(),0)!=0)
            mkdir(path.c_str(),0777);
        FLAGS_log_dir=path;
        FLAGS_max_log_size=100;
        FLAGS_minloglevel=0;
        FLAGS_alsologtostderr=1;
    }
    void glogShutdown()
    {
        google::ShutdownGoogleLogging();
    }
}

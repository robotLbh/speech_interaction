#ifndef GLOG_SPEECH_H_
#define GLOG_SPEECH_H_ 
#include <glog/logging.h>
#include <sys/stat.h>
namespace speech_interaction
{
    void glogInit(std::string& ns);
    void glogShutdown();
}

#endif

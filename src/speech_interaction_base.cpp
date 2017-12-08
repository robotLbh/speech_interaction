#include <glog/logging.h>
#include "speech_interaction_base.h"
#include "speech_interaction_strategy.h"
#include <sstream>
#include <iostream>
#include <boost/bind.hpp>
#include <sys/time.h>
namespace speech_interaction
{
    SpeechInteractionBase::SpeechInteractionBase(Sp vis)
        :strategy_(vis),control_(0),state_(stopped),stop_thread_(0)
    {
        thread_si_=std::make_shared<std::thread>(
                boost::bind(&SpeechInteractionBase::threadSI,this));
        thread_si_->detach();
        thread_sb_=std::make_shared<std::thread>(
                boost::bind(&SpeechInteractionBase::threadSB,this));
        thread_sb_->detach();
    }
    SpeechInteractionBase::~SpeechInteractionBase()
    {
        control_=0;
        stop_thread_=1;
        speechBreaking();
        strategy_->sessionEnd(0);
        sleep(1);
    }
    int SpeechInteractionBase::startSpeechInteraction(std::string ask)
    {
        std::unique_lock<std::mutex> ul_s(mutex_ss_);
        if(control_!=1){
            control_=1;
            changeState(running);
            if(ask!="noask"){
                askToPlay(ask);
                sleep(1);
            }
        }else{
            control_=1;
            changeState(running);
        }
        cv_ss_.notify_one();
        return 0;
    }
    int SpeechInteractionBase::stopSpeechInteraction(void)
    {
        std::unique_lock<std::mutex> ul_s(mutex_ss_);
        control_=0;
        strategy_->sessionEnd(1);
        strategy_->sessionEnd(2);
        speechBreaking();
        changeState(stopped);
        return 0;
    }
    int SpeechInteractionBase::textToPlay(const std::string& text,int type)
    {
        std::string speech=randFilename();
        int ret=strategy_->speechSynthesizer(text,speech);
        if(ret>0) return ret;
        return speechBroadcasting(speech,type);
    }
    int SpeechInteractionBase::askToPlay(const std::string& ask)
    {
        std::vector<std::string> answer;
        int ret=strategy_->languageUnderstanding(ask,answer);
        if (ret) return 10;
        return textToPlay(answer[0]);
    }
    int SpeechInteractionBase::textToSpeech(const std::string& text,const std::string& speech)
    {
        return strategy_->speechSynthesizer(text,speech);
    }
    int SpeechInteractionBase::speechBroadcasting(const std::string &speech,int type)
    {
        if(type==2)
            speechBreaking();
        std::unique_lock<std::mutex> ul(mutex_playlist_);
        switch (type){
            case 2:
                if(playlist_.empty()){
                    playlist_.push_back(speech);
                }else{
                    playlist_.insert(playlist_.begin(),speech);
                }
                strategy_->sessionEnd(1);
                break;
            case 1:
                if(playlist_.empty()){
                    playlist_.push_back(speech);
                }else{
                    playlist_.insert(playlist_.begin(),speech);
                }
                break;
            default:
                playlist_.push_back(speech);
                break;
        }
        cv_sb_.notify_one();
        return 0;
    }
    int SpeechInteractionBase::speechBreaking()
    {
        changeState(interrupted);
        std::unique_lock<std::mutex> ul_b(mutex_playlist_);
        while(!playlist_.empty()){
            if(playlist_.front().find("##",0)!=std::string::npos)
                remove(playlist_.front().c_str());
            playlist_.pop_back();
        }
        return strategy_->speechBreaking();
    }
    State SpeechInteractionBase::getState()
    {
        return state_;
    }
    int SpeechInteractionBase::threadSI()
    {
        std::string ask,speech;
        std::vector<std::string> answer;
        int ret;
        while(!stop_thread_){
            while(control_==1){
                {
                    std::unique_lock<std::mutex> ul_b(mutex_BandR_);
                    changeState(SR);
                    ret=strategy_->speechRecognition(ask);
                }
                // if(sched_yield()==-1){
                //     LOG(INFO)<<"sched_yield() failed,usleep(10000)";
                //     usleep(10000);
                // }
                usleep(1000);
                if(ret||control_==0) continue;
                changeState(LU);
                ret=strategy_->languageUnderstanding(ask,answer);
                if(ret||control_==0) continue;
                changeState(CR);
                if(cmdRecognition(answer)){
                    std::unique_lock<std::mutex> ul_s(mutex_ss_);
                    control_=2;
                }
                changeState(SS);
                speech=randFilename();
                ret=strategy_->speechSynthesizer(answer[0],speech);
                if(ret||control_==0) continue;
                changeState(SB);
                ret=speechBroadcasting(speech);
                sleep(1);
            }
            changeState(stopped);
            if(control_==2) control_=0;
            LOG(INFO)<<"sleeping......";
            std::unique_lock<std::mutex> ul_s(mutex_ss_);
            while(control_==0)
                cv_ss_.wait(ul_s);
        }
        return 0;
    }
    int SpeechInteractionBase::threadSB()
    {
        while(!stop_thread_){
            std::unique_lock<std::mutex> ul_s(mutex_sb_);                   
            cv_sb_.wait(ul_s);
            //strategy_->sessionEnd(1);
            std::unique_lock<std::mutex> ul_b(mutex_BandR_);
            while(!playlist_.empty()){                                                  
                changeState(SB);
                std::string now;                                      
                {                                                                       
                    std::unique_lock<std::mutex> ul(mutex_playlist_);                   
                    now=playlist_.front();                                      
                    playlist_.pop_back();                                                    
                }                                                                       
                strategy_->speechBroadcasting(now);
                if(now.find("##",0)!=std::string::npos)
                    remove(now.c_str());
            }                                                                           
            changeState(finished);
        }
        return 0; 
    }
    int SpeechInteractionBase::changeState(State state)
    {
        std::unique_lock<std::mutex> ul_state(mutex_state_);                   
        state_=state;
        stateProcessing(state_);
        return 0;
    }
    int SpeechInteractionBase::cmdRecognition(const std::vector<std::string>& cmd)
    {
        //TODO:控制指令识别处理接口。
        return 0;
    }
    int SpeechInteractionBase::stateProcessing(State state)
    {
        //TODO:交互状态改变是回调此函数。
    }
    std::string SpeechInteractionBase::randFilename()
    {
        int rand_i=rand()%100000;
        std::stringstream ss;
        ss<<rand_i;
        std::string rand_s=ss.str();
        std::string speech=FLAGS_log_dir+"/##"+rand_s+".wav";
        return speech;
    }
}

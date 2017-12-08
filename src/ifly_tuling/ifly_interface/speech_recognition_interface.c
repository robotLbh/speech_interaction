/*
* 语音听写(iFly Auto Transform)技术能够实时地将语音转换成对应的文字。
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include "speech_recognizer.h"
#include "../ifly_tuling_params.h"

#define FRAME_LEN	640 
#define	BUFFER_SIZE	4096
params_t params_;
int errcode; //麦克风异常
int is_end=0;//0 识别中，1 识别成功，2 识别失败或没人说话。
int interruption=0;
char* txt=NULL;
/* Upload User words */
static int upload_userwords()
{
	char*			userwords	=	NULL;
	size_t			len			=	0;
	size_t			read_len	=	0;
	FILE*			fp			=	NULL;
	int				ret			=	-1;

	fp = fopen("userwords.txt", "rb");
	if (NULL == fp)										
	{
		printf("\nopen [userwords.txt] failed! \n");
		goto upload_exit;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp); 
	fseek(fp, 0, SEEK_SET);  					
	
	userwords = (char*)malloc(len + 1);
	if (NULL == userwords)
	{
		printf("\nout of memory! \n");
		goto upload_exit;
	}

	read_len = fread((void*)userwords, 1, len, fp); 
	if (read_len != len)
	{
		printf("\nread [userwords.txt] failed!\n");
		goto upload_exit;
	}
	userwords[len] = '\0';
	
	MSPUploadData("userwords", userwords, len, "sub = uup, dtt = userword", &ret); //ÉÏ´«ÓÃ»§´Ê±í
	if (MSP_SUCCESS != ret)
	{
		printf("\nMSPUploadData failed ! errorCode: %d \n", ret);
		goto upload_exit;
	}
	
upload_exit:
	if (NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}	
	if (NULL != userwords)
	{
		free(userwords);
		userwords = NULL;
	}
	
	return ret;
}


static void show_result(char *string, char is_over)
{
	//printf("\rResult: [ %s ]", string);
    txt=string;
    if(is_over)
        putchar('\n');
}

static char *g_result = NULL;
static unsigned int g_buffersize = BUFFER_SIZE;

void on_result(const char *result, char is_last)
{
    if (result) {
        size_t left = g_buffersize - 1 - strlen(g_result);
        size_t size = strlen(result);
        if (left < size) {
            g_result = (char*)realloc(g_result, g_buffersize + BUFFER_SIZE);
            if (g_result)
                g_buffersize += BUFFER_SIZE;
            else {
                printf("mem alloc failed\n");
                return;
            }
        }
        strncat(g_result, result, size);
        show_result(g_result, is_last);
    }
}
void on_speech_begin()
{
    if (g_result)
    {
        free(g_result);
    }
    g_result = (char*)malloc(BUFFER_SIZE);
    g_buffersize = BUFFER_SIZE;
    memset(g_result, 0, g_buffersize);

    //printf("Start Listening...\n");
}
void on_speech_end(int reason)
{
    if (reason == END_REASON_VAD_DETECT){
       // printf("Speaking done \n");
        is_end=1;
    }
    else{
        printf("\nRecognizer error %d\n", reason);
        is_end=2;
    }
}

/* demo send audio data from a file */
static void demo_file(const char* audio_file, const char* session_begin_params)
{
    int	errcode = 0;
    FILE*	f_pcm = NULL;
    char*	p_pcm = NULL;
    unsigned long	pcm_count = 0;
    unsigned long	pcm_size = 0;
    unsigned long	read_size = 0;
    struct speech_rec iat;
    struct speech_rec_notifier recnotifier = {
        on_result,
        on_speech_begin,
        on_speech_end
    };

    if (NULL == audio_file)
        goto iat_exit;

    f_pcm = fopen(audio_file, "rb");
    if (NULL == f_pcm)
    {
        printf("\nopen [%s] failed! \n", audio_file);
        goto iat_exit;
    }

    fseek(f_pcm, 0, SEEK_END);
    pcm_size = ftell(f_pcm);
    fseek(f_pcm, 0, SEEK_SET);

    p_pcm = (char *)malloc(pcm_size);
    if (NULL == p_pcm)
    {
        printf("\nout of memory! \n");
        goto iat_exit;
    }

    read_size = fread((void *)p_pcm, 1, pcm_size, f_pcm);
    if (read_size != pcm_size)
    {
        printf("\nread [%s] error!\n", audio_file);
        goto iat_exit;
    }

    errcode = sr_init(&iat, session_begin_params, SR_USER, &recnotifier);
    if (errcode) {
        printf("speech recognizer init failed : %d\n", errcode);
        goto iat_exit;
    }

    errcode = sr_start_listening(&iat);
    if (errcode) {
        printf("\nsr_start_listening failed! error code:%d\n", errcode);
        goto iat_exit;
    }

    while (1)
    {
        unsigned int len = 10 * FRAME_LEN; /* 200ms audio */
        int ret = 0;

        if (pcm_size < 2 * len)
            len = pcm_size;
        if (len <= 0)
            break;

        ret = sr_write_audio_data(&iat, &p_pcm[pcm_count], len);

        if (0 != ret)
        {
            printf("\nwrite audio data failed! error code:%d\n", ret);
            goto iat_exit;
        }

        pcm_count += (long)len;
        pcm_size -= (long)len;		
    }

    errcode = sr_stop_listening(&iat);
    if (errcode) {
        printf("\nsr_stop_listening failed! error code:%d \n", errcode);
        goto iat_exit;
    }

iat_exit:
    if (NULL != f_pcm)
    {
        fclose(f_pcm);
        f_pcm = NULL;
    }
    if (NULL != p_pcm)
    {
        free(p_pcm);
        p_pcm = NULL;
    }

    sr_stop_listening(&iat);
    sr_uninit(&iat);
}

/* demo recognize the audio from microphone */
static int demo_mic(const char* session_begin_params)
{
    int errcode;
    int i = 0;

    struct speech_rec iat;

    struct speech_rec_notifier recnotifier = {
        on_result,
        on_speech_begin,
        on_speech_end
    };

    errcode = sr_init(&iat, session_begin_params, SR_MIC, &recnotifier);
    if (errcode) {
        printf("speech recognizer init failed\n");
        return errcode;
    }
    errcode = sr_start_listening(&iat);
    if (errcode) {
        printf("start listen failed %d\n", errcode);
        return errcode;
    }
    while(iat.state==1&&i++ < 20000){
        //printf("start listen : %d(s)\n", i);
        usleep(1000);
        if(interruption)
            break;
    }
    //usleep(100);
    errcode = sr_stop_listening(&iat);
    if (errcode) {
        printf("stop listening failed %d\n", errcode);
        is_end=2;
    }

    sr_uninit(&iat);
    return errcode;
}

int iflyInitC()
{
    int ret = MSP_SUCCESS;
    ret = MSPLogin(NULL, NULL, params_.appid_ifly);
    if (MSP_SUCCESS != ret)	{
        printf("MSPLogin failed , Error code %d.\n",ret);
        MSPLogout(); 
        return ret;
    }
    // printf("MSPLogin successed\n");
    return 0;
}
int iflyLogout()
{
    return MSPLogout();
}
int speechRecognitionC(char* text)
{
    is_end=0;
    interruption=0;
    int ret=demo_mic(params_.params_sr);
    if(is_end==2||is_end==0){
        if(ret)
            return 10;
        else 
            return ret;
    }
    strcpy(text,g_result);
    return 0;
}
int speechRecognitionEndC()
{
    interruption=1;
    return 0;
}

#include "../ifly_tuling_params.h"
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <locale.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <alsa/asoundlib.h>
#include <assert.h>
#include "wav_parser.h"
#include "sndwav_common.h"

static int is_break_play=0;

ssize_t SNDWAV_P_SaveRead(int fd, void *buf, size_t count) {
	ssize_t result = 0, res;

	while (count > 0) {
		if ((res = read(fd, buf, count)) == 0)
			break;
		if (res < 0)
			return result > 0 ? result : res;
		count -= res;
		result += res;
		buf = (char *) buf + res;
	}
	return result;
}

void SNDWAV_Play(SNDPCMContainer_t *sndpcm, WAVContainer_t *wav, int fd) {
	int load, ret;
	off64_t written = 0;
	off64_t c;
	off64_t count = LE_INT(wav->chunk.length);

	load = 0;
	while (written < count) {
		/* Must read [chunk_bytes] bytes data enough. */
		if(is_break_play==1){
			return;
		}


		do {
			c = count - written;
			if (c > sndpcm->chunk_bytes)
				c = sndpcm->chunk_bytes;
			c -= load;

			if (c == 0)
				break;
			ret = SNDWAV_P_SaveRead(fd, sndpcm->data_buf + load, c);
			if (ret < 0) {
				fprintf(stderr, "Error safe_read/n");
				exit(-1);
			}
			if (ret == 0)
				break;
			load += ret;
		} while ((size_t) load < sndpcm->chunk_bytes);

		/* Transfer to size frame */
		load = load * 8 / sndpcm->bits_per_frame;
		ret = SNDWAV_WritePcm(sndpcm, load);
		if (ret != load)
			break;

		ret = ret * sndpcm->bits_per_frame / 8;
		written += ret;
		load = 0;
	}
}

int speechBroadcastingC(const char* speech ){
	char *devicename = "default";
	int fd;
	WAVContainer_t wav;
	SNDPCMContainer_t playback;
	is_break_play=0;

	memset(&playback, 0x0, sizeof(playback));

	fd = open(speech, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Error open [%s]/n", speech);
		return 11;
	}
//	fseek(fd,0L,SEEK_END);
//	int file_len=ftell(fp);
//	if(file_len==0){
//		close(fd);
//		remove(speech);
//		return -1;
//	}
//	fseek(fd,0,SEEK_SET);
    int ret;
	if (WAV_ReadHeader(fd, &wav) < 0) {
		fprintf(stderr, "Error WAV_Parse [%s]/n", speech);
        ret=12;
		goto Err;
	}

	if (snd_output_stdio_attach(&playback.log, stderr, 0) < 0) {
		fprintf(stderr, "Error snd_output_stdio_attach/n");
        ret=13;
		goto Err;
	}

	if (snd_pcm_open(&playback.handle, devicename, SND_PCM_STREAM_PLAYBACK, 0)
			< 0) {
		fprintf(stderr, "Error snd_pcm_open [ %s]/n", devicename);
        ret=14;
		goto Err;
	}

	if (SNDWAV_SetParams(&playback, &wav) < 0) {
		fprintf(stderr, "Error set_snd_pcm_params/n");
        ret=15;
		goto Err;
	}
//	snd_pcm_dump(playback.handle, playback.log);

	SNDWAV_Play(&playback, &wav, fd);

	snd_pcm_drain(playback.handle);
	close(fd);
	free(playback.data_buf);
	snd_output_close(playback.log);
	snd_pcm_close(playback.handle);
	return 0;

	Err: close(fd);
	if (playback.data_buf)
		free(playback.data_buf);
	if (playback.log)
		snd_output_close(playback.log);
	if (playback.handle)
		snd_pcm_close(playback.handle);
	return ret;
}

int speechBreakingC(){
	is_break_play=1;
    return 0;
}


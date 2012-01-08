/* VJ audio mp3 startup kit.
   By Antti Silvast 2003 (asilvast@cc.hut.fi)
   */

/* buffer limit */
#define MAX_AUDIO_BUFFER_SIZE 65536

/* the audio buffer of requested size.*/
extern float audio_buffer[MAX_AUDIO_BUFFER_SIZE];

/* the time blur for audio bands.
 bar_value = time_blur*new_value + (1.0-time_blur)*old_value
 */
extern float time_blur;

/* number of bands for filters */
extern int nof_bands;
#define MAX_BANDS 256
/* the audio bars, values 0.0...1.0 */
extern float audio_bars[MAX_BANDS];

/*
   Opens audio interface. filename is the mp3 to be played, buffer_size in bytes.
 */
int openaudio(char * filename, int buffer_size);

/* Starts sampling and analyzing */
int startaudio();

/* Closes audio interface */
int closeaudio();

/* sets band i to correspond to frequencies low...high */
void set_band(int i, float cutoff);


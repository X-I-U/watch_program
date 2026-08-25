#ifndef __SPEAKER_H_
#define __SPEAKER_H_

void speaker_init(void);
void speaker_play(const int16_t *pcm, size_t samples);
void speaker_play_stream(const int16_t *pcm, size_t samples);
void speaker_play_tone(int freq_hz, int duration_ms, float amplitude);

#endif

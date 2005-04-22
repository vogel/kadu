#ifndef KADU_SOUNDFILE_H
#define KADU_SOUNDFILE_H

#include "../../kadu-stdint.h"

class SoundFile
{
	//klasa automagicznie przy pomocy libsndfile konwertuje wszystko na d¼wiêk 16 bitowy
	public:
	int length;
	int16_t *data;
	int channels;
	int speed;
	
	SoundFile(const char *path);
	~SoundFile();
	bool isOk();
	void setVolume(float vol);
	static void setVolume(int16_t *data, int length, float vol);
};
#endif

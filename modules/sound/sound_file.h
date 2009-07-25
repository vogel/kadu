#ifndef KADU_SOUNDFILE_H
#define KADU_SOUNDFILE_H

#include <QtGlobal>

/** @ingroup sound
 * @{
 */
class SoundFile
{
	//klasa automagicznie przy pomocy libsndfile konwertuje wszystko na d¼wiêk 16 bitowy
	public:
	int length;
	qint16 *data;
	int channels;
	int speed;
	
	SoundFile(const char *path);
	~SoundFile();
	bool isOk();
	void setVolume(float vol);
	static void setVolume(qint16 *data, int length, float vol);
};
/** @} */
#endif

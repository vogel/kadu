#ifndef PLAYER_EXTERNAL_H
#define PLAYER_EXTERNAL_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/phononnamespace.h>

/**
 * @defgroup phonon_sound Phonon sound
 * @{
 */
class PhononPlayer : public QObject
{
	Q_OBJECT

	Phonon::MediaObject *music;
	Phonon::AudioOutput *output;

private slots:
	void playSound(const QString &s, bool volCntrl, double vol);

public:
	PhononPlayer();
	~PhononPlayer();
};

extern PhononPlayer *phonon_player;

/** @} */

#endif

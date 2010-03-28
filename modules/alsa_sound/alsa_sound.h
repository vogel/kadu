#ifndef KADU_ALSA_SOUND_H
#define KADU_ALSA_SOUND_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API

#include <alsa/asoundlib.h>

#include "modules/sound/sound-player.h"

class ALSAPlayerSlots : public SoundPlayer
{
	Q_OBJECT

	void createDefaultConfiguration();

public:
	explicit ALSAPlayerSlots(QObject *parent = 0);
	virtual ~ALSAPlayerSlots();

	virtual bool isSimplePlayer() { return true; }

public slots:
	virtual void playSound(const QString &path, bool volumeControl, double volume);

};

extern ALSAPlayerSlots *alsa_player_slots;

#endif

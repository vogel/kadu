#ifndef PLAYER_ARTS_H
#define PLAYER_ARTS_H
#include <qobject.h>
#include <qstring.h>
#include <soundserver.h>
#include "../sound/sound.h"
#include "misc.h"

using namespace Arts;

class ArtsPlayerSlots : public QObject
{
	Q_OBJECT
		Dispatcher disp;
	public:
		ArtsPlayerSlots();
		~ArtsPlayerSlots();
		SoundServerV2 server;
	private slots:
		void playSound(const QString &s, bool volCntrl, double vol);
};

extern ArtsPlayerSlots *artsPlayerObj;

#endif


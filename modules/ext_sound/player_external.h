#ifndef PLAYER_EXTERNAL_H
#define PLAYER_EXTERNAL_H
#include <qobject.h>
#include <qstring.h>
#include "../sound/sound.h"
#include "misc.h"

class ExternalPlayerSlots : public QObject
{
	Q_OBJECT
	public:
		ExternalPlayerSlots();
		~ExternalPlayerSlots();
	private slots:
		void choosePlayerFile();
		void play(const QString &s, bool volCntrl, double vol, QString player);
		
		void playSound(const QString &s, bool volCntrl, double vol);
};

extern ExternalPlayerSlots *externalPlayerObj;

#endif


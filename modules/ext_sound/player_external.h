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
		ExternalPlayerSlots(QObject *parent=0, const char *name=0);
		~ExternalPlayerSlots();
	private slots:
		void playSound(const QString &s, bool volCntrl, double vol);
		void play(const QString &s, bool volCntrl, double vol, QString player);
		void choosePlayerFile();
};

extern ExternalPlayerSlots *external_player_slots;

#endif

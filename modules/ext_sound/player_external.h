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
	private slots:
		void choosePlayerFile();
		void play(const QString &s, bool volCntrl, double vol, QString player);
		
		void playSound(const QString &s, bool volCntrl, double vol);
		void playMessage(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playChat(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playNotify(const uin_t uin, const QString &sound, bool volCntrl, double vol);
};

extern ExternalPlayerSlots *externalPlayerObj;

#endif


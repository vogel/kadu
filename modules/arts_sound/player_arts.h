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
		SoundServerV2 server;
	private slots:
		void play(const QString &s, bool volCntrl=false, double vol=1);
		
		void playSound(const QString &s, bool volCntrl, double vol);
		void playMessage(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playChat(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playNotify(const uin_t uin, const QString &sound, bool volCntrl, double vol);
};

extern ArtsPlayerSlots *slotsObj;

#endif


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
		void play(const QString &s, bool volCntrl=false, double vol=1);
		
		void playSound(const QString &s, bool volCntrl, double vol);

		void playNewMessage(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg);
		void playNewChat(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg);
		void playConnectionError(const QString &sound, bool volCntrl, double vol, const QString &msg);

		void playNotify(const UinType uin, const QString &sound, bool volCntrl, double vol);
		
		void playMessage(const QString &sound, bool volCntrl, double vol, const QString &from, const QString &type, const QString &msg, const UserListElement *ule);
};

extern ArtsPlayerSlots *artsPlayerObj;

#endif


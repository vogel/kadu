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
		void playNewMessage(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg);
		void playNewChat(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg);
		void playConnectionError(const QString &sound, bool volCntrl, double vol, const QString &msg);

		void playNotify(const UinType uin, const QString &sound, bool volCntrl, double vol);
		
		void playMessage(const QString &sound, bool volCntrl, double vol, const QString &from, const QString &type, const QString &msg, const UserListElement *ule);
};

extern ExternalPlayerSlots *externalPlayerObj;

#endif


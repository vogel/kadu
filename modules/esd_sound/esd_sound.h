#ifndef KADU_ESD_SOUND_H
#define KADU_ESD_SOUND_H
#include <qobject.h>
#include <qstring.h>
#include "../sound/sound.h"
#include "misc.h"

class ESDPlayerSlots : public QObject
{
	Q_OBJECT
	public:
		int sock;
		ESDPlayerSlots();
		~ESDPlayerSlots();
	private slots:
		void play(const QString &s, bool volCntrl, double vol);
		
		void playSound(const QString &s, bool volCntrl, double vol);

		void playNewMessage(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg);
		void playNewChat(UinsList senders, const QString &sound, bool volCntrl, double vol, const QString &msg);
		void playConnectionError(const QString &sound, bool volCntrl, double vol, const QString &msg);

		void playNotify(const UinType uin, const QString &sound, bool volCntrl, double vol);
		
		void playMessage(const QString &sound, bool volCntrl, double vol, const QString &from, const QString &type, const QString &msg, const UserListElement *ule);
};

extern ESDPlayerSlots *esdPlayerObj;

#endif


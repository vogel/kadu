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
		void playMessage(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playChat(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playNotify(const uin_t uin, const QString &sound, bool volCntrl, double vol);
};

extern ESDPlayerSlots *esdPlayerObj;

#endif


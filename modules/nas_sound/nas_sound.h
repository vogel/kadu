#ifndef KADU_NAS_SOUND_H
#define KADU_NAS_SOUND_H
#include <qobject.h>
#include <qstring.h>
#include "../sound/sound.h"
#include "misc.h"

class NASPlayerSlots : public QObject
{
	Q_OBJECT
	public:
		NASPlayerSlots();
	private slots:
		void play(const QString &s, bool volCntrl, double vol);
		
		void playTestSound(const QString &s, bool volCntrl, double vol);
		void playMessage(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playChat(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playNotify(const uin_t uin, const QString &sound, bool volCntrl, double vol);
};

extern NASPlayerSlots *slotsObj;

#endif


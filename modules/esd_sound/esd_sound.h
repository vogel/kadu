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
		void playSound(const QString &s, bool volCntrl, double vol);
};

extern ESDPlayerSlots *esdPlayerObj;

#endif


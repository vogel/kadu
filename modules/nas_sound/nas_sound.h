#ifndef KADU_NAS_SOUND_H
#define KADU_NAS_SOUND_H
#include <qobject.h>
#include <qstring.h>
#include <qglobal.h>

#include "../sound/sound.h"
#include "misc.h"

#ifdef Q_OS_WIN
#define INTERNAL_QT_SOUND_SUPPORT
#endif

#ifndef INTERNAL_QT_SOUND_SUPPORT
#include <qsocketnotifier.h>
namespace audiolib {
#include <audiolib.h>
#include <soundlib.h>
}
#endif


class NASPlayerSlots : public QObject
{
	Q_OBJECT

#ifndef INTERNAL_QT_SOUND_SUPPORT
	audiolib::AuServer *auserver;
	QSocketNotifier* sn;
#endif

	public:
		NASPlayerSlots();
		~NASPlayerSlots();
		bool isConnected();
	private slots:
		void dataReceived();
		
		void playSound(const QString &s, bool volCntrl, double vol);
		void playMessage(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playChat(UinsList senders, const QString &sound, const QString &msg, bool volCntrl, double vol);
		void playNotify(const uin_t uin, const QString &sound, bool volCntrl, double vol);
};

extern NASPlayerSlots *slotsObj;

#endif


#ifndef KADU_NAS_SOUND_H
#define KADU_NAS_SOUND_H
#include <qobject.h>
#include <qstring.h>
#include <qglobal.h>
/**
 * @defgroup nas_sound NAS sound
 * @{
 */
#ifdef Q_OS_WIN
#define INTERNAL_QT_SOUND_SUPPORT
#endif

#ifndef INTERNAL_QT_SOUND_SUPPORT
#include <qsocketnotifier.h>
namespace audiolib {
#include <audio/audiolib.h>
#include <audio/soundlib.h>
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
		NASPlayerSlots(QObject *parent=0, const char *name=0);
		~NASPlayerSlots();
		bool isConnected();
	private slots:
		void dataReceived();
		
		void playSound(const QString &s, bool volCntrl, double vol);
};

extern NASPlayerSlots *nas_player_slots;

/** @} */

#endif

#ifndef PLAYER_ARTS_H
#define PLAYER_ARTS_H
#include <qobject.h>
#include <qstring.h>
#include <soundserver.h>

using namespace Arts;

class aRtsPlayerSlots : public QObject
{
	Q_OBJECT
		Dispatcher disp;
	public:
		aRtsPlayerSlots(QObject *parent=0, const char *name=0);
		~aRtsPlayerSlots();
		SoundServerV2 server;
	private slots:
		void playSound(const QString &s, bool volCntrl, double vol);
};

extern aRtsPlayerSlots *arts_player_slots;

#endif


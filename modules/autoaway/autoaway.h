#ifndef AUTOAWAY_H
#define AUTOAWAY_H

#include <qtimer.h>
#include <qobject.h>
#include <qtimer.h>
#include <qevent.h>

#include "gadu.h"

class AutoAwaySlots :public QObject
{
	Q_OBJECT
	public:
	AutoAwaySlots();
	~AutoAwaySlots();

	public slots:
		void onCreateConfigDialog();
		void onApplyConfigDialog();
		void on();
		void off();
		void changeAutoInvisibleTime(int i);
		void changeAutoDisconnectTime(int i);
		void correctAutoDisconnectTime(int i);
		void checkAutoInvisibleTime(bool b);
		void checkAutoDisconnectTime(bool b);
		void setTextLength(const QString &str);
};

class AutoAwayTimer : private QTimer
{
	Q_OBJECT

	private slots:
		void checkIdleTime();

	protected:
		bool eventFilter(QObject *, QEvent *);

	private:
		AutoAwayTimer(QObject *parent = 0);
		bool autoawayed;
		bool autoinvisibled;		/* zapamietuje czy przeszedl na autoinvisible - potrzebne do autodisconnect */
		bool autodisconnected;		/* zapamietuje czy rozlaczylismy sie juz */
		bool autodescription;		/* zapamietuje czy juz zmienil opis */
		GaduStatus beforeAutoAway;
		int idletime;
		friend class AutoAwaySlots;
};

#endif

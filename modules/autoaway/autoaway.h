#ifndef AUTOAWAY_H
#define AUTOAWAY_H

#include <qtimer.h>
#include <qobject.h>
#include <qtimer.h>
#include <qevent.h>

class AutoAwaySlots :public QObject
{
	Q_OBJECT
	public:
	AutoAwaySlots();
	
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
		int beforeAutoAway;
		int idletime;
		friend class AutoAwaySlots;
};

#endif

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
		int beforeAutoAway;
		int idletime;
		friend class AutoAwaySlots;
};

#endif

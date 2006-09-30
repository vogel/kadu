#ifndef AUTOAWAY_H
#define AUTOAWAY_H

#include <qtimer.h>
#include <qobject.h>
#include <qtimer.h>
#include <qevent.h>

#include "gadu.h"
/**
 * @defgroup autoaway Autoaway
 * @{
 */
class AutoAwaySlots : public QObject
{
	Q_OBJECT
	public:
		AutoAwaySlots(QObject *parent=0, const char *name=0);
		~AutoAwaySlots();

	public slots:
		void onCreateTabGeneral();
		void onApplyTabGeneral();
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
		friend class AutoAwaySlots;
			
		AutoAwayTimer(QObject *parent = 0, const char *name=0);
		QString changeDescription(const QString &oldDescription);
		
		bool didChangeStatus;
		bool didChangeDescription;

		enum DescAction {NOTHING=0, REPLACE=1, PREPEND=2, APPEND=3} action;
		QString actionText;
		int checkInterval;

		int autoAwayTime;
		int autoDisconnectTime;
		int autoInvisibleTime;

		bool autoAwayEnabled;
		bool autoInvisibleEnabled;
		bool autoDisconnectEnabled;
		
		bool restoreStatus;

		GaduStatus oldStatus;
		int idleTime;
};

/** @} */

#endif

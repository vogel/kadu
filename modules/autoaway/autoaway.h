#ifndef AUTOAWAY_H
#define AUTOAWAY_H

#include <qtimer.h>
#include <qobject.h>
#include <qtimer.h>
#include <qevent.h>

#include "status_changer.h"
#include "gadu.h"

/**
 * @defgroup autoaway Autoaway
 * @{
 */
class AutoAwayStatusChanger : public StatusChanger
{
	Q_OBJECT

public:
	enum ChangeStatusTo {
		NoChangeStatus,
		ChangeStatusToBusy,
		ChangeStatusToInvisible,
		ChangeStatusToOffline
	};

	enum ChangeDescriptionTo {
		NoChangeDescription,
		ChangeDescriptionReplace,
		ChangeDescriptionPrepend,
		ChangeDescriptionAppend
	};

private:
	ChangeStatusTo changeStatusTo;
	ChangeDescriptionTo changeDescriptionTo;
	QString descriptionAddon;

public:
	AutoAwayStatusChanger();
	virtual ~AutoAwayStatusChanger();

	virtual void changeStatus(UserStatus &status);

	void setChangeStatusTo(ChangeStatusTo newChangeStatusTo);
	void setChangeDescriptionTo(ChangeDescriptionTo newChangeDescriptionTo, const QString &newDescriptionAddon);

};

class AutoAwaySlots : public QObject
{
	Q_OBJECT

	AutoAwayStatusChanger *autoAwayStatusChanger;

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

		AutoAwayTimer(AutoAwayStatusChanger *autoAwayStatusChanger, QObject *parent = 0, const char *name=0);
		QString changeDescription(const QString &oldDescription);

		AutoAwayStatusChanger *autoAwayStatusChanger;

		int checkInterval;

		int autoAwayTime;
		int autoDisconnectTime;
		int autoInvisibleTime;

		bool autoAwayEnabled;
		bool autoInvisibleEnabled;
		bool autoDisconnectEnabled;

		GaduStatus oldStatus;
		int idleTime;
};

/** @} */

#endif

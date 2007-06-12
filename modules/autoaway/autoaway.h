#ifndef AUTOAWAY_H
#define AUTOAWAY_H

#include <qtimer.h>
#include <qobject.h>
#include <qtimer.h>
#include <qevent.h>

#include "main_configuration_window.h"
#include "status_changer.h"
#include "gadu.h"

class QLineEdit;
class QSpinBox;

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

class AutoAway : public ConfigurationUiHandler
{
	Q_OBJECT

	AutoAwayStatusChanger *autoAwayStatusChanger;
	QTimer *timer;

	unsigned int checkInterval;

	unsigned int autoAwayTime;
	unsigned int autoDisconnectTime;
	unsigned int autoInvisibleTime;

	bool autoAwayEnabled;
	bool autoInvisibleEnabled;
	bool autoDisconnectEnabled;

	unsigned int idleTime;

	QSpinBox *autoAwaySpinBox;
	QSpinBox *autoInvisibleSpinBox;
	QSpinBox *autoOfflineSpinBox;

	QLineEdit *descriptionTextLineEdit;

private slots:
	void checkIdleTime();
	void configurationUpdated();

	void autoAwaySpinBoxValueChanged(int value);
	void autoInvisibleSpinBoxValueChanged(int value);
	void autoOfflineSpinBoxValueChanged(int value);

	void descriptionChangeChanged(int index);

protected:
	bool eventFilter(QObject *, QEvent *);

public:
	AutoAway(QObject *parent = 0, const char *name=0);
	virtual ~AutoAway();

	QString changeDescription(const QString &oldDescription);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

public slots:
	void on();
	void off();

};

/** @} */

#endif

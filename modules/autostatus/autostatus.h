#ifndef AUTOSTATUS_H
#define AUTOSTATUS_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtGui/QDialog>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"
#include "action.h"

class QTimer;
class PowerStatusChanger;

/*
class AutostatusChanger : public StatusChanger
{
	Q_OBJECT

	friend class AutoAway;

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
	AutostatusChanger();
	virtual ~AutostatusChanger();

	virtual void changeStatus(UserStatus &status);

	void setChangeStatusTo(ChangeStatusTo newChangeStatusTo);
	void setChangeDescriptionTo(ChangeDescriptionTo newChangeDescriptionTo, const QString &newDescriptionAddon);

};
*/
/*!
 * This class provides functionality to automaticly change status
 * after served time
 * \brief Automaticly status change class
 */
class Autostatus : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	public:
		//! Default constructor.
		Autostatus();

	//! Default destructor.
		~Autostatus();

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
 
	private:
		PowerStatusChanger *powerStatusChanger;

		void addDefaultConfiguration();
		void on();
		void off();
		bool enabled;

		int autoStatus;
		int autoTime;
		int currStat;
		QString currDesc;

		QStringList::Iterator it;
		QTimer* timer;
		QStringList statusList;
		ActionDescription *autostatusActionDescription;
	
	public slots:
		//! This slot is called when new Autostatus it starts
		void onAutostatus(QAction *, bool);

		//! This slot is called when timeout
		void changeStatus();

	protected:
		virtual void configurationUpdated();
};

#endif

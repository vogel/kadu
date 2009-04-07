#ifndef FILEDESC_H
#define FILEDESC_H

#include <QtCore/QObject>

#include "main_configuration_window.h"
#include "status_changer.h"

class QTimer;

class FileDescStatusChanger : public StatusChanger
{
	Q_OBJECT

	QString title;
	bool disabled;

public:
	FileDescStatusChanger();
	virtual ~FileDescStatusChanger();

	virtual void changeStatus(UserStatus &status);

	void setTitle(const QString &newTitle);
	void disable();

};

class FileDescription : public ConfigurationUiHandler
{
	Q_OBJECT

private:
	QTimer* timer;
	QString currDesc;

	FileDescStatusChanger *fileDescStatusChanger;

	void createDefaultConfiguration();

public:
	FileDescription();
	virtual ~FileDescription();
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow){};

public slots:
	void checkTitle();

};

#endif

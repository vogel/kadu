#ifndef FILEDESC_H
#define FILEDESC_H

#include <QtCore/QObject>

#include "gui/windows/main-configuration-window.h"
#include "status/status-changer.h"

class QTimer;

class FileDescription;

class FileDescStatusChanger : public StatusChanger
{
	Q_OBJECT

	FileDescription *Parent;
	QString Title;

public:
	explicit FileDescStatusChanger(FileDescription *parent = 0, QObject *parentObj = 0);
	virtual ~FileDescStatusChanger();

	virtual void changeStatus(StatusContainer *container, Status &status);

	void setTitle(const QString &title);

};

class FileDescription : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

	QTimer *Timer;

	QString File;
	bool AllowOther;
	bool ForceDesc;

	FileDescStatusChanger *StatusChanger;

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

public:
	explicit FileDescription(QObject *parent = 0);
	virtual ~FileDescription();

	bool allowOther() const { return AllowOther; }
	bool forceDesc() const { return ForceDesc; }

public slots:
	void checkTitle();

};

#endif

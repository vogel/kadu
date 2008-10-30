/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QT4_DOCKING_NOTIFY_CONFIGURATION_WIDGET_H
#define QT4_DOCKING_NOTIFY_CONFIGURATION_WIDGET_H

#include "../notify/notify.h"

class QLabel;
class QLineEdit;
class QSpinBox;
class QComboBox;

struct Qt4NotifyProperties
{
	QString eventName;

	unsigned int icon;
	unsigned int timeout;
	QString syntax;
	QString title;
};

class Qt4NotifyConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QMap<QString, Qt4NotifyProperties> properties;
	Qt4NotifyProperties currentProperties;
	QString currentNotifyEvent;

	QSpinBox *timeout;
	QLineEdit *syntax;
	QLineEdit *title;
	QComboBox *icon;

private slots:
	void timeoutChanged(int timeout);
	void syntaxChanged(const QString &syntax);
	void titleChanged(const QString &title);
	void iconChanged(int index);

public:
	Qt4NotifyConfigurationWidget(QWidget *parent = 0, char *name = 0);

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();

	virtual void switchToEvent(const QString &event);
};

#endif

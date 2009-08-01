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

#include "gui/widgets/configuration/notifier-configuration-widget.h"

class ConfigurationWindow;

class Qt4NotifyConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QString currentNotifyEvent;

private slots:

	void showConfigurationWindow();

public:
	Qt4NotifyConfigurationWidget(QWidget *parent = 0);

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();

	virtual void switchToEvent(const QString &event);
};

#endif // QT4_DOCKING_NOTIFY_CONFIGURATION_WIDGET_H

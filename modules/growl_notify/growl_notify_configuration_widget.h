/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROWL_NOTIFY_CONFIGURATION_WIDGET_H
#define GROWL_NOTIFY_CONFIGURATION_WIDGET_H

#include "../notify/notify.h"

class QLabel;
class QLineEdit;
class QSpinBox;
class QComboBox;

struct GrowlNotifyProperties
{
	QString eventName;
	QString syntax;
	QString title;
};

class GrowlNotifyConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QMap<QString, GrowlNotifyProperties> properties;
	GrowlNotifyProperties currentProperties;
	QString currentNotifyEvent;

	QLineEdit *syntax;
	QLineEdit *title;

private slots:
	void syntaxChanged(const QString &syntax);
	void titleChanged(const QString &title);

public:
	GrowlNotifyConfigurationWidget(QWidget *parent = 0, char *name = 0);

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();

	virtual void switchToEvent(const QString &event);
};

#endif

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HINTS_CONFIGURATION_WIDGET_H
#define HINTS_CONFIGURATION_WIDGET_H

#include "gui/widgets/configuration/notifier-configuration-widget.h"

class QLabel;

class HintsConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QString currentNotifyEvent;

	QLabel *preview;

private slots:
	void showConfigurationWindow();
	void updatePreview();

public:
	HintsConfigurationWidget(QWidget *parent = 0);

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();

	virtual void switchToEvent(const QString &event);

};

#endif // HINTS_CONFIGURATION_WIDGET_H

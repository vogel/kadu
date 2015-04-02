/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HINTS_CONFIGURATION_WIDGET_H
#define HINTS_CONFIGURATION_WIDGET_H

#include "gui/widgets/configuration/notifier-configuration-widget.h"

class QLabel;

class HintsConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QString currentNotificationEvent;

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

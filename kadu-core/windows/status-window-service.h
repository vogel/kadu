/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;
class InjectedFactory;
class StatusContainer;
class StatusWindow;

class StatusWindowService : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit StatusWindowService(QObject *parent = nullptr);
	virtual ~StatusWindowService();

	/**
	 * @short Shows new status window.
	 * @param statusContainer status container
	 * @param parent dialog's parent widget, null by default
	 * @return Pointer to the created StatusWindow dialog.
	 *
	 * Creates and shows new status window.
	 */
	StatusWindow * showDialog(StatusContainer *statusContainer, QWidget *parent = nullptr);

private:
	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;
	QMap<StatusContainer *, StatusWindow *> m_dialogs;

	StatusWindow * getDialog(StatusContainer *statusContainer, QWidget *parent = nullptr);

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);

	void statusWindowClosed(StatusContainer *statusContainer);

};

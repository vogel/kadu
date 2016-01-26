/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "status/status-container-aware-object.h"

#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtWidgets/QToolBar>
#include <injeqt/injeqt.h>

class InjectedFactory;
class StatusButton;
class StatusConfigurationHolder;
class StatusContainerManager;
class StatusContainer;

class KADUAPI StatusButtons : public QToolBar, private StatusContainerAwareObject
{
	Q_OBJECT

public:
	explicit StatusButtons(QWidget *parent = nullptr);
	virtual ~StatusButtons();

protected:
	virtual void statusContainerRegistered(StatusContainer *statusContainer);
	virtual void statusContainerUnregistered(StatusContainer *statusContainer);

private:
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<StatusConfigurationHolder> m_statusConfigurationHolder;
	QPointer<StatusContainerManager> m_statusContainerManager;

	QMap<StatusContainer *, StatusButton *> Buttons;

	void enableStatusName();
	void disableStatusName();

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setStatusConfigurationHolder(StatusConfigurationHolder *statusConfigurationHolder);
	INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
	INJEQT_INIT void init();

};

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

#include "gui/actions/action-description.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatWidgetManager;
class ChatWidgetRepository;
class Configuration;

class OpenInNewTabAction : public ActionDescription
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit OpenInNewTabAction(QObject *parent = nullptr);
	virtual ~OpenInNewTabAction();

protected:
	virtual void actionTriggered(QAction *sender, bool toggled) override;
	virtual void updateActionState(Action *action) override;

private:
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QPointer<Configuration> m_configuration;

private slots:
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_INIT void init();

};

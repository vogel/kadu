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

#include "open-forum-action.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "os/generic/url-opener.h"

OpenForumAction::OpenForumAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setName(QStringLiteral("forumAction"));
	setText(tr("Forum"));
	setType(ActionDescription::TypeMainMenu);
}

OpenForumAction::~OpenForumAction()
{
}

void OpenForumAction::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void OpenForumAction::setUrlOpener(UrlOpener *urlOpener)
{
	m_urlOpener = urlOpener;
}

void OpenForumAction::actionTriggered(QAction *, bool)
{
	if (m_configuration->deprecatedApi()->readEntry("General", "Language") == "pl")
		m_urlOpener->openUrl("http://www.kadu.im/forum/");
	else
		m_urlOpener->openUrl("http://www.kadu.im/forum/viewforum.php?f=12");
}

#include "moc_open-forum-action.cpp"

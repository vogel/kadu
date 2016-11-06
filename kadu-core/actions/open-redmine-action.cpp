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

#include "open-redmine-action.h"

#include "os/generic/url-opener.h"

OpenRedmineAction::OpenRedmineAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"kadu_icons/report-a-bug"});
	setName(QStringLiteral("bugsAction"));
	setText(tr("Report a Bug"));
	setType(ActionDescription::TypeMainMenu);
}

OpenRedmineAction::~OpenRedmineAction()
{
}

void OpenRedmineAction::setUrlOpener(UrlOpener *urlOpener)
{
	m_urlOpener = urlOpener;
}

void OpenRedmineAction::init()
{
	registerAction(actionsRegistry());
}

void OpenRedmineAction::actionTriggered(QAction *, bool)
{
	m_urlOpener->openUrl("http://www.kadu.im/redmine/projects/kadu");
}

#include "moc_open-redmine-action.cpp"

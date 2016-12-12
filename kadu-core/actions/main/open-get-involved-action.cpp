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

#include "open-get-involved-action.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "os/generic/url-opener.h"

OpenGetInvolvedAction::OpenGetInvolvedAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"kadu_icons/get-involved"});
	setName(QStringLiteral("getInvolvedAction"));
	setText(tr("Get Involved"));
	setType(ActionDescription::TypeMainMenu);
}

OpenGetInvolvedAction::~OpenGetInvolvedAction()
{
}

void OpenGetInvolvedAction::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void OpenGetInvolvedAction::setUrlOpener(UrlOpener *urlOpener)
{
	m_urlOpener = urlOpener;
}

void OpenGetInvolvedAction::actionTriggered(QAction *, bool)
{
	if (m_configuration->deprecatedApi()->readEntry("General", "Language") == "pl")
		m_urlOpener->openUrl("http://www.kadu.im/w/Do%C5%82%C4%85cz");
	else
		m_urlOpener->openUrl("http://www.kadu.im/w/English:GetInvolved");
}

#include "moc_open-get-involved-action.cpp"

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

#include "show-descriptions-action.h"

#include "actions/action.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"

ShowDescriptionsAction::ShowDescriptionsAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setCheckable(true);
	setIcon(KaduIcon{"kadu_icons/show-descriptions"});
	setName(QStringLiteral("descriptionsAction"));
	setText(tr("Show Descriptions"));
	setType(ActionDescription::TypeUserList);
}

ShowDescriptionsAction::~ShowDescriptionsAction()
{
}

void ShowDescriptionsAction::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ShowDescriptionsAction::actionInstanceCreated(Action *action)
{
	auto enabled = m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowDesc");
	action->setChecked(enabled);
}

void ShowDescriptionsAction::actionTriggered(QAction *, bool toggled)
{
	m_configuration->deprecatedApi()->writeEntry("Look", "ShowDesc", toggled);
	ConfigurationAwareObject::notifyAll();
}

#include "moc_show-descriptions-action.cpp"

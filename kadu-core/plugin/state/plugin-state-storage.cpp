/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin-state-storage.h"

#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "plugin/state/plugin-state.h"
#include "storage/storage-point.h"

#include <QtCore/QVector>
#include <QtXml/QDomElement>

QMap<QString, PluginState> PluginStateStorage::load(StoragePoint &storagePoint) const
{
	auto result = QMap<QString, PluginState>();
	auto elements = storagePoint.storage()->getNodes(storagePoint.point(), QLatin1String("Plugin"));
	for (const auto &element : elements)
	{
		auto name = element.attribute("name");
		auto state = stringToPluginState(storagePoint.storage()->getTextNode(element, QLatin1String("State")));
		result.insert(name, state);
	}

	return result;
}

void PluginStateStorage::store(StoragePoint &storagePoint, const QMap<QString, PluginState> &pluginStates) const
{
	storagePoint.storage()->removeChildren(storagePoint.point());

	for (const auto &name : pluginStates.keys())
	{
		auto stateString = pluginStateToString(pluginStates.value(name));
		if (!stateString.isEmpty())
		{
			auto node = storagePoint.storage()->getNamedNode(storagePoint.point(), QLatin1String{"Plugin"}, name, ConfigurationApi::ModeAppend);
			storagePoint.storage()->appendTextNode(node, QLatin1String{"State"}, stateString);
		}
	}
}

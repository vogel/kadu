/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "last-seen-plugin-injector-factory.h"

#include "last-seen-module.h"

#include <injeqt/injector.h>

LastSeenPluginInjectorFactory::LastSeenPluginInjectorFactory(QObject *parent) :
		PluginInjectorFactory{parent}
{
}

LastSeenPluginInjectorFactory::~LastSeenPluginInjectorFactory()
{
}

injeqt::injector LastSeenPluginInjectorFactory::createPluginInjector(injeqt::injector &injector) const
{
	 auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
	 modules.emplace_back(make_unique<LastSeenModule>());

	return injeqt::injector{std::vector<injeqt::injector *>{&injector}, std::move(modules)};
}

#include "moc_last-seen-plugin-injector-factory.cpp"

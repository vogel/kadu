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

#include "idle-plugin-modules-factory.h"

#include "idle-module.h"

IdlePluginModulesFactory::IdlePluginModulesFactory(QObject *parent) : PluginModulesFactory{parent}
{
}

IdlePluginModulesFactory::~IdlePluginModulesFactory()
{
}

std::vector<std::unique_ptr<injeqt::module>> IdlePluginModulesFactory::createPluginModules() const
{
    auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
    modules.emplace_back(std::make_unique<IdleModule>());

    return modules;
}

#include "moc_idle-plugin-modules-factory.cpp"

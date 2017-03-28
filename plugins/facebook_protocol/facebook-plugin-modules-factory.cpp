/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "facebook-plugin-modules-factory.h"

#include "facebook-module.h"

#include <injeqt/injector.h>

FacebookPluginModulesFactory::FacebookPluginModulesFactory(QObject *parent) : PluginModulesFactory{parent}
{
}

FacebookPluginModulesFactory::~FacebookPluginModulesFactory()
{
}

std::vector<std::unique_ptr<injeqt::module>> FacebookPluginModulesFactory::createPluginModules() const
{
    auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
    modules.emplace_back(std::make_unique<FacebookModule>());

    return modules;
}

#include "facebook-plugin-modules-factory.moc"

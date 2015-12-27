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

#include "cenzor-plugin-injector-factory.h"

#include "cenzor-module.h"

#include <injeqt/injector.h>

CenzorPluginInjectortFactory::CenzorPluginInjectortFactory(QObject *parent) :
		PluginInjectorFactory{parent}
{
}

CenzorPluginInjectortFactory::~CenzorPluginInjectortFactory()
{
}

injeqt::injector CenzorPluginInjectortFactory::createPluginInjector(injeqt::injector &injector) const
{
	 auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
	 modules.emplace_back(make_unique<CenzorModule>());

	return injeqt::injector{std::vector<injeqt::injector *>{&injector}, std::move(modules)};
}

#include "moc_cenzor-plugin-injector-factory.cpp"

/*
 * %kadu copyright begin%
 * Copyright 2015, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"
#include "misc/memory.h"

#include <QtCore/QtPlugin>
#include <injeqt/module.h>

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginModulesFactory
 * @short Factory for injector for plugins.
 *
 * Every Kadu plugin has to have a PluginModulesFactory that creates injeqt::injector instances.
 * Use injector.get<PluginObject> to get a plugin object.
 */
class KADUAPI PluginModulesFactory : public QObject
{
    Q_OBJECT

public:
    explicit PluginModulesFactory(QObject *parent = nullptr);
    virtual ~PluginModulesFactory();

    virtual std::vector<std::unique_ptr<injeqt::module>> createPluginModules() const = 0;
    virtual QString parentInjectorName() const;
};

Q_DECLARE_INTERFACE(PluginModulesFactory, "im.kadu.PluginModulesFactory")

/**
 * @}
 */

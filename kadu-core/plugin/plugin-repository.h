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

#pragma once

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class PluginActivationService;
class PluginObject;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginRepository
 * @short Contains and returns plugin objects.
 */
class KADUAPI PluginRepository : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit PluginRepository(QObject *parent = nullptr);
	virtual ~PluginRepository();

	/**
	 * @param pluginName name of plugin
	 * @return root object from plugin with name pluginName
	 */
	template<class T>
	T * pluginObject(const QString &pluginName) const
	{
		return dynamic_cast<T*>(pluginObject(pluginName));
	}

private:
	QPointer<PluginActivationService> m_pluginActivationService;

	PluginObject * pluginObject(const QString &pluginName) const;

private slots:
	INJEQT_SETTER void setPluginActivationService(PluginActivationService *pluginActivationService);

};

/**
 * @}
 */

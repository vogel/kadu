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

#pragma once

#include "history-exports.h"

#include "misc/memory.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injector.h>

class PluginInjectorProvider;

class HISTORYAPI HistoryInjectedFactory : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit HistoryInjectedFactory(QObject *parent = nullptr);
	virtual ~HistoryInjectedFactory();

	template<typename T, typename ...Args>
	T * makeInjected(Args&& ...args)
	{
		auto result = new T(std::forward<Args>(args)...);
		injectInto(result);
		return result;
	}

	template<typename T, typename ...Args>
	owned_qptr<T> makeOwned(Args&& ...args)
	{
		auto result = make_owned<T>(std::forward<Args>(args)...);
		injectInto(result);
		return result;
	}

	template<typename T, typename ...Args>
	not_owned_qptr<T> makeNotOwned(Args&& ...args)
	{
		auto result = make_not_owned<T>(std::forward<Args>(args)...);
		injectInto(result);
		return result;
	}

	template<typename T, typename ...Args>
	std::unique_ptr<T> makeUnique(Args&& ...args)
	{
		auto result = make_unique<T>(std::forward<Args>(args)...);
		injectInto(result.get());
		return result;
	}

private:
	QPointer<PluginInjectorProvider> m_pluginInjectorProvider;

	void injectInto(QObject *object);

private slots:
	INJEQT_SET void setPluginInjectorProvider(PluginInjectorProvider *pluginInjectorProvider);

};

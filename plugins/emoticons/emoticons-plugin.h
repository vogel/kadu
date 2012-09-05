/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICONS_PLUGIN_H
#define EMOTICONS_PLUGIN_H

#include <QtCore/QObject>

#include "plugins/generic-plugin.h"

class EmoticonsConfigurationUiHandler;
class EmoticonConfigurator;
class EmoticonExpanderDomVisitorProvider;
class InsertEmoticonAction;

class EmoticonsPlugin : public QObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

	QScopedPointer<EmoticonsConfigurationUiHandler> ConfigurationUiHandler;
	QScopedPointer<EmoticonExpanderDomVisitorProvider> ExpanderDomVisitorProvider;
	QScopedPointer<InsertEmoticonAction> InsertAction;
	QScopedPointer<EmoticonConfigurator> Configurator;

	void registerConfigurationUi();
	void unregisterConfigurationUi();

	void registerEmoticonExpander();
	void unregisterEmoticonExpander();

	void registerActions();
	void unregisterActions();

	void startConfigurator();
	void stopConfigurator();

public:
	explicit EmoticonsPlugin(QObject *parent = 0);
	virtual ~EmoticonsPlugin();

	virtual int init(bool firstLoad);
	virtual void done();

};

#endif // EMOTICONS_PLUGIN_H

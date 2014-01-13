/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef AUTORESPONDER_H
#define AUTORESPONDER_H

#include <QtCore/QObject>
#include <QtCore/QSet>

#include "chat/chat.h"
#include "message/message-filter.h"
#include "plugin/plugin-root-component.h"

#include "autoresponder-configuration.h"

// TODOfind a better interface than MessageFilter for this class

class AutoresponderConfigurationUiHolder;
class AutoresponderConfigurator;
class ChatWidget;

class AutoResponder : public MessageFilter, public PluginRootComponent
{
	Q_OBJECT
	Q_INTERFACES(PluginRootComponent)
	Q_PLUGIN_METADATA(IID "im.kadu.PluginRootComponent")

	AutoresponderConfigurationUiHolder *UiHandler;
	AutoresponderConfigurator *Configurator;
	AutoresponderConfiguration Configuration;

	QSet<Chat> RepliedChats;

public:
	explicit AutoResponder(QObject *parent = 0);
	virtual ~AutoResponder();

	void setConfiguration(const AutoresponderConfiguration &configuration);

	virtual bool acceptMessage(const Message &message);

	virtual bool init(bool firstLoad);
	virtual void done();

public slots:
	void chatWidgetClosed(ChatWidget *chat);

};

#endif // AUTORESPONDER_H

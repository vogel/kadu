/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef HISTORY_CHAT_CONFIGURATION_WIDGET_H
#define HISTORY_CHAT_CONFIGURATION_WIDGET_H

#include "configuration/configuration-aware-object.h"
#include "gui/widgets/chat-configuration-widget.h"

class QCheckBox;

class SimpleConfigurationValueStateNotifier;

class HistoryChatConfigurationWidget : public ChatConfigurationWidget, ConfigurationAwareObject
{
	Q_OBJECT

	bool GlobalStoreHistory;
	QCheckBox *StoreHistoryCheckBox;

	SimpleConfigurationValueStateNotifier *StateNotifier;

	void createGui();
	void loadValues();

private slots:
	void updateState();

protected:
	virtual void configurationUpdated();

public:
	explicit HistoryChatConfigurationWidget(const Chat &chat, QWidget *parent = 0);
	virtual ~HistoryChatConfigurationWidget();

	virtual const ConfigurationValueStateNotifier * stateNotifier() const;

	virtual void apply();
	virtual void cancel();

};

#endif // HISTORY_CHAT_CONFIGURATION_WIDGET_H

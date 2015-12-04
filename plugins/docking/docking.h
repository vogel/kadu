/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "docking-exports.h"

#include "misc/memory.h"

#include <QtCore/QObject>

class DockingConfigurationProvider;
class DockingMenuActionRepository;
class StatusNotifierItem;

class KaduIcon;

class DOCKINGAPI Docking final : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Docking)

public:
	static void createInstance();
	static void destroyInstance();
	static Docking * instance();
	
	DockingMenuActionRepository * dockingMenuActionRepository() const;

signals:
	void messageClicked();

private:
	static Docking *m_instance;

	owned_qptr<DockingConfigurationProvider> m_dockingConfigurationProvider;
	owned_qptr<DockingMenuActionRepository> m_dockingMenuActionRepository;
	owned_qptr<StatusNotifierItem> m_statusNotifierItem;

	Docking();
	virtual ~Docking();

	void openUnreadMessages();

private slots:
	void configurationUpdated();
	void statusIconChanged(const KaduIcon &icon);
	void needAttentionChanged(bool needAttention);
	void searchingForTrayPosition(QPoint &point);
	void activateRequested();

};

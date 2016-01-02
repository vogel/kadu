/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

#include "configuration/configuration-aware-object.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "configuration/gui/configuration-ui-handler.h"

class MPRISPlayer;

class QComboBox;

class MPRISPlayerConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
	Q_OBJECT
	
	QPointer<MPRISPlayer> m_mprisPlayer;

	QMap<QString, QString> PlayersMap;
	QComboBox *PlayersBox;

	void loadPlayersListFromFile();
	void fillPlayersBox();

private slots:
	INJEQT_SETTER void setMPRISPlayer(MPRISPlayer *mprisPlayer);

	void configurationApplied();

public:
	Q_INVOKABLE explicit MPRISPlayerConfigurationUiHandler(QObject *parent = nullptr);
	virtual ~MPRISPlayerConfigurationUiHandler();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
	virtual void mainConfigurationWindowDestroyed() override;
	virtual void mainConfigurationWindowApplied() override;

public slots:
	void addPlayer();
	void editPlayer();
	void delPlayer();

};

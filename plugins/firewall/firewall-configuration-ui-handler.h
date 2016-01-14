/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/gui/configuration-ui-handler.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyManager;
class Configuration;
class MainConfigurationWindow;

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QTextEdit;

class FirewallConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit FirewallConfigurationUiHandler(QObject *parent = nullptr);
	virtual ~FirewallConfigurationUiHandler();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
	virtual void mainConfigurationWindowDestroyed() override;
	virtual void mainConfigurationWindowApplied() override;

private:
	QPointer<BuddyManager> m_buddyManager;
	QPointer<Configuration> m_configuration;

	QListWidget *AllList;
	QListWidget *SecureList;
	QTextEdit *QuestionEdit;
	QLineEdit *AnswerEdit;

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);

	void left(QListWidgetItem *);
	void right(QListWidgetItem *);
	void allLeft();
	void allRight();

	void configurationApplied();

};

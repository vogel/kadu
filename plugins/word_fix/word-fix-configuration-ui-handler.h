/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

class QLineEdit;
class QPushButton;
class QTreeWidget;

class Configuration;
class WordFix;

class WordFixConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit WordFixConfigurationUiHandler(QObject *parent = nullptr);
	virtual ~WordFixConfigurationUiHandler();

public slots:
	void wordSelected();
	void changeSelected();
	void deleteSelected();
	void addNew();
	void moveToNewValue();

private:
	QPointer<Configuration> m_configuration;
	QPointer<WordFix> m_wordFix;

	QPushButton *m_changeButton;
	QPushButton *m_deleteButton;
	QPushButton *m_addButton;
	QLineEdit *m_wordEdit;
	QLineEdit *m_valueEdit;
	QTreeWidget *m_list;

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
	virtual void mainConfigurationWindowDestroyed() override;
	virtual void mainConfigurationWindowApplied() override;

	void saveList();

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setWordFix(WordFix *wordFix);

};

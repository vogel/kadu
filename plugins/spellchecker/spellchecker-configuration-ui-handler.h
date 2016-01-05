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

#include "configuration/gui/configuration-ui-handler.h"

#include "misc/memory.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class SpellcheckerConfiguration;
class SpellChecker;

class QListWidget;
class QListWidgetItem;

class SpellcheckerConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SpellcheckerConfigurationUiHandler(QObject *parent = nullptr);
	virtual ~SpellcheckerConfigurationUiHandler();

protected:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
	virtual void mainConfigurationWindowDestroyed() override;
	virtual void mainConfigurationWindowApplied() override;

private:
	QPointer<SpellcheckerConfiguration> m_spellcheckerConfiguration;
	QPointer<SpellChecker> m_spellChecker;

	owned_qptr<QListWidget> m_availableLanguagesList;
	owned_qptr<QListWidget> m_checkedLanguagesList;

private slots:
	INJEQT_SET void setSpellcheckerConfiguration(SpellcheckerConfiguration *spellcheckerConfiguration);
	INJEQT_SET void setSpellChecker(SpellChecker *spellChecker);

	void configForward();
	void configBackward();
	void configForward2(QListWidgetItem *item);
	void configBackward2(QListWidgetItem *item);

};

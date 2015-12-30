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

#include "spellchecker-configuration-ui-handler.h"

#include "configuration/spellchecker-configuration.h"
#include "spellchecker.h"

#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/message-dialog.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>

SpellcheckerConfigurationUiHandler::SpellcheckerConfigurationUiHandler(QObject *parent) :
		QObject{parent}
{
}

SpellcheckerConfigurationUiHandler::~SpellcheckerConfigurationUiHandler()
{
}

void SpellcheckerConfigurationUiHandler::setSpellcheckerConfiguration(SpellcheckerConfiguration *spellcheckerConfiguration)
{
	m_spellcheckerConfiguration = spellcheckerConfiguration;
}

void SpellcheckerConfigurationUiHandler::setSpellChecker(SpellChecker *spellChecker)
{
	m_spellChecker = spellChecker;
}

void SpellcheckerConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
#if !defined(HAVE_ASPELL)
	mainConfigurationWindow->widget()->widgetById("spellchecker/ignoreCase")->hide();
#endif

	auto optionsGroupBox = mainConfigurationWindow->widget()->configGroupBox("Chat", "Spelling", "Spell Checker Options");

	auto options = make_owned<QWidget>(optionsGroupBox->widget());
	auto optionsLayout = make_owned<QGridLayout>(options);

	m_availableLanguagesList = make_owned<QListWidget>(options);
	auto moveToChecked = make_owned<QPushButton>(tr("Move to 'Checked'"), options);

	optionsLayout->addWidget(make_owned<QLabel>(tr("Available languages"), options), 0, 0);
	optionsLayout->addWidget(m_availableLanguagesList, 1, 0);
	optionsLayout->addWidget(moveToChecked, 2, 0);

	m_checkedLanguagesList = make_owned<QListWidget>(options);
	auto moveToAvailable = make_owned<QPushButton>(tr("Move to 'Available languages'"), options);

	optionsLayout->addWidget(make_owned<QLabel>(tr("Checked"), options), 0, 1);
	optionsLayout->addWidget(m_checkedLanguagesList, 1, 1);
	optionsLayout->addWidget(moveToAvailable, 2, 1);

	connect(moveToChecked, SIGNAL(clicked()), this, SLOT(configForward()));
	connect(moveToAvailable, SIGNAL(clicked()), this, SLOT(configBackward()));
	connect(m_checkedLanguagesList.get(), SIGNAL(itemDoubleClicked(QListWidgetItem *)),
			this, SLOT(configBackward2(QListWidgetItem *)));
	connect(m_availableLanguagesList.get(), SIGNAL(itemDoubleClicked(QListWidgetItem *)),
			this, SLOT(configForward2(QListWidgetItem*)));

	optionsGroupBox->addWidgets(0, options);

	m_availableLanguagesList->setSelectionMode(QAbstractItemView::SingleSelection);
	m_checkedLanguagesList->setSelectionMode(QAbstractItemView::SingleSelection);
	m_availableLanguagesList->addItems(m_spellChecker->notCheckedLanguages());
	m_checkedLanguagesList->addItems(m_spellChecker->checkedLanguages());
}

void SpellcheckerConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
}

void SpellcheckerConfigurationUiHandler::mainConfigurationWindowApplied()
{
	m_spellcheckerConfiguration->setChecked(m_spellChecker->checkedLanguages());
}

void SpellcheckerConfigurationUiHandler::configForward()
{
	if (!m_availableLanguagesList->selectedItems().isEmpty())
		configForward2(m_availableLanguagesList->selectedItems().at(0));
}

void SpellcheckerConfigurationUiHandler::configBackward()
{
	if (!m_checkedLanguagesList->selectedItems().isEmpty())
		configBackward2(m_checkedLanguagesList->selectedItems().at(0));
}

void SpellcheckerConfigurationUiHandler::configForward2(QListWidgetItem *item)
{
	QString langName = item->text();
	if (m_spellChecker->addCheckedLang(langName))
	{
		m_checkedLanguagesList->addItem(langName);
		delete m_availableLanguagesList->takeItem(m_availableLanguagesList->row(item));
	}
	else
		mainConfigurationWindowApplied();
}

void SpellcheckerConfigurationUiHandler::configBackward2(QListWidgetItem *item)
{
	QString langName = item->text();
	m_availableLanguagesList->addItem(langName);
	delete m_checkedLanguagesList->takeItem(m_checkedLanguagesList->row(item));
	m_spellChecker->removeCheckedLang(langName);
}

#include "moc_spellchecker-configuration-ui-handler.cpp"

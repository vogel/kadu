/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "languages-manager.h"

#include "config-wizard-profile-page.h"

ConfigWizardProfilePage::ConfigWizardProfilePage(QWidget *parent) :
		ConfigWizardPage(parent)
{
	setDescription(tr("<h3>Welcome to Kadu Instant Messenger</h3>"
		"<p>This wizard will help you to configure the basic settings of Kadu.</p>"
		"<p>Please choose a preferred language and create a nickname</p>"));

	createGui();
}

ConfigWizardProfilePage::~ConfigWizardProfilePage()
{
}

void ConfigWizardProfilePage::createGui()
{
	formLayout()->addRow(new QLabel(tr("<h3>Profile setup</h3>"), this));

	LanguagesCombo = new QComboBox(this);
	setLanguages();
	formLayout()->addRow(tr("Language") + ':', LanguagesCombo);

	QLabel *restartInfo = new QLabel("<font size='-1'><i>" + (qApp->translate("@default",
			// NOTE: it's the same string as in varia/configuration/dialog.ui
			"Kadu needs to be restarted before changes to the language settings will take effect.")) +
			"</i></font>", this);
	formLayout()->addRow(QString(), restartInfo);

	NickNameEdit = new QLineEdit(this);
	NickNameEdit->setMaximumWidth(300);
	formLayout()->addRow(tr("Nickname") + ':', NickNameEdit);
}

void ConfigWizardProfilePage::setLanguages()
{
	for (QMap<QString, QString>::const_iterator it = LanguagesManager::languages().constBegin(), end = LanguagesManager::languages().constEnd(); it != end; ++it)
		LanguagesCombo->addItem(it.value(), it.key());
}

void ConfigWizardProfilePage::initializePage()
{
	int languageIndex = LanguagesCombo->findData(config_file.readEntry("General", "Language"));
	if (languageIndex > -1)
		LanguagesCombo->setCurrentIndex(languageIndex);

	NickNameEdit->setText(config_file.readEntry("General", "Nick", "Me"));
}

void ConfigWizardProfilePage::acceptPage()
{
	config_file.writeEntry("General", "Language", LanguagesCombo->itemData(LanguagesCombo->currentIndex()).toString());
	config_file.writeEntry("General", "Nick", NickNameEdit->text());

	Core::instance()->myself().setDisplay(NickNameEdit->text());
}

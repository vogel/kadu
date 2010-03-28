/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QTextBrowser>

#include "languages-manager.h"

#include "config-wizard-profile-page.h"

ConfigWizardProfilePage::ConfigWizardProfilePage(QWidget *parent) :
		QWizardPage(parent)
{
	createGui();
}

ConfigWizardProfilePage::~ConfigWizardProfilePage()
{
}

void ConfigWizardProfilePage::createGui()
{
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->setSpacing(5);

	QTextBrowser *descriptionPane = new QTextBrowser(this);
	descriptionPane->setText(tr("<h3>Welcome to Kadu Instant Messenger</h3>"
		"<p>This wizard will help you to configure the basic settings of Kadu.</p>"
		"<p>Please choose a preffered language and create a nickname</p>"));

	mainLayout->addWidget(descriptionPane, 2);

	QWidget *formWidget = new QWidget(this);
	QFormLayout *formLayout = new QFormLayout(formWidget);

	mainLayout->addWidget(formWidget, 5);

	formLayout->addRow(new QLabel(tr("<h3>Profile setup</h3>"), this));

	LanguagesCombo = new QComboBox(this);
	setLanguages();

	formLayout->addRow(tr("Language") + ":", LanguagesCombo);

	NickNameEdit = new QLineEdit(this);
	NickNameEdit->setText(tr("Me"));
	NickNameEdit->setMaximumWidth(300);

	formLayout->addRow(tr("Nickname") + ":", NickNameEdit);

	QPushButton *proxyConnectionButton = new QPushButton(tr("Configure..."));

	formLayout->addRow(tr("Proxy connection settings") + ":", proxyConnectionButton);
	formLayout->addRow("", new QLabel(tr("<font size='-1'><i>for advanced users only</i></font>"), this));
}

void ConfigWizardProfilePage::setLanguages()
{
	QStringList languageValues = LanguagesManager::languageValues();
	QStringList languageNames = LanguagesManager::languageNames();

	for (int i = 0; i < languageValues.size(); i++)
		LanguagesCombo->addItem(languageNames.at(i), languageValues.at(i));
}

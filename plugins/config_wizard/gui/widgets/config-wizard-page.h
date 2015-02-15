/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CONFIG_WIZARD_PAGE_H
#define CONFIG_WIZARD_PAGE_H

#include <QtWidgets/QWizardPage>

class QFormLayout;
class QTextBrowser;

class ConfigWizardPage : public QWizardPage
{
	Q_OBJECT

	QTextBrowser * DescriptionPane;
	QFormLayout * FormLayout;

	void createGui();

protected:
	QFormLayout * formLayout() { return FormLayout; }

public:
	explicit ConfigWizardPage(QWidget *parent = 0);
	virtual ~ConfigWizardPage();

	virtual void acceptPage();
	virtual void rejectPage();

	void setDescription(const QString &description);

};

#endif // CONFIG_WIZARD_PAGE_H

/*
 * %kadu copyright begin%
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>

#include "identities/identity.h"
#include "identities/identity-manager.h"

#include "choose-identity-widget.h"

ChooseIdentityWidget::ChooseIdentityWidget(QWidget *parent) : QWidget(parent)
{
	description = new QComboBox;

	QStringList items;
	foreach (const Identity identity, IdentityManager::instance()->items())
		items << identity.name();

	description->addItems(items);
	description->insertSeparator(items.count());
	description->addItem(tr("Create new description..."));
	connect(description, SIGNAL(activated(int)), this, SLOT(identitySelected(int)));

	newDescriptionEdit = new QLineEdit;
	connect(newDescriptionEdit, SIGNAL(textChanged(QString)), this, SIGNAL(identityChanged()));

	if (description->count() == 2)
	{
		description->setCurrentIndex(1);
		newDescriptionEdit->setVisible(true);
	}
	else
		newDescriptionEdit->setVisible(false);

	QVBoxLayout *layout = new QVBoxLayout(this);
	
	layout->addWidget(description);
	layout->addWidget(newDescriptionEdit);
}

void ChooseIdentityWidget::identitySelected(int index)
{
	newDescriptionEdit->setVisible(index == description->count() - 1);

	emit identityChanged();
}

QString ChooseIdentityWidget::identityName()
{
	return description->currentIndex() == description->count() - 1
		? newDescriptionEdit->text()
		: description->currentText();
}


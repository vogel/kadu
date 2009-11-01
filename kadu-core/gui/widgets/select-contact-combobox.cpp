/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCompleter>

#include "buddies/buddy-manager.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/model/buddies-model.h"
#include "buddies/model/buddies-model-proxy.h"
#include "gui/widgets/contacts-line-edit.h"
#include "gui/widgets/contacts-list-view.h"
#include "gui/widgets/select-contact-popup.h"

#include "select-contact-combobox.h"

SelectContactCombobox::SelectContactCombobox(QWidget *parent) :
		QComboBox(parent), CurrentContact(BuddyShared::TypeNull)
{
	setEditable(true);

	connect(this, SIGNAL(editTextChanged(const QString &)),
			this, SLOT(contactTextChanged(const QString &)));

	BuddiesModel *model = new BuddiesModel(BuddyManager::instance(), this);
	ProxyModel = new BuddiesModelProxy(this);
	ProxyModel->setSourceModel(model);

	QCompleter *completer = new QCompleter(this);
	completer->setPopup(new ContactsListView(0, this));
	completer->setModel(ProxyModel);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionRole(Qt::DisplayRole);
	completer->setCompletionMode(QCompleter::PopupCompletion);

	setCompleter(completer);

	Popup = new SelectContactPopup();
}

SelectContactCombobox::~SelectContactCombobox()
{
	delete Popup;
	Popup = 0;
}


void SelectContactCombobox::addFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->addFilter(filter);
	Popup->view()->addFilter(filter);
}

void SelectContactCombobox::removeFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->removeFilter(filter);
	Popup->view()->removeFilter(filter);
}

void SelectContactCombobox::contactTextChanged(const QString &contactText)
{
	Buddy named = BuddyManager::instance()->byDisplay(contactText);
	if (named != CurrentContact)
	{
		CurrentContact = named;
		emit contactChanged(CurrentContact);
	}
}

void SelectContactCombobox::showPopup()
{
	Popup->setGeometry(QRect(
			mapToGlobal(QPoint(0, 0)),
			QSize(geometry().width(), Popup->geometry().height())));
	Popup->show(lineEdit()->text());
}

void SelectContactCombobox::hidePopup()
{
	Popup->hide();
	setEditText(Popup->nameFilterEdit()->text());
}

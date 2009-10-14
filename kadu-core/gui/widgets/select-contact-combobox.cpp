/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCompleter>
#include <QtGui/QLineEdit>

#include "contacts/contact-manager.h"
#include "contacts/filter/contact-name-filter.h"
#include "contacts/model/contacts-model.h"
#include "gui/widgets/contacts-list-view.h"
#include "gui/widgets/select-contact-popup.h"

#include "select-contact-combobox.h"

SelectContactCombobox::SelectContactCombobox(QWidget *parent) :
		QComboBox(parent)
{
	setEditable(true);

	ContactsModel *model = new ContactsModel(ContactManager::instance(), this);

	QCompleter *completer = new QCompleter(this);
	completer->setPopup(new ContactsListView(0, this));
	completer->setModel(model);
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

void SelectContactCombobox::showPopup()
{
	Popup->setGeometry(QRect(
			mapToGlobal(QPoint(0, height())),
			QSize(geometry().width(), Popup->geometry().height())));
	Popup->show();
}

void SelectContactCombobox::hidePopup()
{
	Popup->hide();
}

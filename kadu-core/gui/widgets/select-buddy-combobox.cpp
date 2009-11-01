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
#include "gui/widgets/buddies-line-edit.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/select-buddy-popup.h"

#include "select-buddy-combobox.h"

SelectBuddyCombobox::SelectBuddyCombobox(QWidget *parent) :
		QComboBox(parent), MyBuddy(BuddyShared::TypeNull)
{
	setEditable(true);

	connect(this, SIGNAL(editTextChanged(const QString &)),
			this, SLOT(buddyTextChanged(const QString &)));

	BuddiesModel *model = new BuddiesModel(BuddyManager::instance(), this);
	ProxyModel = new BuddiesModelProxy(this);
	ProxyModel->setSourceModel(model);

	QCompleter *completer = new QCompleter(this);
	completer->setPopup(new BuddiesListView(0, this));
	completer->setModel(ProxyModel);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionRole(Qt::DisplayRole);
	completer->setCompletionMode(QCompleter::PopupCompletion);

	setCompleter(completer);

	Popup = new SelectBuddyPopup();
}

SelectBuddyCombobox::~SelectBuddyCombobox()
{
	delete Popup;
	Popup = 0;
}


void SelectBuddyCombobox::addFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->addFilter(filter);
	Popup->view()->addFilter(filter);
}

void SelectBuddyCombobox::removeFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->removeFilter(filter);
	Popup->view()->removeFilter(filter);
}

void SelectBuddyCombobox::buddyTextChanged(const QString &contactText)
{
	Buddy named = BuddyManager::instance()->byDisplay(contactText);
	if (named != MyBuddy)
	{
		MyBuddy = named;
		emit buddyChanged(MyBuddy);
	}
}

void SelectBuddyCombobox::showPopup()
{
	Popup->setGeometry(QRect(
			mapToGlobal(QPoint(0, 0)),
			QSize(geometry().width(), Popup->geometry().height())));
	Popup->show(lineEdit()->text());
}

void SelectBuddyCombobox::hidePopup()
{
	Popup->hide();
	setEditText(Popup->nameFilterEdit()->text());
}

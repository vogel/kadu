/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QAbstractItemModel>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>

#include "accounts/account_manager.h"

#include "contacts/contact.h"
#include "contacts/contact-account-data.h"
#include "contacts/model/contacts-model.h"

#include "config_file.h"

#include "contacts-list-widget-delegate.h"

ContactsListWidgetDelegate::ContactsListWidgetDelegate(QObject *parent)
	: QStyledItemDelegate(parent), DescriptionFontMetrics(0)
{
	configurationUpdated();
}

Contact ContactsListWidgetDelegate::contact(const QModelIndex &index) const
{
	const ContactsModel *model = dynamic_cast<const ContactsModel *>(index.model());
	if (!model)
		return Contact::null;

	return model->contact(index);
}

void ContactsListWidgetDelegate::configurationUpdated()
{
	QFont font = config_file.readFontEntry("Look", "UserboxFont");

	if (DescriptionFontMetrics)
		delete DescriptionFontMetrics;

	font.setPointSize(font.pointSize() - 2);
	DescriptionFontMetrics = new QFontMetrics(font);
}

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

#include "accounts/account_manager.h"

#include "contacts/contact.h"
#include "contacts/contact-account-data.h"
#include "contacts/model/contacts-model.h"

#include "config_file.h"

#include "contacts-list-widget-delegate.h"

ContactsListWidgetDelegate::ContactsListWidgetDelegate(QObject *parent)
	: DescriptionFontMetrics(0)
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

void ContactsListWidgetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (!DescriptionFontMetrics)
		return;

	Contact cnt = contact(index);

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *cad = cnt.accountData(account);

	QColor origColor = painter->pen().color();
	QString description;

	if (cad)
	{
		// TODO: 0.6.6
/*
		if (User.protocolData("Gadu", "Blocking").toBool())
			painter->setPen(QColor(255, 0, 0));
		else if (IgnoredManager::isIgnored(UserListElements(users)))
			painter->setPen(QColor(192, 192, 0));
		else if (config_file.readBoolEntry("General", "PrivateStatus") && User.protocolData("Gadu", "OfflineTo").toBool())
			painter->setPen(QColor(128, 128, 128));
*/
//		if (User.data("HideDescription").toString() != "true")

		description = cad->status().description();
	}

	int itemHeight = 50; // AlignUserboxIconsTop ? lineHeight(listBox()):height(listBox());
	int yPos;
	bool hasDescription = !description.isEmpty();

	QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
	if (!icon.isNull())
	{
		yPos = (itemHeight - 32) / 2;
		painter->drawPixmap(3, yPos, icon.pixmap(32, 32));
	}

	QString display = index.data().toString();
	if (display.isNull())
		return;

	QFont oldFont = painter->font();

// 	if (isBold())
// 	{
// 		QFont newFont = QFont(oldFont);
// 		newFont.setWeight(QFont::Bold);
// 		painter->setFont(newFont);
// 	}

	QFontMetrics fm = painter->fontMetrics();

	if (/*ShowDesc &&*/ hasDescription)
		yPos = fm.ascent() + 1;
	else
		yPos = ((itemHeight - fm.height()) / 2) + fm.ascent();

	painter->drawText(32 + 5, yPos, display);

// 	if (isBold())
// 		painter->setFont(oldFont);

	if (/*!ShowDesc ||*/ !hasDescription)
		return;

	yPos += fm.height() - fm.descent();

	QFont newFont = QFont(oldFont);
	newFont.setPointSize(oldFont.pointSize() - 2);
	painter->setFont(newFont);

// 	if (!ShowMultilineDesc)
// 		description.replace("\n", " ");
/*
	int h;
	QStringList out;
	calculateSize(description, width(listBox()) - 5 - pm.width(), out, h);
	if (!out.empty() && !isSelected())
		painter->setPen(descColor);
	else
		painter->setPen(origColor);
	foreach(const QString &text, out)
	{
		painter->drawText(pm.width() + 5, yPos, text);
		yPos += descriptionFontMetrics->lineSpacing();
	}

	painter->setFont(oldFont);*/
}

QSize ContactsListWidgetDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Contact cnt = contact(index);
	if (cnt.isNull())
		return QSize(0, 0);

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *cad = cnt.accountData(account);

	return QSize(100, 100);
}

void ContactsListWidgetDelegate::configurationUpdated()
{
	QFont font = config_file.readFontEntry("Look", "UserboxFont");

	if (DescriptionFontMetrics)
		delete DescriptionFontMetrics;

	font.setPointSize(font.pointSize() - 2);
	DescriptionFontMetrics = new QFontMetrics(font);
}

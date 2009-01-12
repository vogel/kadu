/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QAbstractItemModel>
#include <QtGui/QAbstractItemView>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>
#include <QtGui/QTextDocument>
#include <QtGui/QTextFrame>
#include <QtGui/QTextFrameFormat>
#include <QtGui/QTextOption>

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "contacts/contact.h"
#include "contacts/contact-account-data.h"
#include "contacts/model/contacts-model.h"

#include "config_file.h"

#include "contacts-list-widget-delegate.h"

ContactsListWidgetDelegate::ContactsListWidgetDelegate(ContactsModel *model, QObject *parent)
	: QAbstractItemDelegate(parent), Model(model), DescriptionFontMetrics(0)
{
	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account *)),
		this, SLOT(accountRegistered(Account *)));
	foreach (Account *account, AccountManager::instance()->accounts())
		accountRegistered(account);

	configurationUpdated();
}

ContactsListWidgetDelegate::~ContactsListWidgetDelegate()
{
	disconnect(AccountManager::instance(), SIGNAL(accountRegistered(Account *)),
		this, SLOT(accountRegistered(Account *)));
}

void ContactsListWidgetDelegate::accountRegistered(Account *account)
{
	connect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(contactStatusChanged(Account *, Contact, Status)));
}

void ContactsListWidgetDelegate::accountUnregistered(Account *account)
{
	disconnect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(contactStatusChanged(Account *, Contact, Status)));
}

void ContactsListWidgetDelegate::contactStatusChanged(Account *account, Contact contact, Status oldStatus)
{
	emit sizeHintChanged(Model->contactIndex(contact));
}

Contact ContactsListWidgetDelegate::contact(const QModelIndex &index) const
{
	const ContactsModel *model = dynamic_cast<const ContactsModel *>(index.model());
	if (!model)
		return Contact::null;

	return model->contact(index);
}

QTextDocument * ContactsListWidgetDelegate::getDescriptionDocument(const QString &text, int width) const
{
	QTextDocument *doc = new QTextDocument(text);

	QTextOption opt = doc->defaultTextOption();
	opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	doc->setDefaultTextOption(opt);

	QTextFrameFormat fmt = doc->rootFrame()->frameFormat();
	fmt.setMargin(0);
	doc->rootFrame()->setFrameFormat(fmt);

	doc->setTextWidth(width);
	return doc;
}

QSize ContactsListWidgetDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size(0, 0);

	const QStyleOptionViewItemV4 *opt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(&option);
	if (!opt)
		return size;

	const QAbstractItemView *widget = dynamic_cast<const QAbstractItemView *>(opt->widget);
	if (!widget)
		return size;
	int width = 230; // widget->viewport()->width();

	Contact con = contact(index);
	ContactAccountData *cad = con.accountData(AccountManager::instance()->defaultAccount());

	QString description;
	if (cad)
		description = cad->status().description();

	if (description.isEmpty())
		return QSize(width, widget->fontMetrics().lineSpacing() + 3);

	QTextDocument *document = getDescriptionDocument(description, width);

	size = QSize(width, document->size().height() + widget->fontMetrics().lineSpacing() + 3);
	delete document;

	printf("size: %d %d\n", size.width(), size.height());

	return size;
}

void ContactsListWidgetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	const QStyleOptionViewItemV4 *opt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(&option);
	if (!opt)
		return;

	const QAbstractItemView *widget = dynamic_cast<const QAbstractItemView *>(opt->widget);
	if (!widget)
		return;

	QRect rect = opt->rect;

	Contact con = contact(index);
	ContactAccountData *cad = con.accountData(AccountManager::instance()->defaultAccount());

	QString description;
	if (cad)
		description = cad->status().description();

	painter->save();
	painter->setClipRect(rect);
	painter->translate(rect.topLeft());

	int itemHeight = /*AlignUserboxIconsTop ?*/ widget->fontMetrics().lineSpacing()/* : rect.height()*/;
	int yPos;

	QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
	if (!pixmap.isNull())
	{
		yPos = (itemHeight - pixmap.height()) / 2;
		painter->drawPixmap(3, yPos, pixmap);
	}

	QString display = index.data(Qt::DisplayRole).toString();
	if (display.isEmpty())
	{
		painter->restore();
		return;
	}

// 	if (isBold())
// 	{
// 		QFont newFont = QFont(oldFont);
// 		newFont.setWeight(QFont::Bold);
// 		painter->setFont(newFont);
// 	}

	QFontMetrics fm = painter->fontMetrics();

	yPos = (/*ShowDesc &&*/ !description.isEmpty())
		? fm.ascent() + 1
		: ((itemHeight - fm.height()) / 2) + fm.ascent();

	painter->drawText(pixmap.width() + 5, yPos, display);

// 	if (isBold())
// 		painter->setFont(oldFont);

	if (description.isEmpty())
	{
		painter->restore();
		return;
	}

	yPos += fm.height() - fm.descent();

// 	QFont newFont = QFont(font);
// 	newFont.setPointSize(oldFont.pointSize() - 2);
// 	painter->setFont(newFont);

// 	if (!ShowMultilineDesc)
// 		description.replace("\n", " ");

// 	QRect descriptionRect = rect;
// 	descriptionRect.setX(pixmap.width() + 5);
// 	descriptionRect.setY(rect.y() + yPos);

	painter->translate(pixmap.width() + 5, yPos);

	QTextDocument *document = getDescriptionDocument(description, rect.width());
	document->drawContents(painter/*, rect*/);
	delete document;

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
			}*/

	painter->restore();
}

void ContactsListWidgetDelegate::configurationUpdated()
{
	QFont font = config_file.readFontEntry("Look", "UserboxFont");

	if (DescriptionFontMetrics)
		delete DescriptionFontMetrics;

	font.setPointSize(font.pointSize() - 2);
	DescriptionFontMetrics = new QFontMetrics(font);
}

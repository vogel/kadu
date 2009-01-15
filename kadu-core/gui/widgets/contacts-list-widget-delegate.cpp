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
#include <QtGui/QListView>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>
#include <QtGui/QTextDocument>
#include <QtGui/QTextFrame>
#include <QtGui/QTextFrameFormat>
#include <QtGui/QTextOption>

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "contacts/contact-account-data.h"
#include "contacts/model/contacts-model.h"

#include "config_file.h"

#include "contacts-list-widget-delegate.h"

ContactsListWidgetDelegate::ContactsListWidgetDelegate(ContactsModel *model, QObject *parent)
	: QAbstractItemDelegate(parent), Model(model)
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

void ContactsListWidgetDelegate::contactStatusChanged(Account *account, Contact c, Status oldStatus)
{
	emit sizeHintChanged(Model->contactIndex(c));
}

Contact ContactsListWidgetDelegate::contact(const QModelIndex &index) const
{
	const ContactsModel *model = dynamic_cast<const ContactsModel *>(index.model());
	if (!model)
		return Contact::null;

	return model->contact(index);
}

bool ContactsListWidgetDelegate::isBold(Contact contact) const
{
	if (!ShowBold)
		return false;

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *contactData = contact.accountData(account);

	if (0 == contactData)
		return false;

	Status status = contactData->status();
	return status.isOnline() || status.isBusy();
}

QString ContactsListWidgetDelegate::displayDescription(Contact contact) const
{
	if (!ShowDesc)
		return QString::null;

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *cad = contact.accountData(account);
	if (!cad)
		return QString::null;

	return cad->status().description();
}

QTextDocument * ContactsListWidgetDelegate::descriptionDocument(const QString &text, int width) const
{
	QTextDocument *doc = new QTextDocument(text);

	doc->setDefaultFont(DescriptionFont);

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
	printf("size hint\n");

	QSize size(0, 0);

	const QStyleOptionViewItemV4 *opt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(&option);
	if (!opt)
		return size;

	const QAbstractItemView *widget = dynamic_cast<const QAbstractItemView *>(opt->widget);
	if (!widget)
		return size;
	int width = widget->viewport()->width();
	printf("width: %d\n", width);

	QFontMetrics fontMetrics(Font);
	int displayHeight = fontMetrics.lineSpacing() + 3;

	QString description = displayDescription(contact(index));
	int descriptionHeight = 0;

	QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
	int textLeft = pixmap.isNull()
		? 5
		: pixmap.width() + 5;

	if (!description.isEmpty())
	{
		QTextDocument *dd = descriptionDocument(description, opt->rect.width() - textLeft);
		descriptionHeight = (int)dd->size().height();
		delete dd;
	}

	int pixmapHeight = pixmap.height();

	int height = QMAX(pixmapHeight, displayHeight + descriptionHeight);

	return QSize(width, height);
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

	painter->save();
	painter->setClipRect(rect);
	painter->translate(rect.topLeft());

	painter->setFont(Font);

	Contact con = contact(index);

	QFontMetrics fontMetrics(Font);
	int displayHeight = fontMetrics.lineSpacing() + 3;

	QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
	int pixmapHeight = pixmap.height();

	QString description = displayDescription(con);
	bool hasDescription = !description.isEmpty();

	QTextDocument *dd = 0;
	int descriptionHeight = 0;
	int textLeft = pixmap.isNull()
		? 5
		: pixmap.width() + 5;

	if (hasDescription)
	{
		dd = descriptionDocument(description, rect.width() - textLeft);
		descriptionHeight = (int)dd->size().height();
	}

	int height = QMAX(pixmapHeight, displayHeight + descriptionHeight);
	int itemHeight = displayHeight; // /*AlignUserboxIconsTop ?*/ widget->fontMetrics().lineSpacing() + 3/* : rect.height()*/;

	if (!pixmap.isNull())
		painter->drawPixmap(3, (itemHeight - pixmap.height()) / 2, pixmap);

	QString display = index.data(Qt::DisplayRole).toString();
	if (display.isEmpty())
	{
		painter->restore();
		if (dd)
			delete dd;
		return;
	}

	if (isBold(con))
	{
		QFont bold = QFont(Font);
		bold.setWeight(QFont::Bold);
		painter->setFont(bold);
	}

	int top = hasDescription
		? fontMetrics.ascent() + 1
		: ((itemHeight - fontMetrics.height()) / 2) + fontMetrics.ascent();

	painter->drawText(textLeft, top, display);

	if (isBold(con))
		painter->setFont(Font);

	if (!hasDescription)
	{
		painter->restore();
		// don't need to delete descriptionDocument here, it is for sure NULL
		return;
	}

	top += 5;

	painter->setFont(DescriptionFont);

// 	if (!ShowMultilineDesc)
// 		description.replace("\n", " ");

// 	QRect descriptionRect = rect;
// 	descriptionRect.setX(pixmap.width() + 5);
// 	descriptionRect.setY(rect.y() + yPos);

	painter->translate(textLeft, top);
	dd->drawContents(painter);
	delete dd;

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
	Font = config_file.readFontEntry("Look", "UserboxFont");
	DescriptionFont = Font;
	DescriptionFont.setPointSize(Font.pointSize() - 2);

	ShowDesc = config_file.readBoolEntry("Look", "ShowDesc");
	ShowBold = config_file.readBoolEntry("Look", "ShowBold");

	QListView *listView = dynamic_cast<QListView *>(parent());
	if (!listView)
		return;

	listView->repaint();
}

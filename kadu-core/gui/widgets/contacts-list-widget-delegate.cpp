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

	int displayHeight = widget->fontMetrics().lineSpacing() + 3;

	QString description = displayDescription(contact(index));
	int descriptionHeight = 0;
	if (!description.isEmpty())
	{
		QTextDocument *descriptionDocument = getDescriptionDocument(description, opt->rect.width());
		descriptionHeight = (int)descriptionDocument->size().height();
		delete descriptionDocument;
	}

	QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
	int pixmapHeight = pixmap.height();

	int height = QMAX(displayHeight, QMAX(pixmapHeight, descriptionHeight));

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

	Contact con = contact(index);

	int displayHeight = widget->fontMetrics().lineSpacing() + 3;

	QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
	int pixmapHeight = pixmap.height();

	QString description = displayDescription(con);
	bool hasDescription = !description.isEmpty();

	QTextDocument *descriptionDocument = 0;
	int descriptionHeight = 0;

	if (hasDescription)
	{
		descriptionDocument = getDescriptionDocument(description, rect.width());
		descriptionHeight = (int)descriptionDocument->size().height();
	}

	int height = QMAX(displayHeight, QMAX(pixmapHeight, descriptionHeight));
	int itemHeight = displayHeight; // /*AlignUserboxIconsTop ?*/ widget->fontMetrics().lineSpacing() + 3/* : rect.height()*/;
	int left = 32;

	if (!pixmap.isNull())
	{
		painter->drawPixmap(3, (itemHeight - pixmap.height()) / 2, pixmap);
		left = pixmap.width() + 5;
	}

	QString display = index.data(Qt::DisplayRole).toString();
	if (display.isEmpty())
	{
		painter->restore();
		if (descriptionDocument)
			delete descriptionDocument;
		return;
	}

	QFont oldFont = painter->font();
	if (isBold(con))
	{
		QFont newFont = QFont(oldFont);
		newFont.setWeight(QFont::Bold);
		painter->setFont(newFont);
	}

	QFontMetrics fm = painter->fontMetrics();

	int top = hasDescription
		? fm.ascent() + 1
		: ((itemHeight - fm.height()) / 2) + fm.ascent();

	painter->drawText(left, top, display);

	if (isBold(con))
		painter->setFont(oldFont);

	if (!hasDescription)
	{
		painter->restore();
		// don't need to delete descriptionDocument here, it is for sure NULL
		return;
	}

	top += fm.height() + 1;

	QFont newFont = QFont(painter->font());
	newFont.setPointSize(oldFont.pointSize() - 2);
	painter->setFont(newFont);

// 	if (!ShowMultilineDesc)
// 		description.replace("\n", " ");

// 	QRect descriptionRect = rect;
// 	descriptionRect.setX(pixmap.width() + 5);
// 	descriptionRect.setY(rect.y() + yPos);

	painter->translate(left, top);
	descriptionDocument->drawContents(painter/*, rect*/);
	delete descriptionDocument;

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
	ShowDesc = config_file.readBoolEntry("Look", "ShowDesc");
	ShowBold = config_file.readBoolEntry("Look", "ShowBold");

	if (DescriptionFontMetrics)
		delete DescriptionFontMetrics;

	font.setPointSize(font.pointSize() - 2);
	DescriptionFontMetrics = new QFontMetrics(font);

	QListView *listView = dynamic_cast<QListView *>(parent());
	if (!listView)
		return;

	listView->repaint();
}

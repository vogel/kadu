/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QPainter>
#include <QtGui/QTextDocument>
#include <QtGui/QTextFrame>
#include <QtGui/QTextFrameFormat>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeView>

#include "accounts/account.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy.h"
#include "chat/chat.h"
#include "contacts/contact.h"
#include "gui/widgets/avatar-painter.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "identities/identity.h"
#include "model/roles.h"

#include "talkable-painter.h"

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

#ifdef Q_OS_WIN32
bool TalkablePainter::useColorsWorkaround()
{
	static bool checked = false;
	static bool use = false;
	if (checked)
		return use;

	checked = true;

	// copied from QWindowsVistaStylePrivate::useVista()
	if (QSysInfo::WindowsVersion < QSysInfo::WV_VISTA || !(QSysInfo::WindowsVersion & QSysInfo::WV_NT_based))
		return false;

	// inspired by QWindowsXPStylePrivate::useXP()
	typedef BOOL (WINAPI *PtrIsAppThemed)();
	typedef BOOL (WINAPI *PtrIsThemeActive)();
	HMODULE uxThemeHandle = GetModuleHandle(TEXT("UxTheme.dll"));
	if (uxThemeHandle == NULL)
		return false;
	PtrIsAppThemed pIsAppThemed = (PtrIsAppThemed) GetProcAddress(uxThemeHandle, "IsAppThemed");
	PtrIsThemeActive pIsThemeActive = (PtrIsThemeActive) GetProcAddress(uxThemeHandle, "IsThemeActive");
	if (pIsAppThemed == NULL || pIsThemeActive == NULL)
		return false;
	use = (pIsThemeActive() && pIsAppThemed());

	return use;
}
#endif

TalkablePainter::TalkablePainter(const TalkableDelegateConfiguration &configuration, QStyleOptionViewItemV4 option, const QModelIndex &index) :
		Configuration(configuration), Option(option), Index(index),
		FontMetrics(Configuration.font()),
		BoldFontMetrics(Configuration.boldFont()),
		DescriptionFontMetrics(Configuration.descriptionFont()),
		DescriptionDocument(0)
{
	Widget = static_cast<const QTreeView *>(option.widget);

	Style = Widget ? Widget->style() : nullptr;

	int minHFrameMargin = 4;
	int minVFrameMargin = 2;

	int qStyleHFrameMargin = Style ? Style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, Widget) : 0;
	int qStyleVFrameMargin = Style ? Style->pixelMetric(QStyle::PM_FocusFrameVMargin, 0, Widget) : 0;

	HFrameMargin = qMax(minHFrameMargin, qStyleHFrameMargin);
	VFrameMargin = qMax(minVFrameMargin, qStyleVFrameMargin);
}

TalkablePainter::~TalkablePainter()
{
	delete DescriptionDocument;
	DescriptionDocument = 0;
}

void TalkablePainter::fixColors()
{
#ifdef Q_OS_WIN
	// Kadu bug #1531
	// http://bugreports.qt.nokia.com/browse/QTBUG-15637
	// for windows only
	if (useColorsWorkaround())
	{
		// copied from QWindowsVistaStyle::drawControl()
		Option.palette.setColor(QPalette::All, QPalette::HighlightedText, Option.palette.color(QPalette::Active, QPalette::Text));
		Option.palette.setColor(QPalette::All, QPalette::Highlight, Option.palette.base().color().darker(108));
	}
#endif
}

QColor TalkablePainter::textColor() const
{
	const QPalette::ColorGroup colorGroup = drawDisabled()
			? QPalette::Disabled
			: QPalette::Normal;

	const QPalette::ColorRole colorRole = drawSelected()
			? QPalette::HighlightedText
			: QPalette::Text;

	return Option.palette.color(colorGroup, colorRole);
}

bool TalkablePainter::useBold() const
{
	if (showMessagePixmap())
		return true;

	if (!Configuration.showBold())
		return false;

	const Contact &contact = Index.data(ContactRole).value<Contact>();
	if (!contact)
		return false;
	if (contact.isBlocking())
		return false;
	if (contact.ownerBuddy().isBlocked())
		return false;

	const Status &status = Index.data(StatusRole).value<Status>();
	return !status.isDisconnected();
}

bool TalkablePainter::showCheckbox() const
{
	return Index.flags() & Qt::ItemIsUserCheckable;
}

bool TalkablePainter::showMessagePixmap() const
{
	if (!Configuration.showMessagePixmap())
		return false;

	switch (Index.data(ItemTypeRole).toUInt())
	{
		case ChatRole:
		{
			const Chat &chat = Index.data(ChatRole).value<Chat>();
			return chat.unreadMessagesCount() > 0;
		}
		case BuddyRole:
		{
			const Buddy &buddy = Index.data(BuddyRole).value<Buddy>();
			return buddy.unreadMessagesCount() > 0;
		}
		case ContactRole:
		{
			const Contact &contact = Index.data(ContactRole).value<Contact>();
			return contact.unreadMessagesCount() > 0;
		}
	}

	return false;
}

bool TalkablePainter::showIdentityName() const
{
	if (!Configuration.showIdentityName())
		return false;

	if (Index.parent().isValid())
		return true;

	return Option.state & QStyle::State_MouseOver;
}

bool TalkablePainter::showDescription() const
{
	if (!Configuration.showDescription())
		return false;

	const QString &description = Index.data(DescriptionRole).toString();
	return !description.isEmpty();
}

void TalkablePainter::computeCheckboxRect()
{
	CheckboxRect = QRect(ItemRect.topLeft(), QSize(0, 0));

	if (!showCheckbox())
		return;

	QStyleOptionButton option;
	option.rect = CheckboxRect;
	option.state = QStyle::State_Enabled;

	QSize size = Style->sizeFromContents(QStyle::CT_CheckBox, &option, QSize());
#ifdef Q_OS_WIN32
	size.setWidth(size.width() + Style->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &option));
#endif
	CheckboxRect.setSize(size);
}

void TalkablePainter::computeIconRect()
{
	QPoint topLeft(CheckboxRect.x() + CheckboxRect.width(), ItemRect.y());
	IconRect = QRect(topLeft, QSize(0, 0));

	const QPixmap &paintedIcon = icon();
	if (paintedIcon.isNull())
		return;

	IconRect.setSize(paintedIcon.size() + QSize(HFrameMargin, 0));

	if (!Configuration.alignTop())
		IconRect.moveTop(ItemRect.top() + (ItemRect.height() - paintedIcon.height()) / 2);
	else if (fontMetrics().lineSpacing() > paintedIcon.height())
		IconRect.moveTop(ItemRect.top() + (fontMetrics().lineSpacing() - paintedIcon.height()) / 2);
}

void TalkablePainter::computeAvatarRect()
{
	AvatarRect = QRect(ItemRect.x() + ItemRect.width(), ItemRect.y(), 0, 0);
	if (!Configuration.showAvatars())
		return;

	int width = Configuration.defaultAvatarSize().width() + HFrameMargin;
	AvatarRect.setWidth(width);
	AvatarRect.moveLeft(AvatarRect.left() - width);

	if (!avatar().isNull())
		AvatarRect.setHeight(Configuration.defaultAvatarSize().height() + 2 * VFrameMargin);
}

QString TalkablePainter::getIdentityName()
{
	const Account &account = Index.data(AccountRole).value<Account>();
	return account.accountIdentity().name();
}

QString TalkablePainter::getName()
{
	return Index.data(Qt::DisplayRole).toString();
}

bool TalkablePainter::drawSelected() const
{
	return Option.state & QStyle::State_Selected;
}

bool TalkablePainter::drawDisabled() const
{
	const Buddy &buddy = Index.data(BuddyRole).value<Buddy>();
	return buddy.isOfflineTo();
}

QTextDocument * TalkablePainter::createDescriptionDocument(const QString &text, int width, QColor color) const
{
	QString description = Qt::escape(text)
	        .replace('\n', Configuration.showMultiLineDescription() ? QLatin1String("<br/>") : QLatin1String(" "));

	QTextDocument * const doc = new QTextDocument();

	doc->setDefaultFont(Configuration.descriptionFont());
	if (color.isValid())
		doc->setDefaultStyleSheet(QString("* { color: %1; }").arg(color.name()));

	doc->setHtml(QString("<span>%1</span>").arg(description));

	QTextOption opt = doc->defaultTextOption();
	opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	doc->setDefaultTextOption(opt);

	QTextFrameFormat frameFormat = doc->rootFrame()->frameFormat();
	frameFormat.setMargin(0);
	doc->rootFrame()->setFrameFormat(frameFormat);

	doc->setTextWidth(width);
	return doc;
}

QTextDocument * TalkablePainter::getDescriptionDocument(int width)
{
	if (DescriptionDocument)
		return DescriptionDocument;

	fixColors();

	const QColor &color = drawSelected() || drawDisabled() || !Configuration.useConfigurationColors()
			? textColor()
			: Configuration.descriptionColor();
	DescriptionDocument = createDescriptionDocument(Index.data(DescriptionRole).toString(), width, color);
	return DescriptionDocument;
}

void TalkablePainter::computeIdentityNameRect()
{
	IdentityNameRect = QRect(AvatarRect.topLeft(), QSize(0, 0));
	if (!showIdentityName())
		return;

	IdentityNameRect.setSize(DescriptionFontMetrics.size(0, getIdentityName()) + QSize(2 * HFrameMargin, 0));
	IdentityNameRect.moveRight(AvatarRect.left() - 1);
}

void TalkablePainter::computeNameRect()
{
	int left = IconRect.x() + IconRect.width();
	int right = IdentityNameRect.left() - 1;
	int width = right - left + 1;
	int height = fontMetrics().height();
	int top = ItemRect.top();
	if (Configuration.alignTop())
		if (fontMetrics().lineSpacing() < IconRect.height())
			top += (IconRect.height() - fontMetrics().lineSpacing()) / 2;

	NameRect = QRect(left, top, width, height);
}

void TalkablePainter::computeDescriptionRect()
{
	DescriptionRect = QRect();

	if (!showDescription())
		return;

	int left = NameRect.left();
	int right = AvatarRect.left() - 1;
	int width = right - left + 1;
	int height = int(getDescriptionDocument(width - HFrameMargin)->size().height() + qreal(0.5)) + VFrameMargin;
	int top = NameRect.y() + NameRect.height();

	DescriptionRect = QRect(left, top, width, height);
}

void TalkablePainter::computeLayout()
{
	computeCheckboxRect();
	computeIconRect();
	computeAvatarRect();
	computeIdentityNameRect();
	computeNameRect();
	computeDescriptionRect();
}

QPixmap TalkablePainter::avatar() const
{
	return Index.data(AvatarRole).value<QPixmap>();
}

QPixmap TalkablePainter::icon() const
{
	return Index.data(Qt::DecorationRole).value<QIcon>().pixmap(16, 16);
}

const QFontMetrics & TalkablePainter::fontMetrics()
{
	if (useBold())
		return BoldFontMetrics;
	else
		return FontMetrics;
}

int TalkablePainter::itemIndentation()
{
	int level = 0;

	if (Widget->rootIsDecorated())
		level++;

	if (Index.parent().isValid())
		level++;

	return level * Widget->indentation();
}

int TalkablePainter::height()
{
	ItemRect = QRect(0, 0, 0, 20);

	const QHeaderView * const header = Widget->header();
	if (header)
		ItemRect.setWidth(header->sectionSize(0) - itemIndentation());

	ItemRect.adjust(HFrameMargin, VFrameMargin, -HFrameMargin, -VFrameMargin);

	computeLayout();

	QRect wholeRect = CheckboxRect;
	wholeRect |= IconRect;
	wholeRect |= AvatarRect;
	wholeRect |= IdentityNameRect;
	wholeRect |= NameRect;
	wholeRect |= DescriptionRect;

	return wholeRect.height() + 2 * VFrameMargin;
}

void TalkablePainter::paintDebugRect(QPainter *painter, QRect rect, QColor color) const
{
	if (rect.isValid())
		rect.adjust(0, 0, -1, -1);

	painter->save();
	painter->setPen(color);
	painter->drawRect(rect);
	painter->restore();
}

void TalkablePainter::paintCheckbox(QPainter *painter)
{
	if (!showCheckbox())
		return;

	QStyleOptionButton option;
	option.rect = CheckboxRect;
	option.state = QStyle::State_Enabled;

	QVariant checked = Index.data(Qt::CheckStateRole);
	if (checked.isValid() && Qt::Checked == static_cast<Qt::CheckState>(checked.toInt()))
		option.state = option.state | QStyle::State_On;
	else
		option.state = option.state | QStyle::State_Off;

	Style->drawControl(QStyle::CE_CheckBox, &option, painter);
}

void TalkablePainter::paintIcon(QPainter *painter)
{
	QRect rect = IconRect.adjusted(0, 0, -HFrameMargin, 0);

	if (showMessagePixmap())
	{
		painter->drawPixmap(rect, Configuration.messagePixmap());
		return;
	}

	const QPixmap &paintedIcon = icon();
	if (paintedIcon.isNull())
		return;

	painter->drawPixmap(rect, paintedIcon);
}

void TalkablePainter::paintAvatar(QPainter *painter)
{
	if (!AvatarRect.isValid())
		return;

	QRect rect = AvatarRect.adjusted(HFrameMargin, VFrameMargin, 0, -VFrameMargin);
	AvatarPainter avatarPainter(Configuration, Option, rect, Index);
	avatarPainter.paint(painter);
}

void TalkablePainter::paintIdentityName(QPainter *painter)
{
	if (!showIdentityName())
		return;

	QRect rect = IdentityNameRect.adjusted(HFrameMargin, 0, -HFrameMargin, 0);
	painter->setFont(Configuration.descriptionFont());
	painter->drawText(rect, getIdentityName());
}

void TalkablePainter::paintName(QPainter *painter)
{
	if (useBold())
		painter->setFont(Configuration.boldFont());
	else
		painter->setFont(Configuration.font());

	painter->drawText(NameRect, fontMetrics().elidedText(getName(), Qt::ElideRight, NameRect.width()));
}

void TalkablePainter::paintDescription(QPainter *painter)
{
	if (!showDescription())
		return;

	QRect rect = DescriptionRect.adjusted(0, VFrameMargin, -HFrameMargin, 0);
	painter->setFont(Configuration.descriptionFont());
	painter->save();
	painter->translate(rect.topLeft());
	getDescriptionDocument(rect.width())->drawContents(painter);
	painter->restore();
}

void TalkablePainter::paint(QPainter *painter)
{
	ItemRect = Option.rect;
	ItemRect.adjust(HFrameMargin, VFrameMargin, -HFrameMargin, -VFrameMargin);

	computeLayout();

	fixColors();

	// some bit of broken logic
	if (drawSelected() || drawDisabled() || !Configuration.useConfigurationColors())
		painter->setPen(textColor());
	else
	{
		Buddy buddy = Index.data(BuddyRole).value<Buddy>();
		Contact contact = Index.data(ContactRole).value<Contact>();
		if (buddy.isBlocked() || contact.isBlocking())
			painter->setPen(QColor(255, 0, 0));
		else
			painter->setPen(Configuration.fontColor());
	}

	paintCheckbox(painter);
	paintIcon(painter);
	paintAvatar(painter);
	paintIdentityName(painter);
	paintName(painter);
	paintDescription(painter);

	/*
	paintDebugRect(painter, ItemRect, QColor(255, 0, 0));
	paintDebugRect(painter, CheckboxRect, QColor(255, 255, 0));
	paintDebugRect(painter, IconRect, QColor(0, 255, 0));
	paintDebugRect(painter, AvatarRect, QColor(0, 0, 255));
	paintDebugRect(painter, IdentityNameRect, QColor(255, 0, 255));
	paintDebugRect(painter, NameRect, QColor(0, 255, 255));
	paintDebugRect(painter, DescriptionRect, QColor(0, 0, 0));
	*/
}

QRect TalkablePainter::checkboxRect()
{
	ItemRect = Option.rect;
	ItemRect.adjust(HFrameMargin, VFrameMargin, -HFrameMargin, -VFrameMargin);

	computeLayout();
	return CheckboxRect;
}

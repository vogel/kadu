/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>

#include <Qt3Support/Q3DragObject>
#include <Qt3Support/Q3PopupMenu>
#include <Qt3Support/Q3VGroupBox>
#include <QtCore/QEvent>
#include <QtCore/QList>
#include <QtCore/QTextCodec>
#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QCursor>
#include <QtGui/QComboBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtGui/QResizeEvent>
#include <QtGui/QSpinBox>
#include <QtGui/QWheelEvent>

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "contacts/contact-account-data.h"

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "../modules/gadu_protocol/gadu.h"
#include "html_document.h"
#include "icons_manager.h"
#include "ignore.h"
#include "kadu.h"
#include "main_configuration_window.h"
#include "misc.h"
#include "pending_msgs.h"
#include "userbox.h"

QFontMetrics* KaduListBoxPixmap::descriptionFontMetrics = NULL;
UinType KaduListBoxPixmap::myUIN;
bool KaduListBoxPixmap::ShowDesc;
bool KaduListBoxPixmap::ShowBold;
bool KaduListBoxPixmap::AlignUserboxIconsTop;
bool KaduListBoxPixmap::ShowMultilineDesc;
int KaduListBoxPixmap::ColumnCount;
QColor KaduListBoxPixmap::descColor;

QImage *UserBox::backgroundImage = 0;

static bool brokenStringCompare;

ToolTipClassManager::ToolTipClassManager()
	: CurrentToolTipClass(0)
{
	kdebugf();
}

ToolTipClassManager::~ToolTipClassManager()
{
	kdebugf();

	if (CurrentToolTipClass)
		CurrentToolTipClass->hideToolTip();
}

void ToolTipClassManager::registerToolTipClass(const QString &toolTipClassName, ToolTipClass *toolTipClass)
{
	ToolTipClasses[toolTipClassName] = toolTipClass;

	if (ToolTipClassName == toolTipClassName)
		CurrentToolTipClass = toolTipClass;
}

void ToolTipClassManager::unregisterToolTipClass(const QString &toolTipClassName)
{
	kdebugf();

	if (ToolTipClassName == toolTipClassName && CurrentToolTipClass)
	{
		CurrentToolTipClass->hideToolTip();
		CurrentToolTipClass = 0;
	}

	if (ToolTipClasses.contains(ToolTipClassName))
		ToolTipClasses.remove(ToolTipClassName);
}

QStringList ToolTipClassManager::getToolTipClasses()
{
	return ToolTipClasses.keys();
}

void ToolTipClassManager::useToolTipClass(const QString &toolTipClassName)
{
	kdebugf();

	if (CurrentToolTipClass)
		CurrentToolTipClass->hideToolTip();

	ToolTipClassName = toolTipClassName;

	if (ToolTipClasses.contains(ToolTipClassName))
		CurrentToolTipClass = ToolTipClasses[ToolTipClassName];
	else
		CurrentToolTipClass = 0;
}

bool ToolTipClassManager::showToolTip(const QPoint &where, Contact who)
{
	if (CurrentToolTipClass)
	{
		CurrentToolTipClass->showToolTip(where, who);
		return true;
	}

	return false;
}

bool ToolTipClassManager::hideToolTip()
{
	if (CurrentToolTipClass)
	{
		CurrentToolTipClass->hideToolTip();
		return true;
	}

	return false;
}

void KaduListBoxPixmap::setFont(const QFont &f)
{
	kdebugf();
	QFont newFont = QFont(f);
	newFont.setPointSize(f.pointSize() - 2);
	if (descriptionFontMetrics)
		delete descriptionFontMetrics;
	descriptionFontMetrics= new QFontMetrics(newFont);
	kdebugf2();
}

KaduListBoxPixmap::KaduListBoxPixmap(Contact contact)
	: Q3ListBoxItem(), pm(pixmapForUser(contact)), buf_text(), buf_width(-1), buf_out(), buf_height(-1), CurrentContact(contact)
{
	setText(contact.display());
}

void KaduListBoxPixmap::setMyUIN(UinType u)
{
	myUIN = u;
}

void KaduListBoxPixmap::setShowDesc(bool sd)
{
	ShowDesc = sd;
}

void KaduListBoxPixmap::setShowBold(bool sb)
{
	ShowBold = sb;
}

void KaduListBoxPixmap::setAlignTop(bool at)
{
	AlignUserboxIconsTop = at;
}

void KaduListBoxPixmap::setShowMultilineDesc(bool m)
{
	ShowMultilineDesc = m;
}

void KaduListBoxPixmap::setColumnCount(int m)
{
	ColumnCount = m;
}

void KaduListBoxPixmap::setDescriptionColor(const QColor &col)
{
	descColor = col;
}

bool KaduListBoxPixmap::isBold() const
{
	if (!ShowBold)
		return false;

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *contactData = CurrentContact.accountData(account);

	if (0 == contactData)
		return false;

	Status status = contactData->status();
	return status.isOnline() || status.isBusy();
}

void KaduListBoxPixmap::paint(QPainter *painter)
{
	if (!descriptionFontMetrics)
		return;

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *data = CurrentContact.accountData(account);

//	kdebugf();
	QColor origColor = painter->pen().color();
	QString description;

	if (data)
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

		description = data->status().description();
	}

	int itemHeight = AlignUserboxIconsTop ? lineHeight(listBox()):height(listBox());
	int yPos;
	bool hasDescription = !description.isEmpty();

	if (!pm.isNull())
	{
		yPos = (itemHeight - pm.height()) / 2;
		painter->drawPixmap(3, yPos, pm);
	}

	if (!text().isEmpty())
	{
		QFont oldFont = painter->font();

		if (isBold())
		{
			QFont newFont = QFont(oldFont);
			newFont.setWeight(QFont::Bold);
			painter->setFont(newFont);
		}

		QFontMetrics fm = painter->fontMetrics();

		if (ShowDesc && hasDescription)
			yPos = fm.ascent() + 1;
		else
			yPos = ((itemHeight - fm.height()) / 2) + fm.ascent();

		painter->drawText(pm.width() + 5, yPos, text());

		if (isBold())
			painter->setFont(oldFont);

//		kdebugmf(KDEBUG_INFO, "isMyUin = %d, own_description = %s\n",
//			isMyUin, (const char *)unicode2latin(own_description));
		if (ShowDesc && hasDescription)
		{
			yPos += fm.height() - fm.descent();

			QFont newFont = QFont(oldFont);
			newFont.setPointSize(oldFont.pointSize() - 2);
			painter->setFont(newFont);

			if (!ShowMultilineDesc)
				description.replace("\n", " ");
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

			painter->setFont(oldFont);
		}
	}
//	kdebugf2();
}

int KaduListBoxPixmap::height(const Q3ListBox* lb) const
{
//	kdebugf();

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *data = CurrentContact.accountData(account);

	QString description;
	if (data)
		description = data->status().description();

// 	if (User.usesProtocol("Gadu") && User.data("HideDescription").toString() != "true")
// 		description = User.status("Gadu").description();

	bool hasDescription = !description.isEmpty();

	int height=lb->fontMetrics().lineSpacing()+3;
	if (hasDescription && ShowDesc)
	{
		if (!ShowMultilineDesc)
			description.replace("\n", " ");
		QStringList out;
		int h;
		calculateSize(description, width(lb) - 5 - pm.width(), out, h);
		height += h;
	}
//	kdebugf2();
	return QMAX(pm.height(), height);
}

int KaduListBoxPixmap::lineHeight(const Q3ListBox* lb) const
{
	int height=lb->fontMetrics().lineSpacing()+3;
	return QMAX(pm.height(), height);
}


int KaduListBoxPixmap::width(const Q3ListBox* lb) const
{
	if (ColumnCount == 0)
		return QMAX(pm.width(), (lb->visibleWidth()));
	else
		return QMAX(pm.width(), (lb->visibleWidth()) / ColumnCount);
}

//#include <sys/time.h>
void KaduListBoxPixmap::calculateSize(const QString &text, int width, QStringList &out, int &height) const
{
	if (!descriptionFontMetrics)
		return;

//	kdebugf();
	if (text == buf_text && width == buf_width)	// we already computed it
	{
		out=buf_out;
		height=buf_height;
		return;
	}

	int tmplen;

	out.clear();
	height=0;

/*	struct timeval t1,t2;
	gettimeofday(&t1, NULL);
	for(int j=0; j<1000; ++j)
	{
		out.clear();
		height=0;
*/
	QStringList tmpout = QStringList::split('\n', text, true);

	int wsize = descriptionFontMetrics->width('W'); //'w' is the widest letter
	int initialLength = width / wsize; // try to guess width

	if (initialLength < 1) // we are all doomed ;) there is no space fo even one letter
	{
		kdebugm(KDEBUG_WARNING, "no space for description!\n");
		height = 0;
		out = QStringList();
		return;
	}

	while (!tmpout.isEmpty())
	{
		QString curtext = tmpout.front();
		int textlen = curtext.length();
		int len = initialLength;
		bool tooWide = false;
		while (1)
		{
			tooWide = (descriptionFontMetrics->width(curtext.left(len)) >= width);
			if (!tooWide && len < textlen)
				len += 5; //moving with 5 letters to make it faster
			else
				break;
		}
		if (tooWide) // crossed userbox width
		{
			while (descriptionFontMetrics->width(curtext.left(len)) >= width) //shortening to find the widest length
				--len;
			tmplen = len;
			while (len > 0 && !curtext[len-1].isSpace()) //find word boundary
				--len;
			if (len == 0) //but maybe someone wrote it without spaces?
				len = tmplen-1;
		}
		if (len < 1)
		{
			kdebugm(KDEBUG_WARNING, "no space for description ! (2)\n");
			height = 0;
			out = QStringList();
			return;
		}
		QString next = curtext.mid(len); //moving rest to the next line
		out.push_back(curtext.left(len));
		tmpout.pop_front();
		++height;
		if (tooWide)
		{
			if (next[0].isSpace()) // if we are breaking line at word boundary, next line can be truncated at beginning
				tmpout.push_front(next.mid(1));
			else
				tmpout.push_front(next);
		}
	}

/*	}
	gettimeofday(&t2, NULL);
	kdebugm(KDEBUG_INFO, "czas: %ld\n", (t2.tv_usec-t1.tv_usec)+(t2.tv_sec*1000000)-(t1.tv_sec*1000000));
*/
	height*=descriptionFontMetrics->lineSpacing();

	buf_text=text;
	buf_width=width;
	buf_out=out;
	buf_height=height;
//	kdebugm(KDEBUG_DUMP, "h:%d txt:%s\n", height, qPrintable(text));
//	for(QStringList::Iterator it = out.begin(); it != out.end(); ++it )
//		kdebugm(KDEBUG_DUMP, ">>%s\n", qPrintable(*it));
}

void KaduListBoxPixmap::changeText(const QString &text)
{
	setText(text);
}

QPixmap KaduListBoxPixmap::pixmapForUser(Contact contact)
{
	Account *account = AccountManager::instance()->defaultAccount();

	bool has_mobile = !contact.mobile().isEmpty();

	if (!contact.accountData(account))
	{
		if (has_mobile)
			return icons_manager->loadPixmap("Mobile");
		else if (!contact.email().isEmpty())
			return icons_manager->loadPixmap("WriteEmail");
		else
			return QPixmap();
	}
	else if (pending.pendingMsgs(contact))
		return icons_manager->loadPixmap("Message");
	else
	{
		Status status = contact.accountData(account)->status();
		const QPixmap &pix = account->statusPixmap(status);

		if (!pix.isNull())
			return pix;
		else
			return icons_manager->loadPixmap("Offline");
	}
}

void KaduListBoxPixmap::refreshItem()
{
	pm = pixmapForUser(CurrentContact);
	changeText(CurrentContact.display());
}

class ULEComparer
{
	public:
		inline bool operator()(const Contact &e1, const Contact &e2) const;
		QList<UserBox::CmpFuncDesc> CmpFunctions;
		ULEComparer() : CmpFunctions() {}
};

inline bool ULEComparer::operator()(const Contact &e1, const Contact &e2) const
{
	int ret = 0;
	foreach(const UserBox::CmpFuncDesc &f, CmpFunctions)
	{
		ret = f.func(e1, e2);
//		kdebugm(KDEBUG_WARNING, "%s %s %d\n", qPrintable(e1.altNick()), qPrintable(e2.altNick()), ret);
		if (ret)
			break;
	}
	return ret < 0;
}

//TODO 0.6.6: workaround for: http://www.kadu.net/mantis/view.php?id=1108
class ScrollBarWatcher : public QObject
{
public:
	ScrollBarWatcher()
	{
	}

	virtual bool eventFilter(QObject *o, QEvent *e)
	{
		if (Kadu::closing())
			return false;
		QScrollBar *scrollbar = dynamic_cast<QScrollBar *>(o);
		if (!scrollbar)
			return false;
		UserBox *userbox = dynamic_cast<UserBox *>(scrollbar->parent());
		if (!userbox)
			return false;

		switch (e->type())
		{
			case QEvent::Show:
			case QEvent::Hide:
				userbox->refresh();
			default:
				return false;
		}
	}
};
ScrollBarWatcher *scrollBarWatcher = 0;
//

CreateNotifier UserBox::createNotifier;

QList<ActionDescription *> UserBox::UserBoxActions;
QList<ActionDescription *> UserBox::ManagementActions;

UserBox::UserBox(KaduMainWindow *mainWindow, bool fancy, ContactList contacts, QWidget* parent, const char* name, Qt::WFlags f)
	: Q3ListBox(parent, name, f), MainWindow(mainWindow), fancy(fancy),
	Contacts(contacts), sortHelper(), toRemove(), AppendProxy(), RemoveProxy(), comparer(new ULEComparer()),
	refreshTimer(), lastMouseStopContact(Contact::null), tipTimer(),
	verticalPositionTimer(), lastVerticalPosition(0)
{
	kdebugf();

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account *)),
			this, SLOT(accountRegistered(Account *)));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account *)),
			this, SLOT(accountUnregistered(Account *)));
	foreach (Account *account, AccountManager::instance()->accounts())
		accountRegistered(account);

	setHScrollBarMode(Q3ScrollView::AlwaysOff);

	//TODO 0.6.6: workaround for: http://www.kadu.net/mantis/view.php?id=1108
	if (!scrollBarWatcher)
		scrollBarWatcher = new ScrollBarWatcher();
	verticalScrollBar()->installEventFilter(scrollBarWatcher);
	//

	showDescriptionAction = new ActionDescription(
		ActionDescription::TypeUserList, "descriptionsAction",
		this, SLOT(showDescriptionsActionActivated(QAction *, bool)),
		"ShowDescription", tr("Hide descriptions"),
		true, tr("Show descriptions")
	);
	connect(showDescriptionAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(setDescriptionsActionState()));

	setDescriptionsActionState();

	connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChangedSlot()));
/*
	connect(group, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAddedToGroup(UserListElement, bool, bool)));
	connect(group, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userRemovedFromGroup(UserListElement, bool, bool)));
*/
/*
	connect(VisibleUsers, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAddedToVisible(UserListElement, bool, bool)));
	connect(VisibleUsers, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userRemovedFromVisible(UserListElement, bool, bool)));
	connect(VisibleUsers, SIGNAL(usersStatusChanged(QString)), this, SLOT(refreshLater()));
	connect(VisibleUsers, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)));
	connect(VisibleUsers, SIGNAL(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)));
	connect(VisibleUsers, SIGNAL(removingProtocol(UserListElement, QString, bool, bool)),
			this, SLOT(removingProtocol(UserListElement, QString, bool, bool)));

	VisibleUsers->addUsers(group);
*/

	addCompareFunction("Status", tr("Statuses"), compareStatus);
	if (brokenStringCompare)
		addCompareFunction("AltNick", tr("Nicks, case insensitive"), compareAltNickCaseInsensitive);
	else
		addCompareFunction("AltNick", tr("Nicks"), compareAltNick);

	UserBoxes.append(this);

	setMinimumWidth(20);
	setSelectionMode(Q3ListBox::Extended);

	connect(this, SIGNAL(doubleClicked(Q3ListBoxItem *)), this, SLOT(doubleClickedSlot(Q3ListBoxItem *)));
	connect(this, SIGNAL(returnPressed(Q3ListBoxItem *)), this, SLOT(returnPressedSlot(Q3ListBoxItem *)));
	connect(this, SIGNAL(currentChanged(Q3ListBoxItem *)), this, SLOT(currentChangedSlot(Q3ListBoxItem *)));
	connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));

	connect(&pending, SIGNAL(messageFromUserAdded(Contact)), this, SLOT(messageFromUserAdded(Contact)));
	connect(&pending, SIGNAL(messageFromUserDeleted(Contact)), this, SLOT(messageFromUserAdded(Contact)));

	connect(&tipTimer, SIGNAL(timeout()), this, SLOT(tipTimeout()));

	connect(&verticalPositionTimer, SIGNAL(timeout()), this, SLOT(resetVerticalPosition()));
	connect(kadu, SIGNAL(shown()), this, SLOT(resetVerticalPosition()));
	connect(kadu, SIGNAL(hiding()), this, SLOT(rememberVerticalPosition()));

	configurationUpdated();

	kdebugf2();
}

UserBox::~UserBox()
{
	kdebugf();

// 	disconnect(VisibleUsers, SIGNAL(userRemoved(UserListElement, bool, bool)),
// 			this, SLOT(userRemovedFromVisible(UserListElement, bool, bool)));

	UserBoxes.remove(this);
	
// 	delete VisibleUsers;
// 	VisibleUsers = 0;
	delete comparer;
	comparer = 0;

	kdebugf2();
}

#define TIP_TM 1000

void UserBox::tipTimeout()
{
	if (!lastMouseStopContact.isNull())
	{
		tool_tip_class_manager->showToolTip(QCursor().pos(), lastMouseStopContact);
		tipTimer.stop();
	}
}

void UserBox::restartTip(const QPoint &p)
{
//	kdebugf();
	KaduListBoxPixmap *item = static_cast<KaduListBoxPixmap *>(itemAt(p));
	if (item)
	{
		if (item->CurrentContact != lastMouseStopContact)
			hideTip();
		lastMouseStopContact = item->CurrentContact;
	}
	else
	{
		hideTip();
		lastMouseStopContact = Contact::null;
	}
	tipTimer.start(TIP_TM);
//	kdebugf2();
}

void UserBox::hideTip(bool waitForAnother)
{
	tool_tip_class_manager->hideToolTip();

	if (waitForAnother)
		tipTimer.start(TIP_TM);
	else
		tipTimer.stop();
}

void UserBox::showDescriptionsActionActivated(QAction *sender, bool toggle)
{
	config_file.writeEntry("Look", "ShowDesc", !toggle);
	KaduListBoxPixmap::setShowDesc(!toggle);
	UserBox::refreshAllLater();
	setDescriptionsActionState();
}

void UserBox::setDescriptionsActionState()
{
	foreach (KaduAction *action, showDescriptionAction->actions())
		action->setChecked(!KaduListBoxPixmap::ShowDesc);
}

void UserBox::wheelEvent(QWheelEvent *e)
{
//	kdebugf();
	Q3ListBox::wheelEvent(e);

	// if event source (e->globalPos()) is inside this widget (QRect(...))
	if (QRect(mapToGlobal(QPoint(0,0)), size()).contains(e->globalPos()))
		restartTip(e->pos());
	else
		hideTip(false);
}

void UserBox::enterEvent(QEvent *e)
{
//	kdebugf();
	Q3ListBox::enterEvent(e);
}

void UserBox::leaveEvent(QEvent *e)
{
//	kdebugf();
	hideTip(false);
	Q3ListBox::leaveEvent(e);
}

void UserBox::mousePressEvent(QMouseEvent *e)
{
	kdebugf();
	hideTip(false);
	if (e->button() != Qt::RightButton)
	{
		MouseStart = e->pos();
		Q3ListBox::mousePressEvent(e);
	}
	else
	{
		Q3ListBoxItem *item = itemAt(e->pos());
		if (item)
		{
			if (!item->isSelected())
				if (!(e->state() & Qt::ControlButton))
					for (unsigned int i = 0, count2 = count(); i < count2; ++i)
						setSelected(i, FALSE);
			setSelected(item, TRUE);
			setCurrentItem(item);
			emit rightButtonPressed(item, e->globalPos());
		}
	}
	kdebugf2();
}

void UserBox::mouseReleaseEvent(QMouseEvent *e)
{
	Q3ListBox::mouseReleaseEvent(e);
	restartTip(e->pos());
}

void UserBox::mouseMoveEvent(QMouseEvent* e)
{
//	kdebugf();
	if ((e->buttons() & Qt::LeftButton) && itemAt(MouseStart) && (e->pos() - MouseStart).manhattanLength() > QApplication::startDragDistance())
	{
		QStringList ules;
		for(int i = 0, count1 = count(); i < count1; ++i)
			if (isSelected(i))
				ules.append(item(i)->text());

		QDrag* drag = new UlesDrag(ules, this);
		drag->exec(Qt::CopyAction);
	}
	else
	{
		Q3ListBox::mouseMoveEvent(e);
		restartTip(e->pos());
	}
//	kdebugf2();
}

void UserBox::keyPressEvent(QKeyEvent *e)
{
//	kdebugf();
	hideTip(false);
	Q3ListBox::keyPressEvent(e);
//	QWidget::keyPressEvent(e);
	Q3ListBoxItem *i = item(currentItem());
	currentChangedSlot(i);
//	kdebugf2();
}

//#include <sys/time.h>
void UserBox::refresh()
{
	kdebugf();
/*	printBacktrace("UserBox::refresh()");

	struct timeval t1,t2;
	gettimeofday(&t1, NULL);
	for(int j=0; j<1000; ++j)
	{
*/
	sort();

	const unsigned int Count = count();
	unsigned int i = 0;

	bool doRefresh = true;

	if (fancy && (numColumns() != config_file.readNumEntry("Look", "UserBoxColumnCount", 1)))
		doRefresh = true;

	if (!doRefresh && (sortHelper.size() == Count))
	{
		kdebugm(KDEBUG_INFO, "checking if order changed\n");
		for (std::vector<Contact>::const_iterator contact = sortHelper.begin(),
			contactEnd = sortHelper.end(); contact != contactEnd; ++contact)
		{
			doRefresh = ((*contact) != static_cast<KaduListBoxPixmap *>(item(i++))->CurrentContact);
			if (doRefresh)
				break;
		}

		if (!doRefresh)
		{
			for (i = 0; i < Count; ++i)
				static_cast<KaduListBoxPixmap *>(item(i))->refreshItem();
			triggerUpdate(true);

			kdebugf2();
			return;
		}
	}

	kdebugm(KDEBUG_INFO, "do real refresh\n");

	// remember selected users
	QStringList s_users;
	for (i = 0; i < Count; ++i)
		if (isSelected(i))
			s_users.append(item(i)->text());
	QString s_user = currentText();

	// remember vertical scrollbar position
	int vScrollValue = verticalScrollBar()->value();

	// clearing list
	Q3ListBox::clear();
	// clear clears columns too...
	if (fancy)
		setColumnMode(config_file.readNumEntry("Look", "UserBoxColumnCount", 1));
/*
	for (std::vector<Contact>::const_iterator contact = sortHelper.begin(),
		contactEnd = sortHelper.end(); contact != contactEnd; ++contact)*/

	foreach (Contact contact, Contacts)
		insertItem(new KaduListBoxPixmap(contact));

	// restore selected users
	foreach(const QString &username, s_users)
		setSelected(findItem(username), true);
	setCurrentItem(findItem(s_user));

	// restore vertical scrollbar position
	verticalScrollBar()->setValue(vScrollValue);

	// because settingCurrentItem changes vertical scrollbar position and line
	// above doesn't prevents this, we must set position as soon as possible
	lastVerticalPosition = vScrollValue;

	verticalPositionTimer.start(0, true);

//	QListBox::refresh();

//	}
//	gettimeofday(&t2, NULL);
//	kdebugm(KDEBUG_INFO, "czas: %ld\n", (t2.tv_usec-t1.tv_usec)+(t2.tv_sec*1000000)-(t1.tv_sec*1000000));

	kdebugf2();
}

void UserBox::resetVerticalPosition()
{
	kdebugf();

	verticalScrollBar()->setValue(lastVerticalPosition);

	updateScrollBars();
}

void UserBox::rememberVerticalPosition()
{
	lastVerticalPosition = verticalScrollBar()->value();
}

UserListElements UserBox::selectedUsers() const
{
	kdebugf();

	ContactList contacts;
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
		if (isSelected(i))
			contacts.append(static_cast<KaduListBoxPixmap *>(item(i))->CurrentContact);

	kdebugf2();
	return UserListElements::fromContactList(contacts, AccountManager::instance()->defaultAccount());
}

void UserBox::selectionChangedSlot()
{	
	if (!Kadu::closing())
		emit userListChanged();
}

void UserBox::refreshLater()
{
	refreshTimer.start(0, true);
}

void UserBox::refreshAll()
{
	kdebugf();
	foreach(UserBox *box, UserBoxes)
		box->refresh();
	kdebugf2();
}

void UserBox::refreshAllLater()
{
	kdebugf();
	foreach(UserBox *box, UserBoxes)
		box->refreshLater();
	kdebugf2();
}

void UserBox::messageFromUserAdded(Contact elem)
{
// 	if (visibleUsers()->contains(UserListElement::fromContact(elem, AccountManager::instance()->defaultAccount())))
// 		refreshLater();
}

void UserBox::messageFromUserDeleted(Contact elem)
{
// 	if (visibleUsers()->contains(UserListElement::fromContact(elem, AccountManager::instance()->defaultAccount())))
// 		refreshLater();
}

void UserBox::closeModule()
{
	kdebugf();

	delete KaduListBoxPixmap::descriptionFontMetrics;
	KaduListBoxPixmap::descriptionFontMetrics = 0;

	delete tool_tip_class_manager;
	tool_tip_class_manager = 0;

	kdebugf2();
}

void UserBox::initModule()
{
	kdebugf();

	tool_tip_class_manager = new ToolTipClassManager();
	tool_tip_class_manager->useToolTipClass(config_file.readEntry("Look", "UserboxToolTipStyle"));

	QStringList options;
	QStringList values;

	userlist->addPerContactNonProtocolConfigEntry("hide_description", "HideDescription");

	brokenStringCompare = (QString("a").localeAwareCompare(QString("B")) > 0);
	if (brokenStringCompare)
		fprintf(stderr, "There's something wrong with native string compare function. Applying workaround (slower).\n");

	kdebugf2();
}

void UserBox::resizeEvent(QResizeEvent *r)
{
//	kdebugf();
	Q3ListBox::resizeEvent(r);
	refreshBackground();
	triggerUpdate(true);
}

void UserBox::refreshBackground()
{
	//hack:
	viewport()->setStyleSheet(QString("QWidget {background-color:%1}").arg(config_file.readColorEntry("Look","UserboxBgColor").name()));
	setStyleSheet(QString("QFrame {color:%1}").arg(config_file.readColorEntry("Look","UserboxFgColor").name()));
	
	/*	
	TODO 0.6.6: dead code. Re-Enable in 0.6.6
	
	setStaticBackground(backgroundImage);
	
	if (!backgroundImage || !fancy)
		return;

	QImage image;
	QString type = config_file.readEntry("Look", "UserboxBackgroundDisplayStyle");

	if (type == "Stretched")
	{
		int userboxWidth = width();
		int userboxHeight = height();

		image = backgroundImage->smoothScale(userboxWidth, userboxHeight);
	}
	else if (type == "Centered")
	{
		int userboxWidth = width();
		int userboxHeight = height();

		int width = backgroundImage->width();
		int height = backgroundImage->height();

		if (!width || !height)
			return;

		int xOffset = (userboxWidth / 2) % width - (width / 2);
		int yOffset = (userboxHeight / 2) % height - (height / 2);

		image.create(width, height, 32);
		bitBlt(&image, 0, 0, backgroundImage, width - xOffset, height - yOffset, xOffset, yOffset);
		bitBlt(&image, xOffset, yOffset, backgroundImage, 0, 0, width - xOffset, height - yOffset);
		bitBlt(&image, 0, yOffset, backgroundImage, width - xOffset, 0, xOffset, height - yOffset);
		bitBlt(&image, xOffset, 0, backgroundImage, 0, height - yOffset, width - xOffset, yOffset);
	}
	else // TILED
		image = *backgroundImage;

 	viewport()->setPaletteBackgroundPixmap(QPixmap::fromImage(image));
	*/
}

void UserBox::doubleClickedSlot(Q3ListBoxItem *item)
{
	emit doubleClicked(static_cast<KaduListBoxPixmap *>(item)->CurrentContact);
}

void UserBox::returnPressedSlot(Q3ListBoxItem *item)
{
	emit returnPressed(static_cast<KaduListBoxPixmap *>(item)->CurrentContact);
}

void UserBox::currentChangedSlot(Q3ListBoxItem *item)
{
	/*
	TODO 0.6.6: obejście dziwnego problemu z Q3ListBox.
	Pomimo kilknięcia, currentItem() wskazuje na poprzednio aktywny element.
	Blokujemy sygnał, aby ponownie nie wykonywać tej samem metody 
	*/
	blockSignals(true);
	setCurrentItem(item);
	blockSignals(false);
	//
	if (item)
		emit currentChanged(static_cast<KaduListBoxPixmap *>(item)->CurrentContact);
}

void UserBox::addActionDescription(ActionDescription *actionDescription)
{
	UserBoxActions.append(actionDescription);
}

void UserBox::insertActionDescription(int pos, ActionDescription *actionDescription)
{
	UserBoxActions.insert(pos, actionDescription);
}

void UserBox::removeActionDescription(ActionDescription *actionDescription)
{
	UserBoxActions.removeAll(actionDescription);
}

void UserBox::addSeparator()
{
	UserBoxActions.append(0);
}

void UserBox::addManagementActionDescription(ActionDescription *actionDescription)
{
	ManagementActions.append(actionDescription);
}

void UserBox::insertManagementActionDescription(int pos, ActionDescription *actionDescription)
{
	ManagementActions.insert(pos, actionDescription);
}

void UserBox::removeManagementActionDescription(ActionDescription *actionDescription)
{
	ManagementActions.removeAll(actionDescription);
}

void UserBox::addManagementSeparator()
{
	ManagementActions.append(0);
}

void UserBox::configurationUpdated()
{
	kdebugf();

	int columnCount = config_file.readNumEntry("Look", "UserBoxColumnCount", 1);
	setColumnMode(columnCount);

	Q3ListBox::setFont(config_file.readFontEntry("Look", "UserboxFont"));

	KaduListBoxPixmap::setFont(config_file.readFontEntry("Look", "UserboxFont"));
	KaduListBoxPixmap::setShowDesc(config_file.readBoolEntry("Look", "ShowDesc"));
	KaduListBoxPixmap::setShowBold(config_file.readBoolEntry("Look", "ShowBold"));
	KaduListBoxPixmap::setAlignTop(config_file.readBoolEntry("Look", "AlignUserboxIconsTop"));
	KaduListBoxPixmap::setShowMultilineDesc(config_file.readBoolEntry("Look", "ShowMultilineDesc"));
	KaduListBoxPixmap::setColumnCount(columnCount);
	KaduListBoxPixmap::setMyUIN(config_file.readUnsignedNumEntry("General", "UIN"));
	KaduListBoxPixmap::setDescriptionColor(config_file.readColorEntry("Look", "DescriptionColor"));

	tool_tip_class_manager->useToolTipClass(config_file.readEntry("Look", "UserboxToolTipStyle"));

	UserBox::setColorsOrBackgrounds();
	UserBox::setDescriptionsActionState();

	UserBox::refreshAllLater();

	kdebugf2();
}


void UserBox::setColorsOrBackgrounds()
{
	/*
	TODO: 0.6.6:	
	QImage *newImage = 0;

	if (config_file.readBoolEntry("Look", "UseUserboxBackground", true))
	{
		QString s = config_file.readEntry("Look", "UserboxBackground");
		if (!s.isEmpty() && QFile::exists(s))
			newImage = new QImage(s);
	}

	if (newImage)
	{
		if (backgroundImage)
			delete backgroundImage;

		backgroundImage = newImage;
	}
	*/
	foreach(UserBox *userbox, UserBoxes)
		userbox->refreshBackground();
}

QList<UserBox *> UserBox::UserBoxes;

QList<UserBox::CmpFuncDesc> UserBox::compareFunctions() const
{
	return comparer->CmpFunctions;
}
/*
void UserBox::applyFilter(UserGroup *g, bool forceRefresh)
{
	kdebugf();
	if (Filters.contains(g))
		return;
	Filters.append(g);
	UserListElements users;
	foreach(const UserListElement &user, *VisibleUsers)
		if (!g->contains(user))
			users.append(user);
	VisibleUsers->removeUsers(users);
	connect(g, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAddedToGroup(UserListElement, bool, bool)));
	connect(g, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userRemovedFromGroup(UserListElement, bool, bool)));
	if (forceRefresh)
	{
		refresh();
		emit selectionChanged();
	}
	kdebugf2();
}

void UserBox::removeFilter(UserGroup *g, bool forceRefresh)
{
	kdebugf();
	if (!Filters.contains(g))
		return;
	Filters.remove(g);
	disconnect(g, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAddedToGroup(UserListElement, bool, bool)));
	disconnect(g, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userRemovedFromGroup(UserListElement, bool, bool)));
	if (Filters.isEmpty()) // there must be at least one group
		Filters.append(userlist);
	UserGroup *last = Filters.last();
	Filters.pop_back(); //temporarily removing

	UserListElements users;
	foreach(const UserListElement &user, *last)
	{
		if (VisibleUsers->contains(user)) // we are not looking for contacts which are certain
			continue;
		bool omit = false;

		foreach(UserGroup *ngroup, NegativeFilters)
			if (ngroup->contains(user))
			{
				omit = true;
				break;
			}
		if (omit)
			continue;

		foreach(UserGroup *group, Filters)
			if (!group->contains(user))
			{
				omit = true;
				break; // if belongs to any group, there is no point in checking further
			}
		if (omit)
			continue;

		users.append(user);
	}
	Filters.append(last); // restoring
	VisibleUsers->addUsers(users);
	if (forceRefresh)
	{
		refresh();
		emit selectionChanged();
	}
	kdebugf2();
}

void UserBox::applyNegativeFilter(UserGroup *g, bool forceRefresh)
{
	kdebugf();
	if (NegativeFilters.contains(g))
		return;
	NegativeFilters.append(g);
	UserListElements users;
	foreach(const UserListElement &user, *VisibleUsers)
		if (g->contains(user))
			users.append(user);
	VisibleUsers->removeUsers(users);
	connect(g, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userRemovedFromGroup(UserListElement, bool, bool)));
	connect(g, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userAddedToGroup(UserListElement, bool, bool)));
	if (forceRefresh)
	{
		refresh();
		emit selectionChanged();
	}
	kdebugf2();
}

void UserBox::removeNegativeFilter(UserGroup *g, bool forceRefresh)
{
	kdebugf();
	if (!NegativeFilters.contains(g))
		return;
	NegativeFilters.remove(g);
	disconnect(g, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userRemovedFromGroup(UserListElement, bool, bool)));
	disconnect(g, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userAddedToGroup(UserListElement, bool, bool)));

	UserListElements users;
	foreach(const UserListElement &user, *g)
	{
		bool omit = false;
		foreach(UserGroup *ngroup, NegativeFilters)
			if (ngroup->contains(user))
			{
				omit = true;
				break;
			}
		if (omit)
			continue;

		foreach(UserGroup *group, Filters)
			if (!group->contains(user))
			{
				omit = true;
				break;
			}
		if (omit)
			continue;

		users.append(user);
	}
	VisibleUsers->addUsers(users);
	if (forceRefresh)
	{
		refresh();
		emit selectionChanged();
	}
	kdebugf2();
}
*/
void UserBox::addCompareFunction(const QString &id, const QString &trDescription,
			int (*cmp)(const Contact &, const Contact &))
{
	comparer->CmpFunctions.append(CmpFuncDesc(id, trDescription, cmp));
	refreshLater();
}

void UserBox::removeCompareFunction(const QString &id)
{
	foreach(const CmpFuncDesc &c, comparer->CmpFunctions)
		if (c.id == id)
		{
			comparer->CmpFunctions.remove(c);
			refreshLater();
			break;
		}
}

bool UserBox::moveUpCompareFunction(const QString &id)
{
	kdebugf();
	CmpFuncDesc d;
	int pos = 0;
	bool found = false;
	QList<CmpFuncDesc>::iterator c;
	for (c = comparer->CmpFunctions.begin(); c != comparer->CmpFunctions.end(); ++c)
	{
 		if ((*c).id == id)
 		{
 			found = true;
 			if (pos == 0)
 				break;
 			d = *c;
 			--c;
 			c = comparer->CmpFunctions.insert(c, d);
 			c += 2;
 			comparer->CmpFunctions.remove(c);
 			refreshLater();
 			break;
 		}
 		++pos;
	}
	kdebugf2();
	return found;
}

bool UserBox::moveDownCompareFunction(const QString &id)
{
	kdebugf();
	CmpFuncDesc d;
	int pos = 0;
	int cnt = comparer->CmpFunctions.count();
	bool found = false;
	QList<CmpFuncDesc>::iterator c;
	for (c = comparer->CmpFunctions.begin(); c != comparer->CmpFunctions.end(); ++c)
	{
 		if ((*c).id == id)
 		{
 			found = true;
 			if (pos == cnt - 1)
 				break;
 			d = *c;
 			++c;
 			c = comparer->CmpFunctions.insert(c, d);
 			c -= 2;
 			comparer->CmpFunctions.remove(c);
 			refreshLater();
 			break;
 		}
 		++pos;
	}
	kdebugf2();
	return found;
}

void UserBox::sort()
{
//	FOREACH(u, sortHelper)
//		kdebugm(KDEBUG_WARNING, ">>%s\n", qPrintable((*u).altNick()));
	std::sort(sortHelper.begin(), sortHelper.end(), *comparer);
//	FOREACH(u, sortHelper)
//		kdebugm(KDEBUG_ERROR, ">>%s\n", qPrintable((*u).altNick()));
}

void UserBox::removingProtocol(UserListElement /*elem*/, QString /*protocolName*/, bool /*massively*/, bool /*last*/)
{
	//_removing_ protocol, so it isn't actually removed -> refreshing _Later_ (when protocol will be removed)
	refreshLater();
}

void UserBox::userDataChanged(UserListElement /*elem*/, QString name, QVariant /*oldValue*/,
					QVariant /*currentValue*/, bool /*massively*/, bool /*last*/)
{
	if (name != "AltNick" && name != "Mobile" && name != "Email" && name != "HideDescription") // we are not interested in other names
		return;
	refreshLater();
}

void UserBox::protocolUserDataChanged(QString protocolName, UserListElement /*elem*/,
					QString name, QVariant /*oldValue*/, QVariant /*currentValue*/,
					bool /*massively*/, bool /*last*/)
{
	if (protocolName != "Gadu")
		return;
	if (name != "Blocking" && name != "OfflineTo")
		return;
	refreshLater();
}


// void UserBox::userAddedToVisible(UserListElement elem, bool /*massively*/, bool /*last*/)
// {
//	kdebugmf(KDEBUG_FUNCTION_START, "start: mass:\n", massively);
// 	lastMouseStopContact = Contact::null;
// 	sortHelper.push_back(elem.toContact(AccountManager::instance()->defaultAccount()));
// 	refreshLater();
// }

class torem
{
	std::vector<Contact>::const_iterator begin;
	std::vector<Contact>::const_iterator end;
//	std::vector<UserListElement>::const_iterator last;

	public:
	torem(const std::vector<Contact> &src) : begin(src.begin()), end(src.end())
	{
//		last = begin;
	}

	inline bool operator()(const Contact &u) const
	{
		for(std::vector<Contact>::const_iterator it = begin; it != end; ++it)
			if ((*it) == u)
				return true;
/*		std::vector<UserListElement>::const_iterator lastlast = last;
		for(; last != end; ++last)
			if ((*last) == u)
				return true;
		for(last = begin; last != lastlast; ++last)
			if ((*last) == u)
				return true;
*/
		return false;
	}
};

// void UserBox::userRemovedFromVisible(UserListElement elem, bool massively, bool last)
// {
// //	kdebugmf(KDEBUG_FUNCTION_START, "start: mass:%d\n", massively);
// 	lastMouseStopContact = Contact::null;
// 	Contact contact = elem.toContact();
// 
// 	if (massively)
// 		toRemove.push_back(contact);
// 	else
// 		sortHelper.erase(std::remove(sortHelper.begin(), sortHelper.end(), contact), sortHelper.end());// the most optimal
// 	if (massively && last)
// 	{
// 		torem pred(toRemove);
// 		sortHelper.erase(std::remove_if(sortHelper.begin(), sortHelper.end(), pred), sortHelper.end());
// 		toRemove.clear();
// 	}
// 	if (massively)
// 		refreshLater();
// 	else
// 		refresh();
// //	kdebugf2();
// }
/*
void UserBox::userAddedToGroup(UserListElement elem, bool massively, bool last)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: mass:%d\n", massively);
	const UserGroup *s = static_cast<const UserGroup *>(sender());
//	kdebugm(KDEBUG_INFO, "sender name: '%s'\n", s->name());
	bool append = true;
	foreach(UserGroup *group, NegativeFilters)
		if (group->contains(elem))
		{
			append = false;
			break;
		}
//	kdebugm(KDEBUG_WARNING, "%d %d %d\n", append, massively, last);
	if (append)
		foreach(UserGroup *group, Filters)
			if (!group->contains(elem))
			{
				append = false;
				break;
			}
//	kdebugm(KDEBUG_WARNING, "%d\n", append);
	if (append)
	{
		if (massively)
			AppendProxy[s].append(elem);
		else
			VisibleUsers->addUser(elem);
	}
	if (massively && last)
	{
		VisibleUsers->addUsers(AppendProxy[s]);
		AppendProxy.remove(s);
	}
	kdebugf2();
}

void UserBox::userRemovedFromGroup(UserListElement elem, bool massively, bool last)
{
//	kdebugmf(KDEBUG_FUNCTION_START, "start: mass:%d\n", massively);
	const UserGroup *s = static_cast<const UserGroup *>(sender());
//	kdebugm(KDEBUG_INFO, "sender name: '%s'\n", s->name());
	if (s == userlist) //HACK :/ ignore removal of user from main userlist, because userlist didn't really removed that user... (look at UserGroup::removeUser())
	{
//		kdebugf2();
		return;
	}
	if (VisibleUsers->contains(elem))
		if (massively)
			RemoveProxy[s].append(elem);
		else
			VisibleUsers->removeUser(elem);
	if (massively && last)
	{
		VisibleUsers->removeUsers(RemoveProxy[s]);
		RemoveProxy.remove(s);
	}
//	kdebugf2();
}
*/
void UserBox::contextMenuEvent(QContextMenuEvent *event)
{
	if (!itemAt(event->pos()))
		return;

	QMenu *menu = new QMenu(this);

	foreach (ActionDescription *actionDescription, UserBox::UserBoxActions)
	{
		if (actionDescription)
		{
			KaduAction *action = actionDescription->createAction(MainWindow);
			menu->addAction(action);
			action->checkState();
		}
		else
			menu->addSeparator();
	}

	QMenu *management = menu->addMenu(tr("User management"));

	foreach (ActionDescription *actionDescription, UserBox::ManagementActions)
		if (actionDescription)
		{
			KaduAction *action = actionDescription->createAction(MainWindow);
			management->addAction(action);
			action->checkState();
		}
		else
			management->addSeparator();

	menu->exec(event->globalPos());
}
/*
const UserGroup *UserBox::visibleUsers() const
{
	return VisibleUsers;
}*/
/*
QList<UserGroup *> UserBox::filters() const
{
	return Filters;
}

QList<UserGroup *> UserBox::negativeFilters() const
{
	return NegativeFilters;
}*/

bool UserBox::currentUserExists() const
{
	return currentItem() != -1;
}

Contact UserBox::currentContact() const
{
	Q3ListBoxItem *i = item(currentItem());
	if (i)
		return static_cast<KaduListBoxPixmap *>(i)->CurrentContact;
	else
	{
		kdebugm(KDEBUG_ERROR, "GO AWAY and check currentUserExists() first!\n");
		printBacktrace("currentUser");
		return Contact::null;
	}
}

UlesDrag::UlesDrag(const QStringList &ules, QWidget* dragSource)
	: QDrag(dragSource)
{
	kdebugf();

	QByteArray data;
	QMimeData *mimeData = new QMimeData;
	QString allUles = ules.join("\n");

	data = allUles.toUtf8(); 

     	mimeData->setData("application/x-kadu-ules", data);

	setMimeData(mimeData);

	kdebugf2();
}

bool UlesDrag::decode(QDropEvent *event, QStringList &ules)
{
	const QMimeData *mimeData = event->mimeData();

	if (!mimeData->hasFormat("application/x-kadu-ules"))
		return false;

	QTextStream stream(mimeData->data("application/x-kadu-ules"), QIODevice::ReadOnly);
	stream.setCodec("UTF-8");

	QString allUles = stream.readAll();

	ules = QStringList::split("\n", allUles);

	return ules.count() > 0;
}

bool UlesDrag::canDecode(QDragEnterEvent *event)
{
	return event->mimeData()->hasFormat("application/x-kadu-ules");
}

inline int compareAltNick(const Contact &c1, const Contact &c2)
{
	return c1.display().localeAwareCompare(c2.display());
}

inline int compareAltNickCaseInsensitive(const Contact &c1, const Contact &c2)
{
	return c1.display().lower().localeAwareCompare(c2.display().lower());
}

inline int compareStatus(const Contact &c1, const Contact &c2)
{
	Account *account = AccountManager::instance()->defaultAccount();

	Contact cc1;
	Contact cc2;

	if (!c1.isNull())
		cc1 = c1;
	if (!c2.isNull())
		cc2 = c2;

	ContactAccountData *d1 = cc1.accountData(account);
	ContactAccountData *d2 = cc2.accountData(account);

	if (d1 && d2)
		return d2->status().compareTo(d1->status());
	else
		return (long int)d2 - (long int)d1;
}

void UserBox::accountRegistered(Account *account)
{
	printf("conencting ...\n");
	connect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(refresh()));
}

void UserBox::accountUnregistered(Account *account)
{
	disconnect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(refresh()));
}


ToolTipClassManager *tool_tip_class_manager = 0;

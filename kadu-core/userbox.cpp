/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>

#include <qapplication.h>
#include <qcheckbox.h>
#include <qcursor.h>
#include <qcombobox.h>
#include <qdragobject.h>
#include <qpainter.h>
#include <qspinbox.h>
#include <qtextcodec.h>
#include <qvgroupbox.h>

#include "action.h"
#include "config_file.h"
#include "debug.h"
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
bool KaduListBoxPixmap::AlignUserboxIconsTop;
bool KaduListBoxPixmap::ShowMultilineDesc;
int KaduListBoxPixmap::ColumnCount;
QColor KaduListBoxPixmap::descColor;
UserListElement UserBox::nullElement;

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

bool ToolTipClassManager::showToolTip(const QPoint &where, const UserListElement &who)
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

KaduListBoxPixmap::KaduListBoxPixmap(UserListElement user, bool bold_)
	: QListBoxItem(), User(user), pm(pixmapForUser(user)), bold(bold_),
		buf_text(), buf_width(-1), buf_out(), buf_height(-1)
{
	setText(user.altNick());
}

void KaduListBoxPixmap::setMyUIN(UinType u)
{
	myUIN=u;
}

void KaduListBoxPixmap::setShowDesc(bool sd)
{
	ShowDesc=sd;
}

void KaduListBoxPixmap::setAlignTop(bool at)
{
	AlignUserboxIconsTop=at;
}

void KaduListBoxPixmap::setShowMultilineDesc(bool m)
{
	ShowMultilineDesc=m;
}

void KaduListBoxPixmap::setColumnCount(int m)
{
	ColumnCount = m;
}

void KaduListBoxPixmap::setDescriptionColor(const QColor &col)
{
	descColor = col;
}

void KaduListBoxPixmap::paint(QPainter *painter)
{
//	kdebugf();
	QColor origColor = painter->pen().color();
	QString description;
	if (User.usesProtocol("Gadu"))
	{
		UserListElements users(User);
		if (User.protocolData("Gadu", "Blocking").toBool())
			painter->setPen(QColor(255, 0, 0));
		else if (IgnoredManager::isIgnored(users))
			painter->setPen(QColor(192, 192, 0));
		else if (User.protocolData("Gadu", "OfflineTo").toBool())
			painter->setPen(QColor(128, 128, 128));
		if (User.data("HideDescription").toString() != "true")
			description = User.status("Gadu").description();
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

		if (bold)
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

		if (bold)
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
			CONST_FOREACH(text, out)
			{
				painter->drawText(pm.width() + 5, yPos, *text);
				yPos += descriptionFontMetrics->lineSpacing();
			}

			painter->setFont(oldFont);
		}
	}
//	kdebugf2();
}

int KaduListBoxPixmap::height(const QListBox* lb) const
{
//	kdebugf();
	QString description;
	if (User.usesProtocol("Gadu") && User.data("HideDescription").toString() != "true")
		description = User.status("Gadu").description();
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

int KaduListBoxPixmap::lineHeight(const QListBox* lb) const
{
	int height=lb->fontMetrics().lineSpacing()+3;
	return QMAX(pm.height(), height);
}


int KaduListBoxPixmap::width(const QListBox* lb) const
{
// 	kdebugf();

	if (ColumnCount == 0)
		return QMAX(pm.width(), (lb->visibleWidth()));
	else
		return QMAX(pm.width(), (lb->visibleWidth()) / ColumnCount);
/*
   joi:
   we need to use lb->width()-20 here for scrollbar - we cannot get this value
   from Qt style, because eg Keramik returns wrong value (=100) (only one standard
   KDE style has wider scrollbars - SGI, and Kadu will look bad on it :( )

   as well we cannot use lb->visibleWidth(), because of one case:
   	group A has 20 contacts, group B - 25, only 22 can be visible
   	changing group A to B will show up horizontal scrollbar :|
*/
}

//#include <sys/time.h>
void KaduListBoxPixmap::calculateSize(const QString &text, int width, QStringList &out, int &height) const
{
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
//	kdebugm(KDEBUG_DUMP, "h:%d txt:%s\n", height, text.local8Bit().data());
//	for(QStringList::Iterator it = out.begin(); it != out.end(); ++it )
//		kdebugm(KDEBUG_DUMP, ">>%s\n", (*it).local8Bit().data());
}

void KaduListBoxPixmap::changeText(const QString &text)
{
	setText(text);
}

QPixmap KaduListBoxPixmap::pixmapForUser(const UserListElement &user)
{
	bool has_mobile = !user.mobile().isEmpty();
	bool usesGadu = user.usesProtocol("Gadu");
	if (!usesGadu)
	{
		if (has_mobile)
			return icons_manager->loadIcon("Mobile");
		else if (!user.email().isEmpty())
			return icons_manager->loadIcon("WriteEmail");
		else
			return QPixmap();
	}
	else if (pending.pendingMsgs(user))
		return icons_manager->loadIcon("Message");
	else
	{
		const QPixmap &pix = user.status("Gadu").pixmap(has_mobile);
		if (!pix.isNull())
			return pix;
		else
			return icons_manager->loadIcon("Online");
	}
}

void KaduListBoxPixmap::refreshItem()
{
	pm = pixmapForUser(User);
	changeText(User.altNick());
}

class ULEComparer
{
	public:
		inline bool operator()(const UserListElement &e1, const UserListElement &e2) const;
		QValueList<UserBox::CmpFuncDesc> CmpFunctions;
		ULEComparer() : CmpFunctions() {}
};

inline bool ULEComparer::operator()(const UserListElement &e1, const UserListElement &e2) const
{
	int ret = 0;
	CONST_FOREACH(f, CmpFunctions)
	{
		ret = (*f).func(e1, e2);
//		kdebugm(KDEBUG_WARNING, "%s %s %d\n", e1.altNick().local8Bit().data(), e2.altNick().local8Bit().data(), ret);
		if (ret)
			break;
	}
	return ret < 0;
}

UserBoxMenu *UserBox::userboxmenu = NULL;
UserBoxMenu *UserBox::management = NULL;
CreateNotifier UserBox::createNotifier;

UserBox::UserBox(UserGroup *group, QWidget* parent, const char* name, WFlags f)
	: QListBox(parent, name, f), VisibleUsers(new UserGroup(userlist->count() * 2, "visible_users")),
	Filters(), NegativeFilters(), sortHelper(), toRemove(), AppendProxy(), RemoveProxy(), comparer(new ULEComparer()),
	refreshTimer(), lastMouseStopUser(nullElement), lastMouseStop(), tipTimer(),
	verticalPositionTimer(), lastVerticalPosition(0)
{
	kdebugf();
	Filters.append(group);

	desc_action = new Action("ShowDescription", tr("Hide descriptions"), "descriptionsAction", Action::TypeUserList);
	desc_action->setOnShape("HideDescription", tr("Show descriptions"));
	desc_action->setToggleAction(true);
	connect(desc_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(descriptionsActionActivated(const UserGroup*, const QWidget*, bool)));
	connect(desc_action, SIGNAL(iconsRefreshed()), this, SLOT(setDescriptionsActionState()));
	setDescriptionsActionState();

	connect(group, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAddedToGroup(UserListElement, bool, bool)));
	connect(group, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userRemovedFromGroup(UserListElement, bool, bool)));

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

	addCompareFunction("Status", tr("Compares statuses"), compareStatus);
	if (brokenStringCompare)
		addCompareFunction("AltNick", tr("Compares nicks (\"altnicks\") case insensitive"), compareAltNickCaseInsensitive);
	else
		addCompareFunction("AltNick", tr("Compares nicks (\"altnicks\")"), compareAltNick);

	if (!userboxmenu)
		userboxmenu = new UserBoxMenu(this);
	if (!management)
		management = new UserBoxMenu(userboxmenu);
	UserBoxes.append(this);

	setMinimumWidth(20);
	setSelectionMode(QListBox::Extended);

	connect(this, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClickedSlot(QListBoxItem *)));
	connect(this, SIGNAL(returnPressed(QListBoxItem *)), this, SLOT(returnPressedSlot(QListBoxItem *)));
	connect(this, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(currentChangedSlot(QListBoxItem *)));
	connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));

	connect(&pending, SIGNAL(messageFromUserAdded(UserListElement)), this, SLOT(messageFromUserAdded(UserListElement)));
	connect(&pending, SIGNAL(messageFromUserDeleted(UserListElement)), this, SLOT(messageFromUserAdded(UserListElement)));

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
	UserBoxes.remove(this);
	delete VisibleUsers;
	VisibleUsers = 0;
	delete comparer;
	comparer = 0;
	delete desc_action;
	desc_action = 0;
	kdebugf2();
}

#define TIP_TM 1000

void UserBox::tipTimeout()
{
	if (lastMouseStopUser != nullElement)
	{
		tool_tip_class_manager->showToolTip(lastMouseStop, lastMouseStopUser);
		tipTimer.stop();
	}
}

void UserBox::restartTip(const QPoint &p)
{
//	kdebugf();
	KaduListBoxPixmap *item = static_cast<KaduListBoxPixmap *>(itemAt(p));
	if (item)
	{
		if (item->User != lastMouseStopUser)
			hideTip();
		lastMouseStopUser = item->User;
	}
	else
	{
		hideTip();
		lastMouseStopUser = nullElement;
	}
	lastMouseStop = p;
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

void UserBox::descriptionsActionActivated(const UserGroup* /*users*/, const QWidget* /*widget*/, bool toggle)
{
	desc_action->setAllOn(toggle);
	config_file.writeEntry("Look", "ShowDesc", !toggle);
//	dynamic_cast<QCheckBox *>(MainConfigurationWindow::instance()->widgetById("showDescription"))->setChecked(toggle);
	KaduListBoxPixmap::setShowDesc(!toggle);
	UserBox::refreshAllLater();
}

void UserBox::setDescriptionsActionState()
{
	desc_action->setAllOn(!KaduListBoxPixmap::ShowDesc);
}

void UserBox::wheelEvent(QWheelEvent *e)
{
//	kdebugf();
	QListBox::wheelEvent(e);

	// if event source (e->globalPos()) is inside this widget (QRect(...))
	if (QRect(mapToGlobal(QPoint(0,0)), size()).contains(e->globalPos()))
		restartTip(e->pos());
	else
		hideTip(false);
}

void UserBox::enterEvent(QEvent *e)
{
//	kdebugf();
	QListBox::enterEvent(e);
}

void UserBox::leaveEvent(QEvent *e)
{
//	kdebugf();
	hideTip(false);
	QListBox::leaveEvent(e);
}

void UserBox::mousePressEvent(QMouseEvent *e)
{
	kdebugf();
	hideTip(false);
	if (e->button() != RightButton)
		QListBox::mousePressEvent(e);
	else
	{
		QListBoxItem *item = itemAt(e->pos());
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
	QListBox::mouseReleaseEvent(e);
	restartTip(e->pos());
}

void UserBox::mouseMoveEvent(QMouseEvent* e)
{
//	kdebugf();
	if ((e->state() & LeftButton) && itemAt(e->pos()))
	{
		QStringList ules;
		for(unsigned int i = 0, count1 = count(); i < count1; ++i)
			if (isSelected(i))
				ules.append(item(i)->text());

		QDragObject* d = new UlesDrag(ules, this);
		d->dragCopy();
	}
	else
	{
		QListBox::mouseMoveEvent(e);
		restartTip(e->pos());
	}
//	kdebugf2();
}

void UserBox::keyPressEvent(QKeyEvent *e)
{
//	kdebugf();
	hideTip(false);
	QListBox::keyPressEvent(e);
//	QWidget::keyPressEvent(e);
	QListBoxItem *i = item(currentItem());
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

	if (sortHelper.size() == Count)
	{
		bool doRefresh = false;

		kdebugm(KDEBUG_INFO, "checking if order changed\n");
		for (std::vector<UserListElement>::const_iterator user = sortHelper.begin(),
			userEnd = sortHelper.end(); user != userEnd; ++user)
		{
			doRefresh = ((*user) != static_cast<KaduListBoxPixmap *>(item(i++))->User);
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
	QListBox::clear();
	// clear clears columns too...
	setColumnMode(config_file.readNumEntry("Look", "UserBoxColumnCount", 1));

	bool showBold = config_file.readBoolEntry("Look", "ShowBold");

	for (std::vector<UserListElement>::const_iterator user = sortHelper.begin(),
		userEnd = sortHelper.end(); user != userEnd; ++user)
	{
		bool bold = showBold && (*user).usesProtocol("Gadu") &&
				((*user).status("Gadu").isOnline() || (*user).status("Gadu").isBusy());
	//	kdebugm(KDEBUG_INFO, "creating: %s %d\n", (*user).altNick().local8Bit().data(), usesGadu);
		insertItem(new KaduListBoxPixmap(*user, bold));
	}

	// restore selected users
	CONST_FOREACH(username, s_users)
		setSelected(findItem(*username), true);
	setCurrentItem(findItem(s_user));

	// restore vertical scrollbar position
	verticalScrollBar()->setValue(vScrollValue);

	// because settingCurrentItem changes vertical scrollbar position and line
	// above doesn't prevents this, we must set position as soon as possible
	lastVerticalPosition = vScrollValue;
	verticalPositionTimer.start(0, true);

	updateScrollBars();
	refresh();

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
	refresh();
}

void UserBox::rememberVerticalPosition()
{
	lastVerticalPosition = verticalScrollBar()->value();
}

UserListElements UserBox::selectedUsers() const
{
	kdebugf();
	UserListElements users;
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
		if (isSelected(i))
			users.append(static_cast<KaduListBoxPixmap *>(item(i))->User);
	kdebugf2();
	return users;
}

UserBox* UserBox::activeUserBox()
{
	kdebugf();
	FOREACH(box, UserBoxes)
	{
		if ((*box)->isActiveWindow())
		{
			kdebugf2();
			return *box;
		}
	}
	kdebugmf(KDEBUG_WARNING, "return NULL!\n");
//	printBacktrace("activeUserBox NULL");
	return NULL;
}

void UserBox::refreshLater()
{
	refreshTimer.start(0, true);
}

void UserBox::refreshAll()
{
	kdebugf();
	FOREACH(box, UserBoxes)
		(*box)->refresh();
	kdebugf2();
}

void UserBox::refreshAllLater()
{
	kdebugf();
	FOREACH(box, UserBoxes)
		(*box)->refreshLater();
	kdebugf2();
}

void UserBox::messageFromUserAdded(UserListElement elem)
{
	if (visibleUsers()->contains(elem))
		refreshLater();
}

void UserBox::messageFromUserDeleted(UserListElement elem)
{
	if (visibleUsers()->contains(elem))
		refreshLater();
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
	QListBox::resizeEvent(r);
	refreshBackground();
	triggerUpdate(true);
}

void UserBox::refreshBackground()
{
	setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
	setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
	setStaticBackground(backgroundImage);

	if (!backgroundImage)
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

	setPaletteBackgroundPixmap(image);
}

void UserBox::doubleClickedSlot(QListBoxItem *item)
{
	emit doubleClicked(static_cast<KaduListBoxPixmap *>(item)->User);
}

void UserBox::returnPressedSlot(QListBoxItem *item)
{
	emit returnPressed(static_cast<KaduListBoxPixmap *>(item)->User);
}

void UserBox::currentChangedSlot(QListBoxItem *item)
{
	if (item)
		emit currentChanged(static_cast<KaduListBoxPixmap *>(item)->User);
}

UserBoxMenu::UserBoxMenu(QWidget *parent, const char *name) : QPopupMenu(parent, name), iconNames()
{
	connect(this, SIGNAL(aboutToHide()), this, SLOT(restoreLook()));
}

int UserBoxMenu::addItem(const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	return insertItem(text, receiver, member, accel, id);
}

int UserBoxMenu::addItem(const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	iconNames.append(qMakePair(text,iconname));
	return insertItem( QIconSet(icons_manager->loadIcon(iconname)) , text, receiver, member, accel, id);
}

int UserBoxMenu::addItemAtPos(int index, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	return insertItem(text, receiver, member, accel, id, index);
}

int UserBoxMenu::addItemAtPos(int index,const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	iconNames.append(qMakePair(text,iconname));
	return insertItem( QIconSet(icons_manager->loadIcon(iconname)) , text, receiver, member, accel, id, index);
}

int UserBoxMenu::getItem(const QString &caption) const
{
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
		if (!QString::localeAwareCompare(caption, text(idAt(i)).left(caption.length())))
			return idAt(i);
	return -1;
}

void UserBoxMenu::restoreLook()
{
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
	{
		setItemVisible(idAt(i), true);
		setItemChecked(idAt(i), false);
	}
}

void UserBoxMenu::show(QListBoxItem *item)
{
	if (item == NULL)
		return;

	emit popup();
	showPopupMenu(this);
}

void UserBoxMenu::refreshIcons()
{
	kdebugf();
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
	{
		int id = idAt(i);
		QString t = text(id);

		CONST_FOREACH(it, iconNames)
			if (t.startsWith((*it).first))
			{
				bool visible = isItemVisible(id);
				bool checked = isItemChecked(id);
				changeItem(id, icons_manager->loadIcon((*it).second), t);
				setItemVisible(id, visible);
				setItemChecked(id, checked);
			}
	}
	kdebugf2();
}

void UserBox::configurationUpdated()
{
	kdebugf();

	int columnCount = config_file.readNumEntry("Look", "UserBoxColumnCount", 1);
	setColumnMode(columnCount);

	QListBox::setFont(config_file.readFontEntry("Look", "UserboxFont"));

	KaduListBoxPixmap::setFont(config_file.readFontEntry("Look", "UserboxFont"));
	KaduListBoxPixmap::setShowDesc(config_file.readBoolEntry("Look", "ShowDesc"));
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
	QString s = config_file.readEntry("Look", "UserboxBackground");

	if (s.isEmpty() || !QFile::exists(s))
	{
		if (backgroundImage)
		{
			delete backgroundImage;
			backgroundImage = 0;
		}
	}
	else
		backgroundImage = new QImage(s);

	FOREACH(userbox, UserBoxes)
		(*userbox)->refreshBackground();
}

QValueList<UserBox *> UserBox::UserBoxes;

QValueList<UserBox::CmpFuncDesc> UserBox::compareFunctions() const
{
	return comparer->CmpFunctions;
}

void UserBox::applyFilter(UserGroup *g)
{
	kdebugf();
	if (Filters.contains(g))
		return;
	Filters.append(g);
	UserListElements users;
	CONST_FOREACH(user, *VisibleUsers)
		if (!g->contains(*user))
			users.append(*user);
	VisibleUsers->removeUsers(users);
	connect(g, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAddedToGroup(UserListElement, bool, bool)));
	connect(g, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userRemovedFromGroup(UserListElement, bool, bool)));
	kdebugf2();
}

void UserBox::removeFilter(UserGroup *g)
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
	CONST_FOREACH(user, *last)
	{
		if (VisibleUsers->contains(*user)) // we are not looking for contacts which are certain
			continue;
		bool omit = false;

		CONST_FOREACH(ngroup, NegativeFilters)
			if ((*ngroup)->contains(*user))
			{
				omit = true;
				break;
			}
		if (omit)
			continue;

		CONST_FOREACH(group, Filters)
			if (!(*group)->contains(*user))
			{
				omit = true;
				break; // if belongs to any group, there is no point in checking further
			}
		if (omit)
			continue;

		users.append(*user);
	}
	Filters.append(last); // restoring
	VisibleUsers->addUsers(users);
	kdebugf2();
}

void UserBox::applyNegativeFilter(UserGroup *g)
{
	kdebugf();
	if (NegativeFilters.contains(g))
		return;
	NegativeFilters.append(g);
	UserListElements users;
	CONST_FOREACH(user, *VisibleUsers)
		if (g->contains(*user))
			users.append(*user);
	VisibleUsers->removeUsers(users);
	connect(g, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userRemovedFromGroup(UserListElement, bool, bool)));
	connect(g, SIGNAL(userRemoved(UserListElement, bool, bool)),
			this, SLOT(userAddedToGroup(UserListElement, bool, bool)));
	kdebugf2();
}

void UserBox::removeNegativeFilter(UserGroup *g)
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
	CONST_FOREACH(user, *g)
	{
		bool omit = false;
		CONST_FOREACH(ngroup, NegativeFilters)
			if ((*ngroup)->contains(*user))
			{
				omit = true;
				break;
			}
		if (omit)
			continue;

		CONST_FOREACH(group, Filters)
			if (!(*group)->contains(*user))
			{
				omit = true;
				break;
			}
		if (omit)
			continue;

		users.append(*user);
	}
	VisibleUsers->addUsers(users);
	kdebugf2();
}

void UserBox::addCompareFunction(const QString &id, const QString &trDescription,
			int (*cmp)(const UserListElement &, const UserListElement &))
{
	comparer->CmpFunctions.append(CmpFuncDesc(id, trDescription, cmp));
	refreshLater();
}

void UserBox::removeCompareFunction(const QString &id)
{
	FOREACH(c, comparer->CmpFunctions)
		if ((*c).id == id)
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
	FOREACH(c, comparer->CmpFunctions)
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
	FOREACH(c, comparer->CmpFunctions)
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
//		kdebugm(KDEBUG_WARNING, ">>%s\n", (*u).altNick().local8Bit().data());
	std::sort(sortHelper.begin(), sortHelper.end(), *comparer);
//	FOREACH(u, sortHelper)
//		kdebugm(KDEBUG_ERROR, ">>%s\n", (*u).altNick().local8Bit().data());
}

void UserBox::removingProtocol(UserListElement /*elem*/, QString /*protocolName*/, bool /*massively*/, bool /*last*/)
{
	//_removing_ protocol, so it isn't actually removed -> refreshing _Later_ (when protocol will be removed)
	refreshLater();
}

void UserBox::userDataChanged(UserListElement /*elem*/, QString name, QVariant /*oldValue*/,
					QVariant /*currentValue*/, bool /*massively*/, bool /*last*/)
{
	if (name != "AltNick" && name != "Mobile" && name != "HideDescription") // we are not interested in other names
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


void UserBox::userAddedToVisible(UserListElement elem, bool /*massively*/, bool /*last*/)
{
//	kdebugmf(KDEBUG_FUNCTION_START, "start: mass:\n", massively);
	lastMouseStopUser = nullElement;
	sortHelper.push_back(elem);
	refreshLater();
}

class torem
{
	std::vector<UserListElement>::const_iterator begin;
	std::vector<UserListElement>::const_iterator end;
//	std::vector<UserListElement>::const_iterator last;

	public:
	torem(const std::vector<UserListElement> &src) : begin(src.begin()), end(src.end())
	{
//		last = begin;
	}

	inline bool operator()(const UserListElement &u) const
	{
		for(std::vector<UserListElement>::const_iterator it = begin; it != end; ++it)
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

void UserBox::userRemovedFromVisible(UserListElement elem, bool massively, bool last)
{
//	kdebugmf(KDEBUG_FUNCTION_START, "start: mass:%d\n", massively);
	lastMouseStopUser = nullElement;
	if (massively)
		toRemove.push_back(elem);
	else
		sortHelper.erase(std::remove(sortHelper.begin(), sortHelper.end(), elem), sortHelper.end());// the most optimal
	if (massively && last)
	{
		torem pred(toRemove);
		sortHelper.erase(std::remove_if(sortHelper.begin(), sortHelper.end(), pred), sortHelper.end());
		toRemove.clear();
	}
	if (massively)
		refreshLater();
	else
		refresh();
//	kdebugf2();
}

void UserBox::userAddedToGroup(UserListElement elem, bool massively, bool last)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: mass:%d\n", massively);
	const UserGroup *s = static_cast<const UserGroup *>(sender());
//	kdebugm(KDEBUG_INFO, "sender name: '%s'\n", s->name());
	bool append = true;
	CONST_FOREACH(group, NegativeFilters)
		if ((*group)->contains(elem))
		{
			append = false;
			break;
		}
//	kdebugm(KDEBUG_WARNING, "%d %d %d\n", append, massively, last);
	if (append)
		CONST_FOREACH(group, Filters)
			if (!(*group)->contains(elem))
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

const UserGroup *UserBox::visibleUsers() const
{
	return VisibleUsers;
}

QValueList<UserGroup *> UserBox::filters() const
{
	return Filters;
}

QValueList<UserGroup *> UserBox::negativeFilters() const
{
	return NegativeFilters;
}

bool UserBox::currentUserExists() const
{
	return currentItem() != -1;
}

UserListElement UserBox::currentUser() const
{
	QListBoxItem *i = item(currentItem());
	if (i)
		return static_cast<KaduListBoxPixmap *>(i)->User;
	else
	{
		kdebugm(KDEBUG_ERROR, "GO AWAY and check currentUserExists() first!\n");
		printBacktrace("currentUser");
		return UserListElement();
	}
}

UlesDrag::UlesDrag(const QStringList &ules, QWidget* dragSource, const char* name)
	: DragSimple("application/x-kadu-ules", ules.join("\n"), dragSource, name)
{
	kdebugf();
	kdebugf2();
}

bool UlesDrag::decode(const QMimeSource *source, QStringList &ules)
{
	QTextStream stream(source->encodedData("application/x-kadu-ules"), IO_ReadOnly);
	stream.setCodec(QTextCodec::codecForLocale());

	QString allUles = stream.read();
	ules = QStringList::split("\n", allUles);

	return ules.count() > 0;
}

bool UlesDrag::canDecode(const QMimeSource *source)
{
	return source->provides("application/x-kadu-ules");
}

inline int compareAltNick(const UserListElement &u1, const UserListElement &u2)
{
	return u1.altNick().localeAwareCompare(u2.altNick());
}

inline int compareAltNickCaseInsensitive(const UserListElement &u1, const UserListElement &u2)
{
	return u1.altNick().lower().localeAwareCompare(u2.altNick().lower());
}

inline int compareStatus(const UserListElement &u1, const UserListElement &u2)
{
	//WARNING: we are utilizing the fact, that enums in eUserStats are in "correct" order
	// we see Busy and Online as equal here
	int r[] = {Online, Online, Invisible, Offline, Blocking};
	bool u1Gadu = u1.usesProtocol("Gadu");
	bool u2Gadu = u2.usesProtocol("Gadu");
	if (u1Gadu && u2Gadu)
		return r[u1.status("Gadu").status()] - r[u2.status("Gadu").status()];
	else
		return int(u2Gadu) - int(u1Gadu);
}

ToolTipClassManager *tool_tip_class_manager = 0;

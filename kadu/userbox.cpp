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
#include <qcursor.h>
#include <qdragobject.h>
#include <qpainter.h>
#include <qspinbox.h>
#include <qvgroupbox.h>

#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "html_document.h"
#include "icons_manager.h"
#include "ignore.h"
#include "kadu.h"
#include "misc.h"
#include "pending_msgs.h"
#include "userbox.h"

QFontMetrics* KaduListBoxPixmap::descriptionFontMetrics=NULL;
UserBoxSlots* UserBox::userboxslots=NULL;
UinType KaduListBoxPixmap::myUIN;
bool KaduListBoxPixmap::ShowDesc;
bool KaduListBoxPixmap::AlignUserboxIconsTop;
bool KaduListBoxPixmap::ShowMultilineDesc;
bool KaduListBoxPixmap::MultiColumn;
int  KaduListBoxPixmap::MultiColumnWidth;
QColor KaduListBoxPixmap::descColor;

static bool brokenStringCompare;

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

KaduListBoxPixmap::KaduListBoxPixmap(const QPixmap &pix, UserListElement user, bool bold_)
	: QListBoxItem(), User(user), pm(pix), bold(bold_), buf_text(), buf_width(-1), buf_out(), buf_height(-1)
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

void KaduListBoxPixmap::setMultiColumn(bool m)
{
	MultiColumn=m;
}

void KaduListBoxPixmap::setMultiColumnWidth(int w)
{
	MultiColumnWidth=w;
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
		else if (isIgnored(users))
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
//	kdebugf();
	if (MultiColumn)
		return MultiColumnWidth;
	else
		return QMAX(pm.width(), lb->width()-20);
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

UserBox::UserBox(UserGroup *group, QWidget* parent, const char* name, WFlags f)
	: QListBox(parent, name, f), VisibleUsers(new UserGroup(userlist->count() * 2, "visible_users")),
	Filters(), NegativeFilters(), sortHelper(), toRemove(), AppendProxy(), RemoveProxy(), comparer(new ULEComparer()),
	refreshTimer(), lastMouseStopUser(), lastMouseStop(), tipAlive(false), tipTimer(), 
	verticalPositionTimer(), lastVerticalPosition(0)
{
	kdebugf();
	Filters.append(group);

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
		addCompareFunction("AltNick", tr("Compares nicks (\"altnicks\") case insensitive"), compareAltNickCaseInsesitive);
	else
		addCompareFunction("AltNick", tr("Compares nicks (\"altnicks\")"), compareAltNick);

	if (!userboxmenu)
		userboxmenu= new UserBoxMenu(this);
	UserBoxes.append(this);

	if (config_file.readBoolEntry("Look", "MultiColumnUserbox"))
		setColumnMode(QListBox::FitToWidth);
	QListBox::setFont(config_file.readFontEntry("Look", "UserboxFont"));
	setMinimumWidth(20);
	setSelectionMode(QListBox::Extended);
	UserBox::setColorsOrBackgrounds();
	connect(this, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClickedSlot(QListBoxItem *)));
	connect(this, SIGNAL(returnPressed(QListBoxItem *)), this, SLOT(returnPressedSlot(QListBoxItem *)));
	connect(this, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(currentChangedSlot(QListBoxItem *)));
	connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));

	connect(&pending, SIGNAL(messageAdded()), this, SLOT(refreshLater()));
	connect(&pending, SIGNAL(messageDeleted()), this, SLOT(refreshLater()));

	connect(&tipTimer, SIGNAL(timeout()), this, SLOT(tipTimeout()));

	connect(&verticalPositionTimer, SIGNAL(timeout()), this, SLOT(resetVerticalPosition()));
	connect(kadu, SIGNAL(shown()), this, SLOT(resetVerticalPosition()));
	connect(kadu, SIGNAL(hiding()), this, SLOT(rememberVerticalPosition()));
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
	kdebugf2();
}

#define TIP_TM 1000

void UserBox::tipTimeout()
{
//	kdebugf();
	if (!lastMouseStopUser.isEmpty())
	{
//		kdebugm(KDEBUG_INFO, "show hint\n");
		emit changeToolTip(lastMouseStop, VisibleUsers->byAltNick(lastMouseStopUser), true);
		tipAlive = true;
		tipTimer.stop();
	}
}

void UserBox::restartTip(const QPoint &p)
{
//	kdebugf();
	KaduListBoxPixmap *item = static_cast<KaduListBoxPixmap *>(itemAt(p));
	if (item)
	{
		if (item->User.altNick() != lastMouseStopUser)
			hideTip();
		lastMouseStopUser = item->User.altNick();
	}
	else
	{
		hideTip();
		lastMouseStopUser = QString::null;
	}
	lastMouseStop = p;
	tipTimer.start(TIP_TM);
//	kdebugf2();
}

void UserBox::hideTip()
{
//	kdebugf();
	if (tipAlive)
	{
		if (VisibleUsers->containsAltNick(lastMouseStopUser))
			emit changeToolTip(QPoint(), VisibleUsers->byAltNick(lastMouseStopUser), false);
		else
			emit changeToolTip(QPoint(), UserListElement(), false);
		tipAlive = false;
		tipTimer.start(TIP_TM);
	}
// 	kdebugf2();
}

void UserBox::wheelEvent(QWheelEvent *e)
{
//	kdebugf();
	QListBox::wheelEvent(e);
	restartTip(e->pos());
}

void UserBox::enterEvent(QEvent *e)
{
//	kdebugf();
	QListBox::enterEvent(e);
}

void UserBox::leaveEvent(QEvent *e)
{
//	kdebugf();
	hideTip();
	tipTimer.stop();
	QListBox::leaveEvent(e);
}

void UserBox::mousePressEvent(QMouseEvent *e)
{
	kdebugf();
	hideTip();
	tipTimer.start(TIP_TM);
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

void UserBox::mouseMoveEvent(QMouseEvent* e)
{
//	kdebugf();
	if ((e->state() & LeftButton) && itemAt(e->pos()))
	{
		QString drag_text;
		for(unsigned int i = 0, count1 = count(); i < count1; ++i)
			if (isSelected(i))
			{
				if (!drag_text.isEmpty())
					drag_text += "\n";
				drag_text += item(i)->text();
			}
		QDragObject* d = new QTextDrag(drag_text, this);
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
	hideTip();
	tipTimer.stop();
	QListBox::keyPressEvent(e);
//	QWidget::keyPressEvent(e);
	QListBoxItem *i = item(currentItem());
	currentChangedSlot(i);
//	kdebugf2();
}

#include <sys/time.h>
void UserBox::refresh()
{
	kdebugf();
//	printBacktrace("UserBox::refresh()");

/*	struct timeval t1,t2;
	gettimeofday(&t1, NULL);
	for(int j=0; j<1000; ++j)
	{
*/
	sort();

	// remember selected users
	QStringList s_users;
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
		if (isSelected(i))
			s_users.append(item(i)->text());
	QString s_user = currentText();

	// remember vertical scrollbar position
	int vScrollValue = verticalScrollBar()->value();

	// clearing list
	QListBox::clear();

	bool showBold = config_file.readBoolEntry("Look", "ShowBold");

	for (std::vector<UserListElement>::const_iterator user = sortHelper.begin(),
		userEnd = sortHelper.end(); user != userEnd; ++user)
	{
		bool has_mobile = !(*user).mobile().isEmpty();
		bool usesGadu = (*user).usesProtocol("Gadu");
		bool bold = showBold && usesGadu &&
					((*user).status("Gadu").isOnline() || (*user).status("Gadu").isBusy());
//		kdebugm(KDEBUG_INFO, "creating: %s %d\n", (*user).altNick().local8Bit().data(), usesGadu);
		KaduListBoxPixmap *lbp;
		if (!usesGadu)
		{
			if ((*user).mobile().isEmpty())
				lbp = new KaduListBoxPixmap(QPixmap(), *user, false);
			else
				lbp = new KaduListBoxPixmap(icons_manager->loadIcon("Mobile"), *user, false);
		}
		else if (pending.pendingMsgs(*user))
			lbp = new KaduListBoxPixmap(icons_manager->loadIcon("Message"), *user, bold);
		else
		{
			const QPixmap &pix = (*user).status("Gadu").pixmap(has_mobile);
			if (!pix.isNull())
				lbp = new KaduListBoxPixmap(pix, *user, bold);
			else
				lbp = new KaduListBoxPixmap(icons_manager->loadIcon("Online"), *user, bold);
		}
		insertItem(lbp);
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

/*	}
	gettimeofday(&t2, NULL);
	kdebugm(KDEBUG_INFO, "czas: %ld\n", (t2.tv_usec-t1.tv_usec)+(t2.tv_sec*1000000)-(t1.tv_sec*1000000));
*/
	kdebugf2();
}

void UserBox::resetVerticalPosition()
{
	kdebugf();
	verticalScrollBar()->setValue(lastVerticalPosition);
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

void UserBox::closeModule()
{
	kdebugf();
	ConfigDialog::disconnectSlot("Look", "Userbox background color", SIGNAL(changed(const char *, const QColor&)), userboxslots, SLOT(chooseColor(const char *, const QColor&)), "userbox_bg_color");
	ConfigDialog::disconnectSlot("Look", "Userbox font color", SIGNAL(changed(const char *, const QColor&)), userboxslots, SLOT(chooseColor(const char *, const QColor&)), "userbox_font_color");
	ConfigDialog::disconnectSlot("Look", "Font in userbox", SIGNAL(changed(const char *, const QFont&)), userboxslots, SLOT(chooseFont(const char *, const QFont&)), "userbox_font_box");
	ConfigDialog::disconnectSlot("Look", "Multicolumn userbox", SIGNAL(toggled(bool)), userboxslots, SLOT(onMultiColumnUserbox(bool)));
	ConfigDialog::disconnectSlot("Look", 0, SIGNAL(clicked()), userboxslots, SLOT(chooseBackgroundFile()), "userbox_background_fileopen");
	ConfigDialog::disconnectSlot("Look", "Change image size", SIGNAL(toggled(bool)), userboxslots, SLOT(userboxBackgroundMove(bool)));
	ConfigDialog::disconnectSlot("Look", "Background", SIGNAL(textChanged(const QString &)), userboxslots, SLOT(backgroundFileChanged(const QString &)));

	ConfigDialog::unregisterSlotOnCreateTab("Look", userboxslots, SLOT(onCreateTabLook()));
	ConfigDialog::unregisterSlotOnApplyTab("Look", userboxslots, SLOT(onApplyTabLook()));
	delete KaduListBoxPixmap::descriptionFontMetrics;
	KaduListBoxPixmap::descriptionFontMetrics = 0;
	delete userboxslots;
	userboxslots = 0;

		ConfigDialog::removeControl("Look", "Image height");
		ConfigDialog::removeControl("Look", "Image width");
		ConfigDialog::removeControl("Look", "Start at [Y]");
		ConfigDialog::removeControl("Look", "Start at [X]");
		ConfigDialog::removeControl("Look", "Change image size");
	ConfigDialog::removeControl("Look", "Background image options");

	ConfigDialog::removeControl("Look", 0, "userbox_background_fileopen");
	ConfigDialog::removeControl("Look", "Background");
	ConfigDialog::removeControl("Look", "userbox_background");
	ConfigDialog::removeControl("Look", "Userbox background");

				ConfigDialog::removeControl("Look", "<b>Text</b> preview", "preview_panel");
			ConfigDialog::removeControl("Look", "Preview panel");
				ConfigDialog::removeControl("Look", "<b>Text</b> preview", "preview_userbox");
			ConfigDialog::removeControl("Look", "Preview userbox");
		ConfigDialog::removeControl("Look", "othr_prvws");
//	ConfigDialog::removeControl("Look", "Previews");

		ConfigDialog::removeControl("Look", "Font in userbox", "userbox_font_box");
//	ConfigDialog::removeControl("Look", "Fonts");

			ConfigDialog::removeControl("Look", "Description font color", "userbox_desc_color");
			ConfigDialog::removeControl("Look", "Userbox font color", "userbox_font_color");
			ConfigDialog::removeControl("Look", "Userbox background color", "userbox_bg_color");
//		ConfigDialog::removeControl("Look", "Main window");
//	ConfigDialog::removeControl("Look", "Colors");

	ConfigDialog::removeControl("Look", "Userbox width when multi column");
	ConfigDialog::removeControl("Look", "Multicolumn userbox");
	ConfigDialog::removeControl("Look", "Columns");

	ConfigDialog::removeControl("Look", "Show available users in bold");
	ConfigDialog::removeControl("Look", "Align icon next to contact name");
	ConfigDialog::removeControl("Look", "Multiline description in userbox");
	ConfigDialog::removeControl("Look", "Show description in userbox");
	kdebugf2();
}

void UserBox::initModule()
{
	kdebugf();
	// add some values at first run
	QWidget w;
	config_file.addVariable("Look", "InfoPanelBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "InfoPanelFgColor", w.paletteForegroundColor());
	config_file.addVariable("Look", "UserboxBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "UserboxFgColor", w.paletteForegroundColor());
	config_file.addVariable("Look", "AlignUserboxIconsTop", false);
	config_file.addVariable("Look", "DescriptionColor", w.paletteForegroundColor());

	// font operations are really slow, so we are doing them only when it's necessary (at first run)
	if (config_file.readEntry("Look", "MultiColumnUserboxWidth").isEmpty())
		config_file.addVariable("Look", "MultiColumnUserboxWidth", int(QFontMetrics(*defaultFont).width("Imie i Nazwisko")*1.5));

	ConfigDialog::addCheckBox("Look", "varOpts-beginner", QT_TRANSLATE_NOOP("@default", "Show description in userbox"), "ShowDesc", true);
	ConfigDialog::addCheckBox("Look", "varOpts-beginner", QT_TRANSLATE_NOOP("@default", "Multiline description in userbox"), "ShowMultilineDesc", true, 0, 0, Advanced);
	ConfigDialog::addCheckBox("Look", "varOpts-advanced", QT_TRANSLATE_NOOP("@default", "Align icon next to contact name"), "AlignUserboxIconsTop", config_file.readBoolEntry("Look", "AlignUserboxIconsTop"), 0, 0, Advanced);
	ConfigDialog::addCheckBox("Look", "varOpts-expert", QT_TRANSLATE_NOOP("@default", "Show available users in bold"), "ShowBold", true, QT_TRANSLATE_NOOP("@default","Displays users that are not offline using a bold font"), 0, Expert);

	ConfigDialog::addVGroupBox("Look", "varOpts2", QT_TRANSLATE_NOOP("@default", "Columns"), 0, Advanced);
	ConfigDialog::addCheckBox("Look", "Columns", QT_TRANSLATE_NOOP("@default", "Multicolumn userbox"), "MultiColumnUserbox", false);
	ConfigDialog::addSpinBox("Look", "Columns", QT_TRANSLATE_NOOP("@default", "Userbox width when multi column"), "MultiColumnUserboxWidth", 1, 1000, 1);

//	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Colors"), 0, Advanced);
//		ConfigDialog::addVGroupBox("Look", "Colors", QT_TRANSLATE_NOOP("@default", "Main window"));
			ConfigDialog::addColorButton("Look", "Main window", QT_TRANSLATE_NOOP("@default", "Userbox background color"), "UserboxBgColor", QColor(), 0, "userbox_bg_color");
			ConfigDialog::addColorButton("Look", "Main window", QT_TRANSLATE_NOOP("@default", "Userbox font color"), "UserboxFgColor", QColor(), 0, "userbox_font_color");
			ConfigDialog::addColorButton("Look", "Main window", QT_TRANSLATE_NOOP("@default", "Description font color"), "DescriptionColor", QColor(), 0, "userbox_desc_color");

//	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Fonts"), 0, Advanced);
		ConfigDialog::addSelectFont("Look", "Fonts", QT_TRANSLATE_NOOP("@default", "Font in userbox"), "UserboxFont", defaultFont->toString(), 0, "userbox_font_box");

//	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Previews"), 0, Advanced);
		ConfigDialog::addHBox("Look", "Previews", "othr_prvws");
			ConfigDialog::addVGroupBox("Look", "othr_prvws", QT_TRANSLATE_NOOP("@default", "Preview userbox"));
				ConfigDialog::addLabel("Look", "Preview userbox", "<b>Text</b> preview", "preview_userbox");
			ConfigDialog::addVGroupBox("Look", "othr_prvws", QT_TRANSLATE_NOOP("@default", "Preview panel"));
				ConfigDialog::addLabel("Look", "Preview panel", "<b>Text</b> preview", "preview_panel");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Userbox background"), 0, Advanced);
	ConfigDialog::addHBox("Look", "Userbox background", "userbox_background");
	ConfigDialog::addLineEdit("Look", "userbox_background", QT_TRANSLATE_NOOP("@default", "Background"), "UserboxBackground", 0, "Background Path");
	ConfigDialog::addPushButton("Look", "userbox_background", 0, "OpenFile", 0, "userbox_background_fileopen");

	ConfigDialog::addVGroupBox("Look", "Userbox background", QT_TRANSLATE_NOOP("@default", "Background image options"), 0, Expert);
		ConfigDialog::addCheckBox("Look", "Background image options", QT_TRANSLATE_NOOP("@default", "Change image size"), "UserboxBackgroundMove", false);
		ConfigDialog::addSpinBox("Look", "Background image options", QT_TRANSLATE_NOOP("@default", "Start at [X]"), "UserboxBackgroundSX", 0, 1499, 1);
		ConfigDialog::addSpinBox("Look", "Background image options", QT_TRANSLATE_NOOP("@default", "Start at [Y]"), "UserboxBackgroundSY", 0, 1099, 1);
		ConfigDialog::addSpinBox("Look", "Background image options", QT_TRANSLATE_NOOP("@default", "Image width"), "UserboxBackgroundSW", 100, 1600, 1);
		ConfigDialog::addSpinBox("Look", "Background image options", QT_TRANSLATE_NOOP("@default", "Image height"), "UserboxBackgroundSH", 100, 1200, 1);

	KaduListBoxPixmap::setFont(config_file.readFontEntry("Look","UserboxFont"));
	KaduListBoxPixmap::setShowDesc(config_file.readBoolEntry("Look", "ShowDesc"));
	KaduListBoxPixmap::setAlignTop(config_file.readBoolEntry("Look", "AlignUserboxIconsTop"));
	KaduListBoxPixmap::setShowMultilineDesc(config_file.readBoolEntry("Look", "ShowMultilineDesc"));
	KaduListBoxPixmap::setMultiColumn(config_file.readBoolEntry("Look", "MultiColumnUserbox"));
	KaduListBoxPixmap::setMultiColumnWidth(config_file.readNumEntry("Look", "MultiColumnUserboxWidth", 230));
	KaduListBoxPixmap::setMyUIN(config_file.readNumEntry("General", "UIN"));
	KaduListBoxPixmap::setDescriptionColor(config_file.readColorEntry("Look", "DescriptionColor"));

	userboxslots= new UserBoxSlots();
	ConfigDialog::registerSlotOnCreateTab("Look", userboxslots, SLOT(onCreateTabLook()));
	ConfigDialog::registerSlotOnApplyTab("Look", userboxslots, SLOT(onApplyTabLook()));

	ConfigDialog::connectSlot("Look", "Userbox background color", SIGNAL(changed(const char *, const QColor&)), userboxslots, SLOT(chooseColor(const char *, const QColor&)), "userbox_bg_color");
	ConfigDialog::connectSlot("Look", "Userbox font color", SIGNAL(changed(const char *, const QColor&)), userboxslots, SLOT(chooseColor(const char *, const QColor&)), "userbox_font_color");

	ConfigDialog::connectSlot("Look", "Font in userbox", SIGNAL(changed(const char *, const QFont&)), userboxslots, SLOT(chooseFont(const char *, const QFont&)), "userbox_font_box");

	ConfigDialog::connectSlot("Look", "Multicolumn userbox", SIGNAL(toggled(bool)), userboxslots, SLOT(onMultiColumnUserbox(bool)));

	ConfigDialog::connectSlot("Look", 0, SIGNAL(clicked()), userboxslots, SLOT(chooseBackgroundFile()), "userbox_background_fileopen");

	ConfigDialog::connectSlot("Look", "Change image size", SIGNAL(toggled(bool)), userboxslots, SLOT(userboxBackgroundMove(bool)));

	ConfigDialog::connectSlot("Look", "Background", SIGNAL(textChanged(const QString &)), userboxslots, SLOT(backgroundFileChanged(const QString &)));

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
	refreshLater();
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
		setItemEnabled(idAt(i), true);
		setItemChecked(idAt(i), false);
	}
}

void UserBoxMenu::show(QListBoxItem *item)
{
	if (item == NULL)
		return;

	emit popup();
	exec(QCursor::pos());
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
				bool enabled=isItemEnabled(id);
				bool checked=isItemChecked(id);
				changeItem(id, icons_manager->loadIcon((*it).second), t);
				setItemEnabled(id, enabled);
				setItemChecked(id, checked);
			}
	}
	kdebugf2();
}

void UserBoxSlots::onCreateTabLook()
{
	kdebugf();

	QSpinBox *multi = ConfigDialog::getSpinBox("Look", "Userbox width when multi column");
	multi->setSuffix(" px");
	multi->setEnabled(config_file.readBoolEntry("Look", "MultiColumnUserbox"));

	backgroundFileChanged(config_file.readEntry("Look", "UserboxBackground"));

	multi = ConfigDialog::getSpinBox("Look", "Start at [X]");
	multi->setSuffix(" px");
	multi->setEnabled(config_file.readBoolEntry("Look", "UserboxBackgroundMove"));
	multi=ConfigDialog::getSpinBox("Look", "Start at [Y]");
	multi->setSuffix(" px");
	multi->setEnabled(config_file.readBoolEntry("Look", "UserboxBackgroundMove"));
	multi=ConfigDialog::getSpinBox("Look", "Image width");
	multi->setSuffix(" px");
	multi->setEnabled(config_file.readBoolEntry("Look", "UserboxBackgroundMove"));
	multi=ConfigDialog::getSpinBox("Look", "Image height");
	multi->setSuffix(" px");
	multi->setEnabled(config_file.readBoolEntry("Look", "UserboxBackgroundMove"));

	updatePreview();
	kdebugf2();
}

void UserBoxSlots::onApplyTabLook()
{
	kdebugf();
	UserBox *userbox = kadu->userbox();

	if (config_file.readBoolEntry("Look", "MultiColumnUserbox"))
		userbox->setColumnMode(QListBox::FitToWidth);
	else
		userbox->setColumnMode(1);

	userbox->QListBox::setFont(config_file.readFontEntry("Look", "UserboxFont"));

	KaduListBoxPixmap::setFont(config_file.readFontEntry("Look","UserboxFont"));
	KaduListBoxPixmap::setShowDesc(config_file.readBoolEntry("Look", "ShowDesc"));
	KaduListBoxPixmap::setAlignTop(config_file.readBoolEntry("Look", "AlignUserboxIconsTop"));
	KaduListBoxPixmap::setShowMultilineDesc(config_file.readBoolEntry("Look", "ShowMultilineDesc"));
	KaduListBoxPixmap::setMultiColumn(config_file.readBoolEntry("Look", "MultiColumnUserbox"));
	KaduListBoxPixmap::setMultiColumnWidth(config_file.readNumEntry("Look", "MultiColumnUserboxWidth", 230));
	KaduListBoxPixmap::setMyUIN(config_file.readNumEntry("General", "UIN"));
	KaduListBoxPixmap::setDescriptionColor(config_file.readColorEntry("Look", "DescriptionColor"));

	UserBox::setColorsOrBackgrounds();

	UserBox::refreshAllLater();
	kdebugf2();
}

void UserBox::setColorsOrBackgrounds()
{
	QString s = config_file.readEntry("Look", "UserboxBackground");
	if (s.isEmpty())
	{
		for(QValueList<UserBox*>::iterator i = UserBoxes.begin(); i != UserBoxes.end(); ++i)
		{
			(*i)->setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
			(*i)->setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
		}
	}
	else
	{
		QPixmap pix(s);
		if (config_file.readBoolEntry("Look", "UserboxBackgroundMove"))
		{
			int sw = config_file.readNumEntry("Look", "UserboxBackgroundSW");
			int sh = config_file.readNumEntry("Look", "UserboxBackgroundSH");
			int sx = config_file.readNumEntry("Look", "UserboxBackgroundSX");
			int sy = config_file.readNumEntry("Look", "UserboxBackgroundSY");
			if (sx >= pix.width())
				sx = pix.width() - 1;
			if (sy >= pix.height())
				sy = pix.height() - 1;
			if (sw > pix.width() - sx)
				sw = pix.width() - sx;
			if (sh > pix.height() - sy)
				sh = pix.height() - sy;
			copyBlt(&pix, 0, 0, &pix, sx, sy, sw, sh);
			pix.resize(sw, sh);
		}
		for(QValueList<UserBox*>::iterator i = UserBoxes.begin(); i != UserBoxes.end(); ++i)
		{
			(*i)->setPaletteBackgroundPixmap(pix);
			(*i)->setStaticBackground(true);
			(*i)->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxFgColor"));
		}
	}
}

void UserBoxSlots::backgroundFileChanged(const QString &text)
{
	QVGroupBox *opts = ConfigDialog::getVGroupBox("Look", "Background image options");
	opts->setEnabled(!text.stripWhiteSpace().isEmpty());
}

void UserBoxSlots::chooseBackgroundFile()
{
	QLineEdit *bg_path = ConfigDialog::getLineEdit("Look", "Background");
	QWidget w;
	ImageDialog *bg_dlg = new ImageDialog(&w);
	bg_dlg->setDir(bg_path->text());
	bg_dlg->setCaption(tr("Insert image"));
	if (bg_dlg->exec() == QDialog::Accepted)
		bg_path->setText(bg_dlg->selectedFile());
	delete bg_dlg;
}

void UserBoxSlots::userboxBackgroundMove(bool toggled)
{
	ConfigDialog::getSpinBox("Look", "Start at [X]")->setEnabled(toggled);
	ConfigDialog::getSpinBox("Look", "Start at [Y]")->setEnabled(toggled);
	ConfigDialog::getSpinBox("Look", "Image width")->setEnabled(toggled);
	ConfigDialog::getSpinBox("Look", "Image height")->setEnabled(toggled);
}

void UserBoxSlots::chooseColor(const char *name, const QColor &color)
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_userbox");
	if (QString(name)=="userbox_bg_color")
		preview->setPaletteBackgroundColor(color);
	else if (QString(name)=="userbox_font_color")
		preview->setPaletteBackgroundColor(color);
	else
		kdebugmf(KDEBUG_ERROR, "label '%s' not known\n", name);
	kdebugf2();
}

void UserBoxSlots::chooseFont(const char *name, const QFont &font)
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_userbox");
	if (QString(name)=="userbox_font_box")
		preview->setFont(font);
	kdebugf2();
}

void UserBoxSlots::updatePreview()
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_userbox");
	preview->setFont(config_file.readFontEntry("Look", "UserboxFont"));
	preview->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxFgColor"));
	preview->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxBgColor"));
	preview->setAlignment(Qt::AlignLeft);
	kdebugf2();
}

void UserBoxSlots::onMultiColumnUserbox(bool toggled)
{
	ConfigDialog::getSpinBox("Look", "Userbox width when multi column")->setEnabled(toggled);
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
	Filters.pop_back(); //temorarily removing

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

void UserBox::moveUpCompareFunction(const QString &id)
{
	kdebugf();
	CmpFuncDesc d;
	int pos = 0;
	FOREACH(c, comparer->CmpFunctions)
	{
		if ((*c).id == id)
		{
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
}

void UserBox::moveDownCompareFunction(const QString &id)
{
	kdebugf();
	CmpFuncDesc d;
	int pos = 0;
	int cnt = comparer->CmpFunctions.count();
	FOREACH(c, comparer->CmpFunctions)
	{
		if ((*c).id == id)
		{
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
	lastMouseStopUser = QString::null;
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
	lastMouseStopUser = QString::null;
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

inline int compareAltNick(const UserListElement &u1, const UserListElement &u2)
{
	return u1.altNick().localeAwareCompare(u2.altNick());
}

inline int compareAltNickCaseInsesitive(const UserListElement &u1, const UserListElement &u2)
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

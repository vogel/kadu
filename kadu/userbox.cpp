/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qmap.h>
#include <qdragobject.h>
#include <qpen.h>
#include <qregexp.h>
#include <qfontdatabase.h>
#include <qpopupmenu.h>
#include <qcursor.h>

#include "status.h"
#include "userbox.h"
#include "ignore.h"
#include "debug.h"
#include "pending_msgs.h"
#include "config_dialog.h"
#include "config_file.h"
#include "misc.h"

QFontMetrics* KaduListBoxPixmap::descriptionFontMetrics=NULL;
int KaduListBoxPixmap::scrollWidth;

void KaduListBoxPixmap::setFont(const QFont &f)
{
	QFont newFont = QFont(f);
	newFont.setPointSize(f.pointSize() - 2);
	if (descriptionFontMetrics)
		delete descriptionFontMetrics;
	descriptionFontMetrics= new QFontMetrics(newFont);
}

void KaduListBoxPixmap::setVerticalScrollWidth(int w)
{
	scrollWidth=w;
}

KaduListBoxPixmap::KaduListBoxPixmap(const QPixmap &pix, const QString &text)
	: QListBoxItem()
{
	buf_width=-1;
	pm = pix;
	setText(text);
}

KaduListBoxPixmap::KaduListBoxPixmap(const QPixmap &pix, const QString &text, const QString &descr, bool bold)
	: QListBoxItem()
{
	buf_width=-1;
	pm = pix;
	setText(text);
	setDescription(descr);
	setBold(bold);
}

void KaduListBoxPixmap::paint(QPainter *painter) {
	UserListElement &user = userlist.byAltNick(text());
	bool isOurUin=((UinType)config_file.readNumEntry("General", "UIN") == user.uin);
	if (user.uin)
	{
		UinsList uins;
		uins.append(user.uin);
		if (user.blocking)
		{
			QPen &pen = (QPen &)painter->pen();
			pen.setColor(QColor(255, 0, 0));
			painter->setPen(pen);
		}
		else if (isIgnored(uins))
		{
			QPen &pen = (QPen &)painter->pen();
			pen.setColor(QColor(192, 192, 0));
			painter->setPen(pen);
		}
		else if (user.offline_to_user)
		{
			QPen &pen = (QPen &)painter->pen();
			pen.setColor(QColor(128, 128, 128));
			painter->setPen(pen);
		}
	}

	int itemHeight = height(listBox());
	int yPos;
	QString descr=isOurUin ? own_description : description();
	bool hasDescription=isOurUin ? ifStatusWithDescription(getCurrentStatus()) : !descr.isEmpty();

	if (!pm.isNull()) {
		yPos = (itemHeight - pm.height()) / 2;
		painter->drawPixmap(3, yPos, pm);
	}

	if (!text().isEmpty()) {
		QFont oldFont = painter->font();

		if (bold) {
			QFont newFont = QFont(oldFont);
			newFont.setWeight(QFont::Bold);
			painter->setFont(newFont);
		}

		QFontMetrics fm = painter->fontMetrics();

		bool showDesc=config_file.readBoolEntry("Look", "ShowDesc");
		if (showDesc && hasDescription)
			yPos = fm.ascent() + 1;
		else
			yPos = ((itemHeight - fm.height()) / 2) + fm.ascent();

		painter->drawText(pm.width() + 5, yPos, text());

		if (bold)
			painter->setFont(oldFont);

//		kdebug("KaduListBoxPixmap::paint(): isOurUin = %d, own_description = %s\n",
//			isOurUin, (const char *)unicode2latin(own_description));
		if (showDesc && hasDescription) {
			yPos += fm.height() - fm.descent();

			QFont newFont = QFont(oldFont);
			newFont.setPointSize(oldFont.pointSize() - 2);
			painter->setFont(newFont);

			if (!config_file.readBoolEntry("Look", "ShowMultilineDesc"))
				descr.replace(QRegExp("\n"), " ");

			int h;
			QStringList out;
			calculateSize(descr, width(listBox())-5-pm.width(), out, h);
			for(QStringList::Iterator it = out.begin(); it != out.end(); ++it )
			{
				painter->drawText(pm.width() + 5, yPos, *it);
				yPos += descriptionFontMetrics->lineSpacing();
			}

			painter->setFont(oldFont);
		}
	}
}

int KaduListBoxPixmap::height(const QListBox* lb) const
{
	UserListElement &user = userlist.byAltNick(text());
	bool isOurUin=((UinType)config_file.readNumEntry("General", "UIN") == user.uin);
	QString descr=isOurUin ? own_description : description();
	bool hasDescription=isOurUin ? ifStatusWithDescription(getCurrentStatus()) : !descr.isEmpty();

	int height=lb->fontMetrics().lineSpacing()+2;
	if (hasDescription && config_file.readBoolEntry("Look", "ShowDesc"))
	{
		if (!config_file.readBoolEntry("Look", "ShowMultilineDesc"))
			descr.replace(QRegExp("\n"), " ");
		QStringList out;
		int h;
		calculateSize(descr, width(lb)-5-pm.width(), out, h);
		height+=h;
	}
	return height;
}

int KaduListBoxPixmap::width(const QListBox* lb) const
{
	if (config_file.readBoolEntry("Look", "MultiColumnUserbox"))
		return config_file.readNumEntry("Look", "MultiColumnUserboxWidth", 230);
	else
		return lb->width()-5-scrollWidth;
}

void KaduListBoxPixmap::calculateSize(const QString &text, int width, QStringList &out, int &height) const
{
//	kdebugf();
	if (text==buf_text && width==buf_width)	
	{
		out=buf_out;
		height=buf_height;
		return;
	}

	int textlen=text.length();
	int start=0, len, tmplen;
	
	out.clear();
	height=0;
	
	while (start<textlen)
	{
		len=0;
		bool tooWide=false;
		while (1)
		{
			tooWide=(descriptionFontMetrics->width(text.mid(start, len))>=width);
			if (!tooWide && text[start+len]!='\n' && start+len<textlen)
				len++;
			else
				break;
		}
		
		if (tooWide)
		{
			tmplen=len;
			while (len>0 && !text[start+len-1].isSpace())
				len--;
			if (len==0)
				len=tmplen-1;
		}

		out.push_back(text.mid(start, len));
		height++;
		start+=len;
		if (text[start].isSpace())
			start++;
	}
	height*=descriptionFontMetrics->lineSpacing();
	
	buf_text=text;
	buf_width=width;
	buf_out=out;
	buf_height=height;
//	kdebug("h:%d txt:%s\n", height, text.local8Bit().data());
//	for(QStringList::Iterator it = out.begin(); it != out.end(); ++it )
//		kdebug(">>%s\n", (*it).local8Bit().data());
}

UserBoxMenu *UserBox::userboxmenu = NULL;

UserBox::UserBox(QWidget* parent,const char* name,WFlags f)
	: QListBox(parent,name),QToolTip(viewport())

{
	if (!userboxmenu)
		userboxmenu= new UserBoxMenu(this);

	UserBoxes.append(this);
	setSelectionMode(QListBox::Extended);
	KaduListBoxPixmap::setVerticalScrollWidth(verticalScrollBar()->sliderRect().width());
}

UserBox::~UserBox()
{
	UserBoxes.remove(this);
}

void UserBox::maybeTip(const QPoint &c)
{
	QListBoxItem* item = static_cast<QListBoxItem*>(itemAt(c));

	if(item)
	{
		QRect r(itemRect(item));
		QString s;

		switch (userlist.byAltNick(item->text()).status)
		{
			case GG_STATUS_AVAIL:
				s = tr("<i>Available</i>");
				break;
			case GG_STATUS_BUSY:
				s = tr("<i>Busy</i>");
				break;
			case GG_STATUS_NOT_AVAIL:
				if (!userlist.byAltNick(item->text()).uin)
					s = tr("<i>Mobile:</i><b> ")+userlist.byAltNick(item->text()).mobile+"</b>";
				else
					s = tr("<nobr><i>Not available</i></nobr>");
				break;
			case GG_STATUS_INVISIBLE:
			case GG_STATUS_INVISIBLE2:
				s = tr("<i>Invisible</i>");
				break;
			case GG_STATUS_INVISIBLE_DESCR:
				s = tr("<i>Invisible <b>(d.)</b></i>");
				break;
			case GG_STATUS_BUSY_DESCR:
				s = tr("<nobr><i>Busy <b>(d.)</b></i></nobr>");
				break;
			case GG_STATUS_NOT_AVAIL_DESCR:
				s = tr("<nobr><i>Not available <b>(d.)</b></i></nobr>");
				break;
			case GG_STATUS_AVAIL_DESCR:
				s = tr("<nobr><i>Available <b>(d.)</b></i></nobr>");
				break;
			case GG_STATUS_BLOCKED:
				s = tr("<nobr><i>Blocking</i></nobr>");
				break;
			default:
				s = tr("<nobr><i>Unknown status</i></nobr>");
				break;
		}
		QString desc = userlist.byAltNick(item->text()).description;
		if (desc != "")
		{
			s += "<br/><br/>";
			s += tr("<b>Description:</b><br/>");
			escapeSpecialCharacters(desc);
			desc.replace(QRegExp(" "), "&nbsp;");
			desc.replace(QRegExp("\n"), "<br/>");
			s += desc;
		}
		tip(r, s);
	}
}

void UserBox::mousePressEvent(QMouseEvent *e) {
	if (e->button() != RightButton)
		QListBox::mousePressEvent(e);
	else {
		QListBoxItem *item;
		item = itemAt(e->pos());
		if (item) {
			if (!item->isSelected())
				if (!(e->state() & Qt::ControlButton))
					for (unsigned int i = 0; i < count(); i++)
						setSelected(i, FALSE);
			setSelected(item, TRUE);
			setCurrentItem(item);
			emit rightButtonClicked(item, e->globalPos());
			}
		}
}

void UserBox::mouseMoveEvent(QMouseEvent* e)
{
	if ((e->state() & LeftButton)&&itemAt(e->pos()))
	{
		QString drag_text;
		for(unsigned int i=0; i<count(); i++)
			if(isSelected(i))
			{
				if(drag_text!="")
					drag_text+="\n";
				drag_text+=item(i)->text();
			}
		QDragObject* d = new QTextDrag(drag_text,this);
		d->dragCopy();
	}
	else
		QListBox::mouseMoveEvent(e);
}

void UserBox::keyPressEvent(QKeyEvent *e)
{
	QListBox::keyPressEvent(e);
	QWidget::keyPressEvent(e);
	emit currentChanged(item(currentItem()));
}

void UserBox::sortUsersByAltNick(QStringList &users) {

	stringHeapSort(users);
}

void UserBox::refresh()
{
	unsigned int i;
	KaduListBoxPixmap *lbp;

	kdebugf();
	this->setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
	this->setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
	this->QListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));
	KaduListBoxPixmap::setFont(config_file.readFontEntry("Look","UserboxFont"));

	// Zapamietujemy zaznaczonych uzytkownikow
	QStringList s_users;
	for (i = 0; i < count(); i++)
		if (isSelected(i))
			s_users.append(item(i)->text());
	QString s_user = currentText();
	// Najpierw dzielimy uzytkownikow na cztery grupy
	QStringList a_users;
	QStringList i_users;
	QStringList n_users;
	QStringList b_users;
	
	UinType myUin=config_file.readNumEntry("General", "UIN");
	for (i = 0; i < Users.count(); i++) {
		UserListElement &user = userlist.byAltNick(Users[i]);
		if (user.uin) {
			if (user.uin == myUin) {
				user.status = getCurrentStatus() & (~GG_STATUS_FRIENDS_MASK);
				user.description = own_description;
				}
			switch (user.status) {
				case GG_STATUS_AVAIL:
				case GG_STATUS_AVAIL_DESCR:
				case GG_STATUS_BUSY:
				case GG_STATUS_BUSY_DESCR:
				case GG_STATUS_BLOCKED:
					a_users.append(user.altnick);
					break;
				case GG_STATUS_INVISIBLE_DESCR:
				case GG_STATUS_INVISIBLE:
				case GG_STATUS_INVISIBLE2:
					i_users.append(user.altnick);
					break;
				default:
					n_users.append(user.altnick);
				}
			}
		else
			b_users.append(user.altnick);
		}
	sortUsersByAltNick(a_users);
	sortUsersByAltNick(i_users);
	sortUsersByAltNick(n_users);
	sortUsersByAltNick(b_users);
	// Czyscimy liste
	clear();
	// Dodajemy aktywnych
	bool showBold=config_file.readBoolEntry("Look", "ShowBold");
	for (i = 0; i < a_users.count(); i++)
	{
		UserListElement &user = userlist.byAltNick(a_users[i]);
		bool has_mobile = user.mobile.length();
		bool bold = showBold ? (user.status == GG_STATUS_AVAIL || user.status == GG_STATUS_AVAIL_DESCR || user.status == GG_STATUS_BUSY || user.status == GG_STATUS_BUSY_DESCR) : 0;
		if (pending.pendingMsgs(user.uin)) {
			lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Message"), user.altnick, user.description, bold);
			insertItem(lbp);
//			insertItem(*icons->loadIcon("message"), user.altnick);
			}
		else
		{
			QPixmap pix;
			switch (user.status) {
				case GG_STATUS_AVAIL:
					if (has_mobile)
						pix = icons_manager.loadIcon("OnlineWithMobile");
					else
						pix = icons_manager.loadIcon("Online");
					break;
				case GG_STATUS_AVAIL_DESCR:
					if (has_mobile)
						pix = icons_manager.loadIcon("OnlineWithDescriptionMobile");
					else
						pix = icons_manager.loadIcon("OnlineWithDescription");
					break;
				case GG_STATUS_BUSY:
					if (has_mobile)
						pix = icons_manager.loadIcon("BusyWithMobile");
					else
						pix = icons_manager.loadIcon("Busy");
					break;
				case GG_STATUS_BUSY_DESCR:
					if (has_mobile)
						pix = icons_manager.loadIcon("BusyWithDescriptionMobile");
					else
						pix = icons_manager.loadIcon("BusyWithDescription");
					break;
				case GG_STATUS_BLOCKED:
					pix = icons_manager.loadIcon("Blocking");
					break;
				}
			if (!pix.isNull())
				lbp = new KaduListBoxPixmap(pix, user.altnick, user.description, bold);
//				insertItem(*pix, user.altnick);
			else
				lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Online"), user.altnick, user.description, bold);
//				insertItem(*icons->loadIcon("online"), user.altnick);
			insertItem(lbp);
		}
	}
	// Dodajemy niewidocznych
	for (i = 0; i < i_users.count(); i++) {
		UserListElement &user = userlist.byAltNick(i_users[i]);
		bool has_mobile = user.mobile.length();
		if (pending.pendingMsgs(user.uin)) {
			lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Message"), user.altnick, user.description, 0);
			insertItem(lbp);
//			insertItem(*icons->loadIcon("message"), user.altnick);
			}
		else {
			QPixmap pix;
			switch (user.status) {
				case GG_STATUS_INVISIBLE_DESCR:
					if (has_mobile)
						pix = icons_manager.loadIcon("InvisibleWithDescriptionMobile");
					else
						pix = icons_manager.loadIcon("OnlineWithDescription");
					break;
				case GG_STATUS_INVISIBLE:
				case GG_STATUS_INVISIBLE2:
					if (has_mobile)
						pix = icons_manager.loadIcon("InvisibleWithMobile");
					else
						pix = icons_manager.loadIcon("Invisible");
					break;
				}
			lbp = new KaduListBoxPixmap(pix, user.altnick, user.description, 0);
			insertItem(lbp);
//			insertItem(*pix, user.altnick);
			}
		}
	// Dodajemy nieaktywnych
	if (config_file.readBoolEntry("General","ShowHideInactive"))
	for (i = 0; i < n_users.count(); i++)
	{
		UserListElement &user = userlist.byAltNick(n_users[i]);
		bool has_mobile = user.mobile.length();
		if (pending.pendingMsgs(user.uin)) {
			lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Message"), user.altnick, user.description, 0);
			insertItem(lbp);
//			insertItem(*icons->loadIcon("message"), user.altnick);
			}
		else
		{
			QPixmap pix;
			switch (user.status) {
				case GG_STATUS_NOT_AVAIL_DESCR:
					if (has_mobile)
						pix = icons_manager.loadIcon("OfflineWithDescriptionMobile");
					else
						pix = icons_manager.loadIcon("OfflineWithDescription");
					break;
				default:
					if (has_mobile)
						pix = icons_manager.loadIcon("OfflineWithMobile");
					else
						pix = icons_manager.loadIcon("Offline");
					break;
				}
			if (!pix.isNull())
				lbp = new KaduListBoxPixmap(pix, user.altnick, user.description, 0);
//				insertItem(*pix, user.altnick);
			else
				lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Online"), user.altnick, user.description, 0);
//				insertItem(*icons->loadIcon("online"), user.altnick);
			insertItem(lbp);
		}
	}
	// Dodajemy uzytkownikow bez numerow GG
	for (i = 0; i < b_users.count(); i++) {
		UserListElement &user = userlist.byAltNick(b_users[i]);
		lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Mobile"), user.altnick, user.description, 0);
		insertItem(lbp);
//		insertItem(*icons->loadIcon("mobile"), user.altnick);
		}
	// Przywracamy zaznaczenie wczesniej zaznaczonych uzytkownikow
	for (i = 0; i < s_users.count(); i++)
		setSelected(findItem(s_users[i]), true);
	setCurrentItem(findItem(s_user));
	kdebug("UserBox::refresh() exit\n");
}

void UserBox::addUser(const QString &altnick)
{
	Users.append(altnick);
}

void UserBox::removeUser(const QString &altnick)
{
	Users.remove(altnick);
}

void UserBox::renameUser(const QString &oldaltnick, const QString &newaltnick)
{
	kdebugf();
	QStringList::iterator it = Users.find(oldaltnick);
	if (it != Users.end())
		(*it) = newaltnick;
	else
		kdebug("Userbox::renameUser(): userbox doesnt contain: %s\n", (const char *)oldaltnick.local8Bit());
}

bool UserBox::containsAltNick(const QString &altnick)
{
	for (QStringList::iterator it = Users.begin(); it != Users.end(); it++)
		if ((*it).lower() == altnick.lower())
			return true;
	kdebug("UserBox::containsAltNick(): userbox doesnt contain: %s\n", (const char *)altnick.lower().local8Bit());
	return false;
}

void UserBox::changeAllToInactive()
{
	QPixmap qp_inact = icons_manager.loadIcon("Offline");
	for(unsigned int i=0; i<count(); i++)
		changeItem(qp_inact,item(i)->text(),i);
}

void UserBox::showHideInactive()
{
	config_file.writeEntry("General","ShowHideInactive",!config_file.readBoolEntry("General","ShowHideInactive"));
	all_refresh();
}

UinsList UserBox::getSelectedUins()
{
	UinsList uins;
	for (unsigned int i = 0; i < count(); i++)
		if (isSelected(i))
		{
			UserListElement user = userlist.byAltNick(text(i));
			if (user.uin)
				uins.append(user.uin);
		}
	return uins;
}

UserList UserBox::getSelectedUsers()
{
	UserList users;
	for (unsigned int i=0; i< count(); i++)
		if (isSelected(i))
			users.addUser(userlist.byAltNick(text(i)));
	return users;
}

UserBox* UserBox::getActiveUserBox()
{
	for (unsigned int i=0; i<UserBoxes.size(); i++)
	{
		UserBox *box=UserBoxes[i];
		if (box->isActiveWindow())
			return box;
	}
	kdebug("return NULL!\n");
	return NULL;
}

QStringList UserBox::getSelectedAltNicks()
{
	QStringList nicks;
	for (unsigned int i=0; i< count(); i++)
		if (isSelected(i))
			nicks.append(text(i));
	return nicks;
}
/////////////////////////////////////////////////////////

void UserBox::all_refresh()
{
	for(unsigned int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->refresh();
}

void UserBox::all_removeUser(QString &altnick)
{
	for(unsigned int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->removeUser(altnick);
}

void UserBox::all_changeAllToInactive()
{
	for(unsigned int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->changeAllToInactive();
}

void UserBox::all_renameUser(const QString &oldaltnick, const QString &newaltnick)
{
	for(unsigned int i = 0; i < UserBoxes.size(); i++)
		UserBoxes[i]->renameUser(oldaltnick, newaltnick);
}

void UserBox::initModule()
{
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "General"));
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Show inactive users"), "ShowHideInactive", true);

// dodanie wpisow do konfiga (pierwsze uruchomienie)
	QWidget w;
	config_file.addVariable("Look", "InfoPanelBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "InfoPanelFgColor", w.paletteForegroundColor());
	config_file.addVariable("Look", "UserboxBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "UserboxFgColor", w.paletteForegroundColor());

	QFontInfo info(qApp->font());
	QFont def_font(info.family(),info.pointSize());

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Look"));

	ConfigDialog::addSpinBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Userbox width when multi column"), "MultiColumnUserboxWidth", 1, 1000, 1, 230);

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Colors"));
		ConfigDialog::addVGroupBox("Look", "Colors", QT_TRANSLATE_NOOP("@default", "Main window"));
			ConfigDialog::addHBox("Look", "Main window", "userbox_bg_color_box");
				ConfigDialog::addLabel("Look", "userbox_bg_color_box", QT_TRANSLATE_NOOP("@default", "Userbox background color"));
				ConfigDialog::addColorButton("Look", "userbox_bg_color_box", "", "UserboxBgColor", config_file.readColorEntry("Look","UserboxBgColor"), "", "userbox_bg_color");
			ConfigDialog::addHBox("Look", "Main window", "userbox_font_color_box");
				ConfigDialog::addLabel("Look", "userbox_font_color_box", QT_TRANSLATE_NOOP("@default", "Userbox font color"));
				ConfigDialog::addColorButton("Look", "userbox_font_color_box", "", "UserboxFgColor", config_file.readColorEntry("Look","UserboxFgColor"), "", "userbox_font_color");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Fonts"));
		ConfigDialog::addSelectFont("Look", "Fonts", QT_TRANSLATE_NOOP("@default", "Font in userbox"), "UserboxFont", def_font.toString(), "", "userbox_font_box");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Previews"));
		ConfigDialog::addHBox("Look", "Previews", "othr_prvws");
			ConfigDialog::addVGroupBox("Look", "othr_prvws", QT_TRANSLATE_NOOP("@default", "Preview userbox"));
				ConfigDialog::addLabel("Look", "Preview userbox", "<b>Text</b> preview", "preview_userbox");
			ConfigDialog::addVGroupBox("Look", "othr_prvws", QT_TRANSLATE_NOOP("@default", "Preview panel"));
				ConfigDialog::addLabel("Look", "Preview panel", "<b>Text</b> preview", "preview_panel");

	UserBoxSlots *userboxslots= new UserBoxSlots();
	ConfigDialog::registerSlotOnCreate(userboxslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(userboxslots, SLOT(onDestroyConfigDialog()));

	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), userboxslots, SLOT(chooseColor(const char *, const QColor&)), "userbox_bg_color");
	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), userboxslots, SLOT(chooseColor(const char *, const QColor&)), "userbox_font_color");

	ConfigDialog::connectSlot("Look", "Font in userbox", SIGNAL(changed(const char *, const QFont&)), userboxslots, SLOT(chooseFont(const char *, const QFont&)), "userbox_font_box");
}

UserBoxMenu::UserBoxMenu(QWidget *parent, const char *name): QPopupMenu(parent, name)
{
	connect(this, SIGNAL(aboutToHide()), this, SLOT(restoreLook()));
}

int UserBoxMenu::addItem(const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	return insertItem(text, receiver, member, accel, id);
}

int UserBoxMenu::addItem(const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	return insertItem( QIconSet(icons_manager.loadIcon(iconname)) , text, receiver, member, accel, id);
}

int UserBoxMenu::addItemAtPos(int index,const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	return insertItem( QIconSet(icons_manager.loadIcon(iconname)) , text, receiver, member, accel, id, index);
}

int UserBoxMenu::getItem(const QString &caption)
{
	for (unsigned int i=0; i<count(); i++)
		if (!QString::localeAwareCompare(caption,text(idAt(i)).left(caption.length())))
			return idAt(i);
	return -1;
}

void UserBoxMenu::restoreLook()
{
	for (unsigned int i=0; i<count(); i++)
	{
		setItemEnabled(idAt(i),true);
		setItemChecked(idAt(i),false);
//		setItemVisible(idAt(i),true);
// nie ma takiej funkcji w qt 3.0.*
	}
}

void UserBoxMenu::show(QListBoxItem *item)
{
	if (item == NULL)
		return;

	emit popup();
	exec(QCursor::pos());
}

void UserBoxSlots::onCreateConfigDialog()
{
	kdebugf();
	updatePreview();
}

void UserBoxSlots::onDestroyConfigDialog()
{
	kdebugf();
	UserBox::all_refresh();
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
		kdebug("chooseColor: ups!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! '%s'\n", name);
}

void UserBoxSlots::chooseFont(const char *name, const QFont &font)
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_userbox");
	if (QString(name)=="userbox_font_box")
		preview->setFont(font);
}

void UserBoxSlots::updatePreview()
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_userbox");
	preview->setFont(config_file.readFontEntry("Look", "UserboxFont"));
	preview->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxFgColor"));
	preview->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxBgColor"));
	preview->setAlignment(Qt::AlignLeft);
}

QValueList<UserBox *> UserBox::UserBoxes;

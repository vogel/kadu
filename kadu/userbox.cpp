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

KaduListBoxPixmap::KaduListBoxPixmap(const QPixmap &pix, const QString &text)
	: QListBoxPixmap(pix, text)
{
}

void KaduListBoxPixmap::paint(QPainter *painter) {
	UserListElement &user = userlist.byAltNick(text());
	if (user.uin) {
		UinsList uins;
		uins.append(user.uin);
		if (user.blocking) {
			QPen &pen = (QPen &)painter->pen();
			pen.setColor(QColor(255, 0, 0));
			painter->setPen(pen);
			}
		else
			if (isIgnored(uins)) {
				QPen &pen = (QPen &)painter->pen();
				pen.setColor(QColor(192, 192, 0));
				painter->setPen(pen);
				}
			else
				if (user.offline_to_user) {
					QPen &pen = (QPen &)painter->pen();
					pen.setColor(QColor(128, 128, 128));
					painter->setPen(pen);
					}
		}
	QListBoxPixmap::paint(painter);
}

UserBoxMenu *UserBox::userboxmenu = NULL;

UserBox::UserBox(QWidget* parent,const char* name,WFlags f)
	: QListBox(parent,name),QToolTip(viewport())

{	
	if (!userboxmenu)
	    userboxmenu= new UserBoxMenu(this);
	
	UserBoxes.append(this);
	setSelectionMode(QListBox::Extended);
}

UserBox::~UserBox()
{
	UserBoxes.remove(this);
};

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
				s = tr("<I>Available</I>");
				break;
			case GG_STATUS_BUSY:
				s = tr("<I>Busy</I>");
				break;
			case GG_STATUS_NOT_AVAIL:
				if (!userlist.byAltNick(item->text()).uin)
				    s = tr("<I>Mobile:</I><B> ")+userlist.byAltNick(item->text()).mobile+"</B>";
				else
				    s = tr("<nobr><I>Not available</I></nobr>");
				break;
			case GG_STATUS_INVISIBLE:
			case GG_STATUS_INVISIBLE2:
				s = tr("<I>Invisible</I>");
				break;
			case GG_STATUS_INVISIBLE_DESCR:
				s = tr("<I>Invisible <B>(d.)</B></I>");
				break;
			case GG_STATUS_BUSY_DESCR:
				s = tr("<nobr><I>Busy <B>(d.)</B></I></nobr>");
				break;
			case GG_STATUS_NOT_AVAIL_DESCR:
				s = tr("<nobr><I>Not available <B>(d.)</B></I></nobr>");
				break;
			case GG_STATUS_AVAIL_DESCR:
				s = tr("<nobr><I>Available <B>(d.)</B></I></nobr>");
				break;
			case GG_STATUS_BLOCKED:
				s = tr("<nobr><I>Blocking</I></nobr>");				
				break;
			default:
				s = tr("<nobr><I>Unknown status</I></nobr>");
				break;
		};
		QString desc = userlist.byAltNick(item->text()).description;
		if (desc != "")
		{
			s += "<BR><BR>";
			s += tr("<B>Description:</B><BR>");
			escapeSpecialCharacters(desc);
			desc.replace(QRegExp(" "), "&nbsp;");
			s += desc;
		};
		tip(r, s);
	};
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
					for (int i = 0; i < count(); i++)
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
	for(int i=0; i<count(); i++)
		if(isSelected(i))
		{
			if(drag_text!="")
				drag_text+="\n";
			drag_text+=item(i)->text();
		};
        QDragObject* d = new QTextDrag(drag_text,this);
	d->dragCopy();
    }
    else
	QListBox::mouseMoveEvent(e);
};

void UserBox::keyPressEvent(QKeyEvent *e)
{
    QListBox::keyPressEvent(e);
    QWidget::keyPressEvent(e);
}

void UserBox::sortUsersByAltNick(QStringList &users) {
	int i, count;
	QString tmp;
	bool stop;

	QMap<QString,QString> usermap;
	for (i = 0; i < users.count(); i++) {
		usermap.insert(users[i].upper(), users[i]);
		users[i] = users[i].upper();
		}
	// sortowanie uzytkownikow
	count = users.count();
	do {
		stop = true;
		for (i = 0; i < count - 1; i++)
			if (users[i].localeAwareCompare(users[i+1]) > 0) {
				tmp = users[i];
				users[i] = users[i+1];
				users[i+1] = tmp;
				stop = false;
				}
		} while (!stop);
//	users.sort();

	for (i = 0; i < users.count(); i++)
		users[i] = usermap[users[i]];
}

void UserBox::refresh()
{
	int i;
	KaduListBoxPixmap *lbp;
	
	kdebug("UserBox::refresh()\n");
	
	this->setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
	this->setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
	this->QListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));

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
	for (i = 0; i < Users.count(); i++) {
		UserListElement &user = userlist.byAltNick(Users[i]);
		if (user.uin)
			switch (user.status) {
				case GG_STATUS_AVAIL:
				case GG_STATUS_AVAIL_DESCR:
				case GG_STATUS_BUSY:
				case GG_STATUS_BUSY_DESCR:
				case GG_STATUS_BLOCKED:
					a_users.append(user.altnick);
					break;
				case GG_STATUS_INVISIBLE_DESCR:
				case GG_STATUS_INVISIBLE2:
					i_users.append(user.altnick);
					break;
				default:
					n_users.append(user.altnick);
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
	for (i = 0; i < a_users.count(); i++)
	{
		UserListElement &user = userlist.byAltNick(a_users[i]);
		bool has_mobile = user.mobile.length();
		if (pending.pendingMsgs(user.uin)) {
			lbp = new KaduListBoxPixmap(*icons->loadIcon("message"), user.altnick);
			insertItem(lbp);
//			insertItem(*icons->loadIcon("message"), user.altnick);
			}
		else
		{
			QPixmap *pix = NULL;
			switch (user.status) {
				case GG_STATUS_AVAIL:
					if (has_mobile)
						pix = icons->loadIcon("online_m");
					else
		    				pix = icons->loadIcon("online");
		    			break;
				case GG_STATUS_AVAIL_DESCR:
					if (has_mobile)
						pix = icons->loadIcon("online_d_m");
					else
		    				pix = icons->loadIcon("online_d");
		    			break;
				case GG_STATUS_BUSY:
					if (has_mobile)
						pix = icons->loadIcon("busy_m");
					else
						pix = icons->loadIcon("busy");
		    			break;
				case GG_STATUS_BUSY_DESCR:
					if (has_mobile)
						pix = icons->loadIcon("busy_d_m");
					else
						pix = icons->loadIcon("busy_d");
		    			break;
				case GG_STATUS_BLOCKED:
					pix = icons->loadIcon("blocking");
					break;
				};
			if (pix != NULL)
				lbp = new KaduListBoxPixmap(*pix, user.altnick);
//				insertItem(*pix, user.altnick);
			else
				lbp = new KaduListBoxPixmap(*icons->loadIcon("online"), user.altnick);
//				insertItem(*icons->loadIcon("online"), user.altnick);
			insertItem(lbp);
		};
	};	
	// Dodajemy niewidocznych
	for (i = 0; i < i_users.count(); i++) {
		UserListElement &user = userlist.byAltNick(i_users[i]);
		bool has_mobile = user.mobile.length();
		if (pending.pendingMsgs(user.uin)) {
			lbp = new KaduListBoxPixmap(*icons->loadIcon("message"), user.altnick);
			insertItem(lbp);
//	    		insertItem(*icons->loadIcon("message"), user.altnick);
			}
		else {
			QPixmap *pix = NULL;
			switch (user.status) {
				case GG_STATUS_INVISIBLE_DESCR:
					if (has_mobile)
						pix = icons->loadIcon("invisible_d_m");
					else
						pix = icons->loadIcon("invisible_d");
    		    			break;
				case GG_STATUS_INVISIBLE2:
					if (has_mobile)
						pix = icons->loadIcon("invisible_m");
					else
						pix = icons->loadIcon("invisible");
		    			break;
				}
			lbp = new KaduListBoxPixmap(*pix, user.altnick);
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
			lbp = new KaduListBoxPixmap(*icons->loadIcon("message"), user.altnick);
			insertItem(lbp);
//	    		insertItem(*icons->loadIcon("message"), user.altnick);
			}
		else
		{
			QPixmap *pix = NULL;
			switch (user.status) {
				case GG_STATUS_NOT_AVAIL_DESCR:
					if (has_mobile)
						pix = icons->loadIcon("offline_d_m");
					else
			    			pix = icons->loadIcon("offline_d");			
    		    			break;
				default:
					if (has_mobile)
						pix = icons->loadIcon("offline_m");
					else
		    				pix = icons->loadIcon("offline");			
		    			break;
				};
			if (pix != NULL)
				lbp = new KaduListBoxPixmap(*pix, user.altnick);
//				insertItem(*pix, user.altnick);
			else
				lbp = new KaduListBoxPixmap(*icons->loadIcon("online"), user.altnick);
//				insertItem(*icons->loadIcon("online"), user.altnick);
			insertItem(lbp);
		};
	};
	// Dodajemy uzytkownikow bez numerow GG
	for (i = 0; i < b_users.count(); i++) {
		UserListElement &user = userlist.byAltNick(b_users[i]);
		lbp = new KaduListBoxPixmap(*icons->loadIcon("mobile"), user.altnick);
		insertItem(lbp);
//		insertItem(*icons->loadIcon("mobile"), user.altnick);
		}
	// Przywracamy zaznaczenie wczesniej zaznaczonych uzytkownikow
	for (i = 0; i < s_users.count(); i++)
		setSelected(findItem(s_users[i]), true);
	setCurrentItem(findItem(s_user));

	kdebug("UserBox::refresh() exit\n");
};

void UserBox::addUser(const QString &altnick)
{
	Users.append(altnick);
};

void UserBox::removeUser(const QString &altnick)
{
	Users.remove(altnick);
};

void UserBox::renameUser(const QString &oldaltnick, const QString &newaltnick)
{
	QStringList::iterator it = Users.find(oldaltnick);
	(*it) = newaltnick;
};

void UserBox::changeAllToInactive()
{
	QPixmap *qp_inact = icons->loadIcon("offline");
	for(int i=0; i<count(); i++)
		changeItem(*qp_inact,item(i)->text(),i);
};

void UserBox::showHideInactive()
{	
	config_file.writeEntry("General","ShowHideInactive",!config_file.readBoolEntry("General","ShowHideInactive"));
	all_refresh();
}


UinsList UserBox::getSelectedUins() {
	UinsList uins;
	UserListElement user;
	int j;
	for(j=0; j<UserBoxes.size(); j++)
		if (UserBoxes[j]->isActiveWindow())
		    break;

	for (int i = 0; i < UserBoxes[j]->count(); i++)
		if (UserBoxes[j]->isSelected(i)) {
			user = userlist.byAltNick(UserBoxes[j]->text(i));

			if (user.uin)
				uins.append(user.uin);
			}
	return uins;
}

UserList UserBox::getSelectedUsers()
{
    UserList users;
	int j;
	for(j=0; j<UserBoxes.size(); j++)
		if (UserBoxes[j]->isActiveWindow())
		    break;

	for (int i=0; i< UserBoxes[j]->count(); i++)
		if (UserBoxes[j]->isSelected(i))
			users.addUser(userlist.byAltNick(UserBoxes[j]->text(i)));
	return users;

}

QStringList UserBox::getSelectedAltNicks()
{
    QStringList users;

	int j;
	for(j=0; j<UserBoxes.size(); j++)
		if (UserBoxes[j]->isActiveWindow())
		    break;

	for (int i=0; i< UserBoxes[j]->count(); i++)
		if (UserBoxes[j]->isSelected(i))
		    users.append(UserBoxes[j]->text(i));
	return users;
}
/////////////////////////////////////////////////////////

void UserBox::all_refresh()
{
	for(int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->refresh();
};

void UserBox::all_removeUser(QString &altnick)
{
	for(int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->removeUser(altnick);
};

void UserBox::all_changeAllToInactive()
{
	for(int i=0; i<UserBoxes.size(); i++)
		UserBoxes[i]->changeAllToInactive();	
};

void UserBox::all_renameUser(const QString &oldaltnick, const QString &newaltnick)
{
	for(int i = 0; i < UserBoxes.size(); i++)
		UserBoxes[i]->renameUser(oldaltnick, newaltnick);	
};

void UserBox::initModule()
{

	QT_TRANSLATE_NOOP("@default", "General");
	QT_TRANSLATE_NOOP("@default", "Show Inactive users");
	
	
	ConfigDialog::addTab("General");
	ConfigDialog::addCheckBox("General", "General", "Show Inactive users", "ShowHideInactive", true);

// dodanie wpisow do konfiga (pierwsze uruchomienie)
	config_file.addVariable("Look", "UserboxDescBgColor", QColor("#C0C0C0"));
	config_file.addVariable("Look", "UserboxDescTextColor", QColor("#000000"));
	config_file.addVariable("Look", "UserboxBgColor", QColor("#FFFFFF"));
	config_file.addVariable("Look", "UserboxFgColor", QColor("#000000"));
	
//
	
	QT_TRANSLATE_NOOP("@default", "Look");
	QT_TRANSLATE_NOOP("@default", "Userbox properties");
	QT_TRANSLATE_NOOP("@default", "Preview userbox");
	QT_TRANSLATE_NOOP("@default", "Preview desc.");
	QT_TRANSLATE_NOOP("@default", "Font");
	QT_TRANSLATE_NOOP("@default", "Font size");
	QT_TRANSLATE_NOOP("@default", "Other");	
	QT_TRANSLATE_NOOP("@default", "Show userbox-desc.");	
	QT_TRANSLATE_NOOP("@default", "Display group tabs");	
	QT_TRANSLATE_NOOP("@default", "Multicolumn userbox");	
	
	
	ConfigDialog::addTab("Look");
	ConfigDialog::addVGroupBox("Look","Look","Userbox properties");
	ConfigDialog::addGrid("Look", "Userbox properties", "previewgrid", 2);
	ConfigDialog::addHGroupBox("Look","previewgrid","Preview userbox");
	ConfigDialog::addLabel("Look","Preview userbox", "<b>Text</b> preview", "userbox");
	ConfigDialog::addHGroupBox("Look","previewgrid","Preview desc.");
	ConfigDialog::addLabel("Look","Preview desc.", "<b>Text</b> preview", "desc");
	ConfigDialog::addHBox("Look", "Userbox properties", "-");
	ConfigDialog::addComboBox("Look", "-", "", "", "combobox0");
	ConfigDialog::addLineEdit2("Look", "-", "", "", "", "line0");
	ConfigDialog::addColorButton("Look", "-","ColorButton0", QColor(config_file.readEntry("Look","UserboxBgColor")));
	ConfigDialog::addHBox("Look", "Userbox properties", "font&size");
	ConfigDialog::addComboBox("Look", "font&size", "Font");
	ConfigDialog::addComboBox("Look", "font&size", "Font size");
	
	ConfigDialog::addVGroupBox("Look", "Look", "Other");
	ConfigDialog::addCheckBox("Look", "Other", "Show userbox-desc.", "ShowDesc", true);
	ConfigDialog::addCheckBox("Look", "Other", "Display group tabs", "DisplayGroupTabs", true);
	ConfigDialog::addCheckBox("Look", "Other", "Multicolumn userbox", "MultiColumnUserbox", true);

	
	UserBoxSlots *userboxslots= new UserBoxSlots();
	ConfigDialog::registerSlotOnCreate(userboxslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(userboxslots, SLOT(onDestroyConfigDialog()));
	ConfigDialog::connectSlot("Look", "ColorButton0", SIGNAL(changed()), userboxslots, SLOT(chooseColorGet()));
	ConfigDialog::connectSlot("Look", "", SIGNAL(textChanged(const QString&)), userboxslots, SLOT(chooseColorGet(const QString&)), "line0");
	ConfigDialog::connectSlot("Look", "", SIGNAL(activated(int)), userboxslots, SLOT(chooseUserBoxSelect(int)), "combobox0");
};


UserBoxMenu::UserBoxMenu(QWidget *parent, const char *name): QPopupMenu(parent, name)
{
    connect(this, SIGNAL(aboutToHide()), this, SLOT(restoreLook()));
}

int UserBoxMenu::addItem(const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	insertItem(text, receiver, member, accel, id);
}

int UserBoxMenu::addItem(const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	insertItem( QIconSet(QPixmap(QString(DATADIR)+ "/kadu/icons/"+iconname)) , text, receiver, member, accel, id);
}


int UserBoxMenu::getItem(const QString &caption)
{
    for (int i=0; i<=count(); i++)
	if (!QString::localeAwareCompare(caption,text(idAt(i)).left(caption.length())))
	{
		return idAt(i);
	}
	return -1;	
}

void UserBoxMenu::restoreLook()
{
	for (int i=0; i<=count(); i++)
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
	kdebug("UserBoxSlots::onCreateConfigDialog()\n");
	vl_userboxcolor.clear();
	vl_userboxcolor.append(config_file.readColorEntry("Look","UserboxBgColor"));
	vl_userboxcolor.append(config_file.readColorEntry("Look","UserboxDescBgColor"));
	vl_userboxcolor.append(config_file.readColorEntry("Look","UserboxFgColor"));
	vl_userboxcolor.append(config_file.readColorEntry("Look","UserboxDescTextColor"));


	QLineEdit *l_color= ConfigDialog::getLineEdit("Look", "", "line0");
	l_color->setMaxLength(7);
	l_color->setText(vl_userboxcolor[0].name());
	ColorButton *colorbutton= ConfigDialog::getColorButton("Look", "ColorButton0");
	colorbutton->setColor(vl_userboxcolor[0]);
	QComboBox *cb_userboxselect= ConfigDialog::getComboBox("Look", "","combobox0");

	cb_userboxselect->insertItem(tr("Userbox background color"));
	cb_userboxselect->insertItem(tr("Desc. background color"));
	cb_userboxselect->insertItem(tr("Font in userbox window"));
	cb_userboxselect->insertItem(tr("Font in desc."));
	cb_userboxselect->setCurrentItem(0);

	QHGroupBox *hgb_0 = ConfigDialog::getHGroupBox("Look", "Preview userbox");
	hgb_0->setAlignment(Qt::AlignCenter);
	
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "userbox");
	preview->setFont(QFont(config_file.readFontEntry("Look","UserboxFont")));
	preview->setPaletteForegroundColor(vl_userboxcolor[2]);
	preview->setPaletteBackgroundColor(vl_userboxcolor[0]);
	preview->setAlignment(Qt::AlignCenter);


	QHGroupBox *hgb_1 = ConfigDialog::getHGroupBox("Look", "Preview desc.");
	hgb_1->setAlignment(Qt::AlignCenter);

	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "desc");
	preview2->setFont(QFont(config_file.readFontEntry("Look","UserboxDescFont")));
	preview2->setPaletteForegroundColor(vl_userboxcolor[3]);
	preview2->setPaletteBackgroundColor(vl_userboxcolor[1]);
	preview2->setAlignment(Qt::AlignCenter);

	vl_userboxfont.clear();
	vl_userboxfont.append(config_file.readFontEntry("Look", "UserboxFont"));
	vl_userboxfont.append(config_file.readFontEntry("Look", "UserboxDescFont"));
	
	QHBox *h_fontsize= ConfigDialog::getHBox("Look", "font&size");
	h_fontsize->hide();

	QComboBox *cb_userboxfont= ConfigDialog::getComboBox("Look", "Font");
	QComboBox *cb_userboxfontsize= ConfigDialog::getComboBox("Look", "Font size");

	QFontDatabase fdb;
	QValueList<int> vl;
	
	vl = fdb.pointSizes(vl_userboxfont[0].family(),"Normal");
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
		cb_userboxfontsize->insertItem(QString::number(*points));
	
	cb_userboxfontsize->setCurrentText(QString::number(vl_userboxfont[0].pointSize()));
	cb_userboxfont->insertStringList(fdb.families());
	cb_userboxfont->setCurrentText(vl_userboxfont[0].family());
	
	
	connect(cb_userboxfont, SIGNAL(activated(int)), this, SLOT(chooseUserBoxFont(int)));
	connect(cb_userboxfontsize, SIGNAL(activated(int)), this, SLOT(chooseUserBoxFontSize(int)));
}

void UserBoxSlots::onDestroyConfigDialog()
{
	kdebug("UserBoxSlots::onDestroyConfigDialog()\n");
	config_file.writeEntry("Look","UserboxBgColor",vl_userboxcolor[0]);
	config_file.writeEntry("Look","UserboxDescBgColor",vl_userboxcolor[1]);
	config_file.writeEntry("Look","UserboxFgColor", vl_userboxcolor[2]);
	config_file.writeEntry("Look","UserboxDescTextColor", vl_userboxcolor[3]);
	config_file.writeEntry("Look","UserboxBgColor",vl_userboxcolor[0]);
	config_file.writeEntry("Look","UserboxDescBgColor",vl_userboxcolor[1]);
	config_file.writeEntry("Look", "UserboxFont", vl_userboxfont[0]);
	config_file.writeEntry("Look", "UserboxDescFont", vl_userboxfont[1]);
	UserBox::all_refresh();

}

void UserBoxSlots::chooseUserBoxSelect(int nr)
{
	kdebug("UserBoxSlots::chooseUserBoxSelect() item: %d\n", nr);
	ColorButton *colorbutton= ConfigDialog::getColorButton("Look", "ColorButton0");
	QLineEdit *l_color= ConfigDialog::getLineEdit("Look", "", "line0");
	colorbutton->setColor(vl_userboxcolor[nr]);
	l_color->setText(colorbutton->color().name());

	QHBox *h_fontsize= ConfigDialog::getHBox("Look", "font&size");
	QComboBox *cb_userboxfont= ConfigDialog::getComboBox("Look", "Font");
	QComboBox *cb_userboxfontsize= ConfigDialog::getComboBox("Look", "Font size");

	if (nr>1)
	{
	    h_fontsize->show();
	    cb_userboxfontsize->setCurrentText(QString::number(vl_userboxfont[nr-2].pointSize()));
	    cb_userboxfont->setCurrentText(vl_userboxfont[nr-2].family());
	}
	else
	{
	    h_fontsize->hide();
	}
}

void UserBoxSlots::chooseUserBoxFont(int nr)
{
	kdebug("UserBoxSlots::chooseUserBoxFont()\n");

	QFontDatabase fdb;
	QValueList<int> vl;
	QComboBox *cb_userboxfont= ConfigDialog::getComboBox("Look", "Font");
	QComboBox *cb_userboxfontsize= ConfigDialog::getComboBox("Look", "Font size");
	QComboBox *cb_userboxselect= ConfigDialog::getComboBox("Look", "","combobox0");
	
	vl = fdb.pointSizes(cb_userboxfont->text(nr),"Normal");
	cb_userboxfontsize->clear();
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
		cb_userboxfontsize->insertItem(QString::number(*points));
	if (cb_userboxfontsize->count() > 0) {
		cb_userboxfontsize->setCurrentItem(0);
		vl_userboxfont[cb_userboxselect->currentItem()-2] = 
		    QFont(cb_userboxfont->text(nr), cb_userboxfontsize->currentText().toInt());
					     }
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "userbox");
	preview->setFont(vl_userboxfont[0]);
	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "desc");
	preview2->setFont(vl_userboxfont[1]);

}

void UserBoxSlots::chooseUserBoxFontSize(int nr)
{
	QComboBox *cb_userboxfontsize= ConfigDialog::getComboBox("Look", "Font size");
	QComboBox *cb_userboxfont= ConfigDialog::getComboBox("Look", "Font");
	QComboBox *cb_userboxselect= ConfigDialog::getComboBox("Look", "","combobox0");
	
	vl_userboxfont[cb_userboxselect->currentItem()-2]= 
	    QFont(cb_userboxfont->currentText(), cb_userboxfontsize->currentText().toInt());

	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "userbox");
	preview->setFont(vl_userboxfont[0]);
	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "desc");
	preview2->setFont(vl_userboxfont[1]);

}

void UserBoxSlots::chooseColorGet(const QString &text)
{
	kdebug("UserBoxSlots::chooseColorGet(QString)\n");
	if ((text.length() == 7)&& (QColor(text).isValid()))
	    {
	    	ColorButton *colorbutton= ConfigDialog::getColorButton("Look", "ColorButton0");
		QLineEdit *l_color= ConfigDialog::getLineEdit("Look", "", "line0");
		colorbutton->setColor(QColor(text));
		int pos=l_color->cursorPosition();
		chooseColorGet();
		l_color->setCursorPosition(pos);
	    }
}

void UserBoxSlots::chooseColorGet()
{
	kdebug("UserBoxSlots::chooseColorGet()\n");
	ColorButton *colorbutton= ConfigDialog::getColorButton("Look", "ColorButton0");
	QLineEdit *l_color= ConfigDialog::getLineEdit("Look", "", "line0");
	QComboBox *cb_userboxselect= ConfigDialog::getComboBox("Look", "","combobox0");
	
	l_color->setText(colorbutton->color().name());
	vl_userboxcolor[cb_userboxselect->currentItem()]=colorbutton->color();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "userbox");
	preview->setPaletteForegroundColor(vl_userboxcolor[2]);
	preview->setPaletteBackgroundColor(vl_userboxcolor[0]);

	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "desc");
	preview2->setPaletteForegroundColor(vl_userboxcolor[3]);
	preview2->setPaletteBackgroundColor(vl_userboxcolor[1]);
}

QValueList<UserBox *> UserBox::UserBoxes;

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "hints.h"
#include "debug.h"
#include "dock_widget.h"
#include "config_dialog.h"
#include "config_file.h"
#include "misc.h"
#include "status.h"

#include <qapplication.h>
#include <qstylesheet.h>
#include <qregexp.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qvbox.h>

#define FRAME_WIDTH 1

Hint::Hint(QWidget *parent, const QString& text, const QPixmap& pixmap, unsigned int timeout) : QHBoxLayout(0, "Hint")
{
	kdebug("Hint::Hint\n");

	secs = timeout;
	ident = 0;
	setResizeMode(QLayout::Fixed);
	if (!pixmap.isNull() && config_file.readBoolEntry("Hints","Icons"))
	{
		icon = new QLabel(parent, "Icon");
		icon->setPixmap(pixmap);
		icon->hide();
		icon->installEventFilter(this);
		addWidget(icon);
	}
	else
		icon = NULL;

	label = new QLabel(" "+QString(text).replace(QRegExp(" "), "&nbsp;"), parent, "Label");
	label->setAlignment(AlignVCenter | Qt::AlignLeft);
	label->hide();
	label->installEventFilter(this);
	addWidget(label, 1);
}

bool Hint::nextSecond(void)
{
//	kdebug("Hint::nextSecond()\n");
	secs--;
	return secs;
}

void Hint::setShown(bool show)
{
	kdebug("Hint::setShown() show=%d\n", show);

	if (show)
	{
		if (icon != NULL)
			icon->show();
		label->show();
	}
}

void Hint::set(const QFont &font, const QColor &color, const QColor &bgcolor, unsigned int id, bool show)
{
	kdebug("Hint::set()\n");

	ident = id;
	label->setFont(font);
	if (icon != NULL)
	{
		icon->setPaletteBackgroundColor(bgcolor);
		if (show)
			icon->show();
	}
	label->setPaletteForegroundColor(color);
	bcolor = bgcolor;
	label->setPaletteBackgroundColor(bcolor);
	if (show)
		label->show();
}

bool Hint::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == label || (icon != NULL && obj == icon))
	{
		switch (ev->type())
		{
			case QEvent::Enter:
				enter();
				break;
			case QEvent::Leave:
				leave();
				break;
			case QEvent::MouseButtonPress:
			{
				QMouseEvent *mev = (QMouseEvent*)ev;
				switch (mev->button())
				{
					case Qt::LeftButton:
						emit leftButtonClicked(ident);
						break;
					case Qt::RightButton:
						emit rightButtonClicked();
						break;
					case Qt::MidButton:
						emit midButtonClicked(ident);
						break;
					default:
						return false;
				}
				break;
			}
			default:
				return false;
		}
		return true;
	}
	else
		return false;
}

void Hint::enter(void)
{
	if (icon != NULL)
		icon->setPaletteBackgroundColor(bcolor.light());
	label->setPaletteBackgroundColor(bcolor.light());
}

void Hint::leave(void)
{
	if (icon != NULL)
		icon->setPaletteBackgroundColor(bcolor);
	label->setPaletteBackgroundColor(bcolor);
}

Hint::~Hint(void)
{
	kdebug("Hint::~Hint() id=%d\n", ident);

	if (icon != NULL)
		delete icon;
	delete label;
}

HintManager::HintManager()
	: QFrame(0, "HintManager", WStyle_NoBorder | WStyle_StaysOnTop | WStyle_Tool | WX11BypassWM | WWinOwnDC)
{
	kdebug("HintManager::HintManager\n");

	setFrameStyle(QFrame::Box | QFrame::Plain);
	setLineWidth(FRAME_WIDTH);

	grid = new QGridLayout(this, 0, 0, 1, 0, "QGridLayout");
	grid->setResizeMode(QLayout::Fixed);

	hints.setAutoDelete(true);

	loadConfig();

	hint_timer = new QTimer(this);
	connect(hint_timer,SIGNAL(timeout()),this,SLOT(oneSecond()));
}

void HintManager::setHint(void) {
	kdebug("HintManager::setHint()\n");

	if (useposition)
		move(position);
	else
	{
		QPoint pos_hint;
		QSize size_hint = sizeHint();
		QPoint pos_tray = trayicon->trayPosition();
		QSize size_desk = QApplication::desktop()->size();
		if (pos_tray.x() < size_desk.width()/2)
			pos_hint.setX(pos_tray.x()+32);
		else
			pos_hint.setX(pos_tray.x()-size_hint.width());
		if (pos_tray.y() < size_desk.height()/2)
			pos_hint.setY(pos_tray.y()+32);
		else
			pos_hint.setY(pos_tray.y()-size_hint.height());
		move(pos_hint);
	}
}

void HintManager::deleteHint(unsigned int id)
{
	kdebug("HintManager::deleteHint() id=%d\n", id);
//	grid->removeItem(hints.at(id)); //Nie ma tego w qt 3.0.x
	hints.remove(id);
	if (!hints.count())
	{
	//	grid->setRows(0);
	//	grid->setCols(0);
//		grid->invalidate();
		hint_timer->stop();
		hide();
		kdebug("HintManager::deleteHint hints is empty, grid->isEmpty() %d ,grid rows=%d, grid cols=%d!!\n",grid->isEmpty(),grid->numRows(),grid->numCols());
//		QLayoutIterator it = grid->iterator();
//		Hint *item = (Hint*)it.current();
//		kdebug("cos smiesznego %s \n", item->name());
		return;
	}
	for (int i = id; i < hints.count(); i++)
		hints.at(i)->setId(i);
	setHint();
}

void HintManager::oneSecond(void)
{
	kdebug("HintManager::oneSecond()\n");
	for (int i = 0; i < hints.count(); i++)
		if (!(hints.at(i)->nextSecond()))
		{
			deleteHint(i);
			i--;
		}
}

void HintManager::leftButtonSlot(unsigned int id)
{
	kdebug("HintManager::leftButtonSlot() %d\n", id);

	deleteHint(id);
}

void HintManager::rightButtonSlot(void)
{
	kdebug("HintManager::rightButtonSlot() hints.count()=%d\n", hints.count());

//	for (int i = 0; i < hints.count(); i++)
//		grid->removeItem(hints.at(i));

	hint_timer->stop();
	hide();
	hints.clear();
}

void HintManager::midButtonSlot(unsigned int id)
{
	kdebug("HintManager::midButtonSlot() %d\n", id);
}

void HintManager::addHint(const QString& text, const QPixmap& pixmap,  const QFont &font, const QColor &color, const QColor &bgcolor, unsigned int timeout)
{
	kdebug("HintManager::addHint()\n");
	hints.append(new Hint(this, text, pixmap, timeout));
	int i = hints.count()-1;
	grid->addLayout(hints.at(i), i, 0);
	hints.at(i)->set(font, color, bgcolor, i);
	connect(hints.at(i), SIGNAL(leftButtonClicked(unsigned int)), this, SLOT(leftButtonSlot(unsigned int)));
	connect(hints.at(i), SIGNAL(rightButtonClicked()), this, SLOT(rightButtonSlot()));
	connect(hints.at(i), SIGNAL(midButtonClicked(unsigned int)), this, SLOT(midButtonSlot(unsigned int)));
	setHint();
	if (!hint_timer->isActive())
		hint_timer->start(1000);
	if (!isShown())
		show();
}

void HintManager::addHintError(const QString &error)
{
	if (hintmanager == NULL)
		return;
	if (config_file.readBoolEntry("Hints","Errors"))
		addHint("<b>"+tr("Error:")+"</b> "+error, *icons->loadIcon("blocking"), QFont(config[11][0], config[11][1].toInt()), QColor(config[11][2]), QColor(config[11][3]), config[11][4].toInt());
}

void HintManager::addHintNewMsg(const QString &nick, const QString &msg)
{
	if (config_file.readBoolEntry("Hints","ShowContentMessage"))
	{
		QString cite;
		if (msg.length() <= config_file.readNumEntry("Hints","CiteSign"))
			cite = msg;
		else
			cite = msg.left(config_file.readNumEntry("Hints","CiteSign"))+"...";
		addHint(tr("New message from")+" <b>"+nick+"<br></b> <small>"+cite+"</small>", *icons->loadIcon("message"), QFont(config[10][0], config[10][1].toInt()), QColor(config[10][2]), QColor(config[10][3]), config[10][4].toInt());
	}
	else
		addHint(tr("New message from")+" <b>"+nick+"</b>", *icons->loadIcon("message"), QFont(config[10][0], config[10][1].toInt()), QColor(config[10][2]), QColor(config[10][3]), config[10][4].toInt());
}

void HintManager::addHintNewChat(const QString &nick, const QString &msg)
{
	if (config_file.readBoolEntry("Hints","ShowContentMessage"))
	{
		QString cite;
		if (msg.length() <= config_file.readNumEntry("Hints","CiteSign"))
			cite = msg;
		else
			cite = msg.left(config_file.readNumEntry("Hints","CiteSign"))+"...";
		addHint(tr("Chat with")+" <b>"+nick+"<br></b> <small>"+cite+"</small>",*icons->loadIcon("message"), QFont(config[9][0], config[9][1].toInt()), QColor(config[9][2]), QColor(config[9][3]), config[9][4].toInt());
	}
	else
		addHint(tr("Chat with")+" <b>"+nick+"</b>",*icons->loadIcon("message"), QFont(config[9][0], config[9][1].toInt()), QColor(config[9][2]), QColor(config[9][3]), config[9][4].toInt());
}

void HintManager::addHintStatus(const UserListElement &ule, unsigned int status, unsigned int oldstatus)
{
	kdebug("HintManager::addHintStatus\n");

	bool availstatus = isAvailableStatus(status);
	bool availoldstatus = isAvailableStatus(oldstatus);
	int statusnr = statusGGToStatusNr(status);

	if (config_file.readBoolEntry("Hints", "NotifyHintAvailable") && (availstatus && !availoldstatus))
	{
		if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
			addHint(parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true), *icons->loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		else
			if (ifStatusWithDescription(status) && config_file.readBoolEntry("Hints","NotifyHintDescription"))
				addHint("<b>"+ule.altnick+" </b>"+tr("is available")+"<br> <small>"+QStyleSheet::escape(ule.description)+"</small>", *icons->loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
			else
				addHint("<b>"+ule.altnick+" </b>"+tr("is available"), *icons->loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		return;
	}

	if (config_file.readBoolEntry("Hints","NotifyHintChange") && (availstatus && availoldstatus))
	{
		if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
			addHint(parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true), *icons->loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		else
			if (ifStatusWithDescription(status) && config_file.readBoolEntry("Hints","NotifyHintDescription"))
				addHint("<b>"+ule.altnick+" </b>"+tr("changed status to")+" <i>"+qApp->translate("@default", statustext[statusnr])+"</i><br> <small>"+QStyleSheet::escape(ule.description)+"</small>", *icons->loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
			else
				addHint("<b>"+ule.altnick+" </b>"+tr("changed status to")+" <i>"+qApp->translate("@default", statustext[statusnr])+"</i>", *icons->loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		return;
	}

	if (config_file.readBoolEntry("Hints", "NotifyHintUnavailable") && (!availstatus && availoldstatus))
	{
		if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
			addHint(parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true), *icons->loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		else
			if (ifStatusWithDescription(status) && config_file.readBoolEntry("Hints","NotifyHintDescription"))
				addHint("<b>"+ule.altnick+" </b>"+tr("is unavailable")+"<br> <small>"+QStyleSheet::escape(ule.description)+"</small>", *icons->loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
			else
				addHint("<b>"+ule.altnick+" </b>"+tr("is unavailable"), *icons->loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		return;
	}
}

void HintManager::loadConfig(void)
{
	config.clear();
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintOnline")));
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintOnlineD")));
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintBusy")));
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintBusyD")));
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintInvisible")));
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintInvisibleD")));
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintOffline")));
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintOfflineD")));
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintBlocking")));
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintNewChat")));
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintNewMessage")));
	config.append(QStringList::split(",",config_file.readEntry("Hints","HintError")));
	useposition = config_file.readBoolEntry("Hints","UseUserPosition");
	position = config_file.readPointEntry("Hints","HintsPosition");

	switch(config_file.readNumEntry("Hints","NewHintUnder"))
	{
		case 0:
			if (trayicon != NULL && !useposition)
			{
				if (trayicon->trayPosition().y() < QApplication::desktop()->size().height()/2)
					grid->setOrigin(QGridLayout::TopLeft);
				else
					grid->setOrigin(QGridLayout::BottomLeft);
			}
			else
			{
				if (position.y() < QApplication::desktop()->size().height()/2)
					grid->setOrigin(QGridLayout::TopLeft);
				else
					grid->setOrigin(QGridLayout::BottomLeft);
				useposition = true;
			}
			break;
		case 1:
			grid->setOrigin(QGridLayout::BottomLeft);
			break;
		case 2:
			grid->setOrigin(QGridLayout::TopLeft);
			break;
	}
}
void HintManager::initModule(void)
{
	kdebug("HintManager::initModule()\n");

/* T³umaczenia */
	QT_TRANSLATE_NOOP("@default", "Hints");
	QT_TRANSLATE_NOOP("@default", "Enable hints");
	QT_TRANSLATE_NOOP("@default", "Hints options");
	QT_TRANSLATE_NOOP("@default", "Enable icons in hints");
	QT_TRANSLATE_NOOP("@default", "Show connection errors in hints");
	QT_TRANSLATE_NOOP("@default", "Notify about new message");
	QT_TRANSLATE_NOOP("@default", "Notify about new chat");
	QT_TRANSLATE_NOOP("@default", "Show in notify content message");
	QT_TRANSLATE_NOOP("@default", "Show message contents in hint");
	QT_TRANSLATE_NOOP("@default", "Enable status notification by hint");
	QT_TRANSLATE_NOOP("@default", "Notification options");
	QT_TRANSLATE_NOOP("@default", "Notify about user status change");
	QT_TRANSLATE_NOOP("@default", "Notify about user become available");
	QT_TRANSLATE_NOOP("@default", "Notify about user become unavailable");
	QT_TRANSLATE_NOOP("@default", "Add description to hint if exists");
	QT_TRANSLATE_NOOP("@default", "Use custom syntax");
	QT_TRANSLATE_NOOP("@default", "Hint syntax");

/* Zak³adka konfiguracyjna */
	ConfigDialog::registerTab("Hints");
	ConfigDialog::addCheckBox("Hints", "Hints", "Enable hints", "Hints", true);
	ConfigDialog::addVGroupBox("Hints", "Hints", "Hints options");
	ConfigDialog::addGrid("Hints", "Hints options", "grid-options", 2);
	ConfigDialog::addCheckBox("Hints", "grid-options", "Enable icons in hints", "Icons", true);
	ConfigDialog::addCheckBox("Hints", "grid-options", "Show connection errors in hints" ,"Errors", true);
	ConfigDialog::addCheckBox("Hints", "grid-options", "Notify about new chat", "NotifyNewChat", true);
	ConfigDialog::addCheckBox("Hints", "grid-options", "Notify about new message", "NotifyNewMessage", false);
	ConfigDialog::addCheckBox("Hints", "grid-options", "Show in notify content message", "ShowContentMessage", false);
	ConfigDialog::addVGroupBox("Hints", "Hints options", "Content message in hint");
	ConfigDialog::addCheckBox("Hints", "Hints options", "Enable status notification by hint", "NotifyHint", true);
	ConfigDialog::addVGroupBox("Hints", "Hints options", "Notification options");
	ConfigDialog::addGrid("Hints", "Notification options", "grid-notify-status", 2);
	ConfigDialog::addCheckBox("Hints", "grid-notify-status", "Notify about user status change", "NotifyHintChange", false);
	ConfigDialog::addCheckBox("Hints", "grid-notify-status", "Notify about user become available", "NotifyHintAvailable", true);
	ConfigDialog::addCheckBox("Hints", "grid-notify-status", "Notify about user become unavailable", "NotifyHintUnavailable", false);
	//ConfigDialog::addCheckBox("grid-notify-status", "Prevent autoaway notify",  "NotifyHintPreventAutoaway", true);
	ConfigDialog::addCheckBox("Hints", "grid-notify-status", "Add description to hint if exists", "NotifyHintDescription", false);
	ConfigDialog::addCheckBox("Hints", "grid-notify-status", "Use custom syntax", "NotifyHintUseSyntax", false);
	ConfigDialog::addLineEdit("Hints",  "Notification options", "Hint syntax", "NotifyHintSyntax", "");
	config_file.addVariable("Hints","NewHintUnder",0);
	config_file.addVariable("Hints","UseUserPosition",false);
	config_file.addVariable("Hints","HintsPosition",QPoint(0,0));
	config_file.addVariable("Hints","CiteSign",50);
	QFontInfo info(QApplication::font());
	QFont def_font(info.family(),info.pointSize());
	config_file.addVariable("Hints","HintOnline",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	config_file.addVariable("Hints","HintOnlineD",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	config_file.addVariable("Hints","HintBusy",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	config_file.addVariable("Hints","HintBusyD",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	config_file.addVariable("Hints","HintInvisible",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	config_file.addVariable("Hints","HintInvisibleD",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	config_file.addVariable("Hints","HintOffline",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	config_file.addVariable("Hints","HintOfflineD",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	config_file.addVariable("Hints","HintBlocking",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	config_file.addVariable("Hints","HintNewChat",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	config_file.addVariable("Hints","HintNewMessage",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	config_file.addVariable("Hints","HintError",def_font.family()+","+QString::number(def_font.pointSize())+",#000000,#F0F0F0,10");
	
	HintManagerSlots *hintmanagerslots = new HintManagerSlots();
	ConfigDialog::registerSlotOnCreate(hintmanagerslots,SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(hintmanagerslots,SLOT(onDestroyConfigDialog()));
}

void HintManagerSlots::onCreateConfigDialog()
{
	kdebug("HintManagerSlots::onCreateConfigDialog()\n");
	newhintunder = config_file.readNumEntry("Hints","NewHintUnder");
	useposition = config_file.readBoolEntry("Hints","UseUserPosition");
	hintsposition = config_file.readPointEntry("Hints","HintsPosition");
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintOnline")));
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintOnlineD")));
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintBusy")));
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintBusyD")));
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintInvisible")));
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintInvisibleD")));
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintOffline")));
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintOfflineD")));
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintBlocking")));
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintNewChat")));
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintNewMessage")));
	hint.append(QStringList::split(",",config_file.readEntry("Hints","HintError")));
	b_hint = ConfigDialog::getCheckBox("Hints", "Enable hints");
	QVGroupBox *hintgrp = ConfigDialog::getVGroupBox("Hints", "Hints options");
	QCheckBox *b_notify = ConfigDialog::getCheckBox("Hints", "Enable status notification by hint");
	QCheckBox *b_syntax = ConfigDialog::getCheckBox("Hints", "Use custom syntax");
	QVGroupBox *notifygrp = ConfigDialog::getVGroupBox("Hints", "Notification options");
	QLineEdit *e_syntax = ConfigDialog::getLineEdit("Hints", "Hint syntax");
	QCheckBox *b_showcontent = ConfigDialog::getCheckBox("Hints", "Show in notify content message");
	QVGroupBox *messagegrp = ConfigDialog::getVGroupBox("Hints", "Content message in hint");

	messagegrp->setEnabled(b_showcontent->isChecked());
	QHBox *h_msg = new QHBox(messagegrp);
	h_msg->setSpacing(2);
	
	QLabel *l_citesign = new QLabel(tr("Number of quoted characters"),h_msg);

	sb_citesign = new QSpinBox(10, 1000, 1, h_msg);
	sb_citesign->setValue(config_file.readNumEntry("Hints","CiteSign"));
	
	QHBox *hbox = new QHBox(hintgrp);

	QVGroupBox *vboxgrp = new QVGroupBox(tr("New hints add to hint"),hbox);
	combobox2 = new QComboBox(vboxgrp);
	combobox2->insertItem(tr("Auto"));
	combobox2->insertItem(tr("In top"));
	combobox2->insertItem(tr("In bottom"));
	combobox2->setCurrentItem(newhintunder);

	QVBox *vbox = new QVBox(hbox);
	
	b_useposition = new QCheckBox(tr("Use custom hints position"),vbox);
	b_useposition->setChecked(useposition);
	
	QVGroupBox *vboxgrp2 = new QVGroupBox(tr("Hints position"), vbox);
	vboxgrp2->setEnabled(useposition);
	QHBox *hxybox = new QHBox(vboxgrp2);
	QLabel *labelx = new QLabel("x = ",hxybox);
	e_posx = new QLineEdit(hxybox);
	e_posx->setText(QString::number(hintsposition.x()));
	QLabel *labely = new QLabel("y = ",hxybox);
	e_posy = new QLineEdit(hxybox);
	e_posy->setText(QString::number(hintsposition.y()));
	
	hintgrp->setEnabled(b_hint->isChecked());
	notifygrp->setEnabled(b_notify->isChecked());
	e_syntax->parentWidget()->setEnabled(b_syntax->isChecked());

	QHBox *hb_0 = new QHBox(hintgrp);
	hb_0->setSpacing(2);

	cb_notify = new QComboBox(hb_0);
	cb_notify->insertItem(tr("Online"));
	cb_notify->insertItem(tr("Online (d.)"));
	cb_notify->insertItem(tr("Busy"));
	cb_notify->insertItem(tr("Busy (d.)"));
	cb_notify->insertItem(tr("Invisible"));
	cb_notify->insertItem(tr("Invisible (d.)"));
	cb_notify->insertItem(tr("Offline"));
	cb_notify->insertItem(tr("Offline (d.)"));
	cb_notify->insertItem(tr("Blocking"));
	cb_notify->insertItem(tr("New chat"));
	cb_notify->insertItem(tr("New message in chat"));
	cb_notify->insertItem(tr("Error"));

	QLabel *l_timeout = new QLabel(tr("Hint timeout (in seconds)"), hb_0);

	sb_timeout = new QSpinBox(1,600,1,hb_0);
	sb_timeout->setValue(hint[cb_notify->currentItem()][4].toInt());

	QHBox *hb_1 = new QHBox(hintgrp);
	hb_1->setSpacing(2);

	QHGroupBox *hgb_0 = new QHGroupBox(tr("Preview"),hb_1);
	hgb_0->setAlignment(Qt::AlignCenter);

	preview = new QLabel(tr("<b>Text</b> preview"),hgb_0);
	preview->setFont(QFont(hint[cb_notify->currentItem()][0],hint[cb_notify->currentItem()][1].toInt()));
	preview->setPaletteForegroundColor(QColor(hint[cb_notify->currentItem()][2]));
	preview->setPaletteBackgroundColor(QColor(hint[cb_notify->currentItem()][3]));
	preview->setAlignment(Qt::AlignCenter);

	QVBox *vb_0 = new QVBox(hb_1);
	vb_0->setSpacing(2);
	QPushButton *pb_fontcolor = new QPushButton(tr("Change font color"),vb_0);
	QPushButton *pb_bgcolor = new QPushButton(tr("Change background color"),vb_0);
	QPushButton *pb_font = new QPushButton(tr("Change font"),vb_0);

	connect(pb_fontcolor,SIGNAL(clicked()),this,SLOT(changeFontColor()));
	connect(pb_bgcolor,SIGNAL(clicked()),this,SLOT(changeBackgroundColor()));
	connect(pb_font,SIGNAL(clicked()),this,SLOT(changeFont()));
	connect(b_hint,SIGNAL(toggled(bool)),hintgrp,SLOT(setEnabled(bool)));
	connect(b_notify,SIGNAL(toggled(bool)),notifygrp,SLOT(setEnabled(bool)));
	connect(b_syntax,SIGNAL(toggled(bool)),e_syntax->parent(),SLOT(setEnabled(bool)));
	connect(cb_notify,SIGNAL(activated(int)),this,SLOT(activatedChanged(int)));
	connect(sb_timeout,SIGNAL(valueChanged(int)),this,SLOT(changeTimeout(int)));
	connect(b_useposition,SIGNAL(toggled(bool)),vboxgrp2,SLOT(setEnabled(bool)));
	connect(b_showcontent,SIGNAL(toggled(bool)),messagegrp,SLOT(setEnabled(bool)));
}

void HintManagerSlots::onDestroyConfigDialog()
{
	kdebug("HintManagerSlots::onDestroyConfigDialog()\n");
	config_file.writeEntry("Hints", "NewHintUnder", combobox2->currentItem());
	config_file.writeEntry("Hints", "UseUserPosition", b_useposition->isChecked());
	config_file.writeEntry("Hints", "HintsPosition", QPoint(e_posx->text().toInt(),e_posy->text().toInt()));
	config_file.writeEntry("Hints", "CiteSign", sb_citesign->value());
	config_file.writeEntry("Hints", "HintOnline", hint[0].join(","));
	config_file.writeEntry("Hints", "HintOnlineD", hint[1].join(","));
	config_file.writeEntry("Hints", "HintBusy", hint[2].join(","));
	config_file.writeEntry("Hints", "HintBusyD", hint[3].join(","));
	config_file.writeEntry("Hints", "HintInvisible", hint[4].join(","));
	config_file.writeEntry("Hints", "HintInvisibleD", hint[5].join(","));
	config_file.writeEntry("Hints", "HintOffline", hint[6].join(","));
	config_file.writeEntry("Hints", "HintOfflineD", hint[7].join(","));
	config_file.writeEntry("Hints", "HintBlocking", hint[8].join(","));
	config_file.writeEntry("Hints", "HintNewChat", hint[9].join(","));
	config_file.writeEntry("Hints", "HintNewMessage", hint[10].join(","));
	config_file.writeEntry("Hints", "HintError", hint[11].join(","));

	if (hintmanager != NULL)
	{
		switch(b_hint->isChecked())
		{
			case true:
				hintmanager->loadConfig();
				break;
			case false:
				delete hintmanager;
				hintmanager = NULL;
				break;
		}
	}
	else
		if (b_hint->isChecked())
			hintmanager = new HintManager();
}

void HintManagerSlots::activatedChanged(int index)
{
	preview->setFont(QFont(hint[index][0],hint[index][1].toInt()));
	preview->setPaletteForegroundColor(QColor(hint[index][2]));
	preview->setPaletteBackgroundColor(QColor(hint[index][3]));
	sb_timeout->setValue(hint[index][4].toInt());
}

void HintManagerSlots::changeFontColor()
{
	QColor color = QColorDialog::getColor(preview->paletteForegroundColor(), 0, tr("Color dialog"));
	if (color.isValid())
	{
		preview->setPaletteForegroundColor(color);
		hint[cb_notify->currentItem()][2] = color.name();
	}
}

void HintManagerSlots::changeBackgroundColor()
{
	QColor color = QColorDialog::getColor(preview->paletteBackgroundColor(), 0, tr("Color dialog"));
	if (color.isValid())
	{
		preview->setPaletteBackgroundColor(color);
		hint[cb_notify->currentItem()][3] = color.name();
	}
}
void HintManagerSlots::changeFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, preview->font(), 0);
	if ( ok )
	{
		QFont font2;
		font2.setFamily(font.family());
		font2.setPointSize(font.pointSize());
		preview->setFont(font2);
		hint[cb_notify->currentItem()][0] = font2.family();
		hint[cb_notify->currentItem()][1] = QString::number(font2.pointSize());
	}
}

void HintManagerSlots::changeTimeout(int value)
{
	hint[cb_notify->currentItem()][4] = QString::number(value);
}

HintManager *hintmanager = NULL;

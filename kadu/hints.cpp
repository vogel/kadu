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
#include "config_dialog.h"
#include "config_file.h"
#include "pending_msgs.h"
#include "status.h"
#include "chat.h"

#include <qapplication.h>
#include <qstylesheet.h>
#include <qregexp.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qvbox.h>
#include <qglobal.h>

#define FRAME_WIDTH 1

Hint::Hint(QWidget *parent, const QString& text, const QPixmap& pixmap, unsigned int timeout) : QHBoxLayout(0, "Hint")
{
	kdebugf();

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
	label->setTextFormat(Qt::RichText);
	label->setAlignment(AlignVCenter | Qt::AlignLeft);
	label->hide();
	label->installEventFilter(this);
	addWidget(label, 1);
	kdebugf2();
}

bool Hint::nextSecond(void)
{
	secs--;
	return secs;
}

void Hint::setShown(bool show)
{
	kdebugm(KDEBUG_FUNCTION_START, "Hint::setShown() show=%d\n", show);

	if (show)
	{
		if (icon != NULL)
			icon->show();
		label->show();
	}
	kdebugf2();
}

void Hint::set(const QFont &font, const QColor &color, const QColor &bgcolor, unsigned int id, bool show)
{
	kdebugf();

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
	kdebugf2();
}

bool Hint::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == label || (icon != NULL && obj == icon))
	{
		switch (ev->type())
		{
			case QEvent::Enter:
				enter();
				return true;
				break;
			case QEvent::Leave:
				leave();
				return true;
				break;
			case QEvent::MouseButtonPress:
			{
				switch ((dynamic_cast<QMouseEvent*>(ev))->button())
				{
					case Qt::LeftButton:
						emit leftButtonClicked(ident);
						return true;
						break;
					case Qt::RightButton:
						emit rightButtonClicked(ident);
						return true;
						break;
					case Qt::MidButton:
						emit midButtonClicked(ident);
						return true;
						break;
					default:
						return false;
						break;
				}
			}
			default:
				return false;
				break;
		}
	}
	else
		return QHBoxLayout::eventFilter(obj, ev);
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
	kdebugm(KDEBUG_FUNCTION_START, "Hint::~Hint() id=%d\n", ident);

	if (icon != NULL)
	{
#if QT_VERSION < 0x030100
		icon->hide();
#endif
		icon->deleteLater();
	}
#if QT_VERSION < 0x030100
	label->hide();
#endif
	label->deleteLater();
	kdebugf2();
}

HintManager::HintManager()
	: QFrame(0, "HintManager", WStyle_NoBorder | WStyle_StaysOnTop | WStyle_Tool | WX11BypassWM | WWinOwnDC)
{
	kdebugf();

	setFrameStyle(QFrame::Box | QFrame::Plain);
	setLineWidth(FRAME_WIDTH);

	grid = new QGridLayout(this, 0, 0, 1, 0, "QGridLayout");
	grid->setResizeMode(QLayout::Fixed);

	hints.setAutoDelete(true);

	loadConfig();

	hint_timer = new QTimer(this);
	connect(hint_timer,SIGNAL(timeout()),this,SLOT(oneSecond()));
	kdebugf2();
}

void HintManager::setHint(void) {
	kdebugf();
	QPoint detected_pos;
	emit searchingForPosition(detected_pos);
	if (useposition || detected_pos.isNull())
		move(position);
	else
	{
		QPoint pos_hint;
		QSize size_hint = sizeHint();
		QPoint pos_tray = detected_pos;
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
	kdebugf2();
}

void HintManager::deleteHint(unsigned int id)
{
	kdebugm(KDEBUG_FUNCTION_START, "HintManager::deleteHint() id=%d\n", id);
#if QT_VERSION >= 0x030100
	grid->removeItem(hints.at(id));
#endif
	hints.remove(id);
	if (!hints.count())
	{
		hint_timer->stop();
		hide();
		return;
	}
	for (unsigned int i = id; i < hints.count(); i++)
		hints.at(i)->setId(i);
	setHint();
	kdebugf2();
}

void HintManager::oneSecond(void)
{
	kdebugf();

	for (unsigned int i = 0; i < hints.count(); i++)
		if (!(hints.at(i)->nextSecond()))
			deleteHint(i--);
	kdebugf2();
}

void HintManager::leftButtonSlot(unsigned int id)
{
	kdebugm(KDEBUG_FUNCTION_START, "HintManager::leftButtonSlot() %d\n", id);
	switch(config_file.readNumEntry("Hints", "LeftButton")){
		case 1:
			openChat(id);
			break;
		case 2:
			deleteHint(id);
			break;
		case 3:
			deleteAllHints();
			break;
	}
	kdebugf2();
}

void HintManager::rightButtonSlot(unsigned int id)
{
	kdebugm(KDEBUG_FUNCTION_START, "HintManager::rightButtonSlot() %d\n", id);
	switch(config_file.readNumEntry("Hints", "RightButton")){
		case 1:
			openChat(id);
			break;
		case 2:
			deleteHint(id);
			break;
		case 3:
			deleteAllHints();
			break;
	}
	kdebugf2();
}

void HintManager::midButtonSlot(unsigned int id)
{
	kdebugm(KDEBUG_FUNCTION_START, "HintManager::midButtonSlot() %d\n", id);
	switch(config_file.readNumEntry("Hints", "MiddleButton")){
		case 1:
			openChat(id);
			break;
		case 2:
			deleteHint(id);
			break;
		case 3:
			deleteAllHints();
			break;
	}
	kdebugf2();
}

void HintManager::openChat(unsigned int id)
{
	kdebugf();
	UinsList senders=hints.at(id)->getUins();
	if(senders.size()!=0)
		chat_manager->openPendingMsgs(senders);
	deleteHint(id);
	kdebugf2();
}

void HintManager::deleteAllHints()
{
	hint_timer->stop();
#if QT_VERSION >= 0x030100
	for (unsigned int i = 0; i < hints.count(); i++)
		grid->removeItem(hints.at(i));
#endif
	hints.clear();
	hide();

}

void HintManager::addHint(const QString& text, const QPixmap& pixmap,  const QFont &font, const QColor &color, const QColor &bgcolor, unsigned int timeout, UinsList* senders)
{
	if (this==NULL)
		return;
	kdebugf();
	hints.append(new Hint(this, text, pixmap, timeout));
	int i = hints.count()-1;
	setGridOrigin();
	grid->addLayout(hints.at(i), i, 0);
	hints.at(i)->set(font, color, bgcolor, i);
	if(senders)
		hints.at(i)->setUins(*senders);

	connect(hints.at(i), SIGNAL(leftButtonClicked(unsigned int)), this, SLOT(leftButtonSlot(unsigned int)));
	connect(hints.at(i), SIGNAL(rightButtonClicked(unsigned int)), this, SLOT(rightButtonSlot(unsigned int)));
	connect(hints.at(i), SIGNAL(midButtonClicked(unsigned int)), this, SLOT(midButtonSlot(unsigned int)));
	setHint();
	if (!hint_timer->isActive())
		hint_timer->start(1000);
	if (isHidden())
		show();
	kdebugf2();
}

void HintManager::addHintError(const QString &error)
{
	if (this == NULL)
		return;
	kdebugf();

	if (config_file.readBoolEntry("Hints","Errors"))
		addHint("<b>"+tr("Error:")+"</b> "+error, icons_manager.loadIcon("Blocking"), QFont(config[11][0], config[11][1].toInt()), QColor(config[11][2]), QColor(config[11][3]), config[11][4].toInt());
	kdebugf2();
}

void HintManager::addHintNewMsg(const QString &nick, const QString &msg)
{
	if (this == NULL)
		return;
	kdebugf();

	if (config_file.readBoolEntry("Hints","ShowContentMessage"))
	{
		unsigned int citeSign=config_file.readUnsignedNumEntry("Hints","CiteSign");
		QString cite;
		if (msg.length() <= citeSign)
			cite = msg;
		else
			cite = msg.left(citeSign)+"...";
		addHint(tr("New message from")+" <b>"+nick+"<br/></b> <small>"+cite+"</small>", icons_manager.loadIcon("Message"), QFont(config[10][0], config[10][1].toInt()), QColor(config[10][2]), QColor(config[10][3]), config[10][4].toInt());
	}
	else
		addHint(tr("New message from")+" <b>"+nick+"</b>", icons_manager.loadIcon("Message"), QFont(config[10][0], config[10][1].toInt()), QColor(config[10][2]), QColor(config[10][3]), config[10][4].toInt());
	kdebugf2();
}

void HintManager::addHintNewChat(UinsList& senders, const QString &msg)
{
	if (this == NULL)
		return;
	kdebugf();

	QString nick=userlist.byUinValue(senders[0]).altnick;
	
	if (config_file.readBoolEntry("Hints","ShowContentMessage"))
	{
		unsigned int citeSign=config_file.readUnsignedNumEntry("Hints","CiteSign");
		QString cite;
		if (msg.length() <= citeSign)
			cite = msg;
		else
			cite = msg.left(citeSign)+"...";
		addHint(tr("Chat with")+" <b>"+nick+"<br/></b> <small>"+cite+"</small>",icons_manager.loadIcon("Message"), QFont(config[9][0], config[9][1].toInt()), QColor(config[9][2]), QColor(config[9][3]), config[9][4].toInt(), &senders);
	}
	else
		addHint(tr("Chat with")+" <b>"+nick+"</b>",icons_manager.loadIcon("Message"), QFont(config[9][0], config[9][1].toInt()), QColor(config[9][2]), QColor(config[9][3]), config[9][4].toInt(), &senders);
	kdebugf2();
}

void HintManager::addHintStatus(const UserListElement &ule, unsigned int status, unsigned int oldstatus)
{
	if (this == NULL)
		return;
	kdebugf();

	bool availstatus = isAvailableStatus(status);
	bool availoldstatus = isAvailableStatus(oldstatus);
	int statusnr = statusGGToStatusNr(status);

	if (config_file.readBoolEntry("Hints", "NotifyHintAvailable") && (availstatus && !availoldstatus))
	{
		if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
			addHint(parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true), icons_manager.loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		else
			if (ifStatusWithDescription(status) && config_file.readBoolEntry("Hints","NotifyHintDescription"))
				addHint("<b>"+ule.altnick+" </b>"+tr("is available")+"<br> <small>"+QStyleSheet::escape(ule.description)+"</small>", icons_manager.loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
			else
				addHint("<b>"+ule.altnick+" </b>"+tr("is available"), icons_manager.loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		kdebugf2();
		return;
	}

	if (config_file.readBoolEntry("Hints","NotifyHintChange") && (availstatus && availoldstatus))
	{
		if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
			addHint(parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true), icons_manager.loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		else
			if (ifStatusWithDescription(status) && config_file.readBoolEntry("Hints","NotifyHintDescription"))
				addHint("<b>"+ule.altnick+" </b>"+tr("changed status to")+" <i>"+qApp->translate("@default", statustext[statusnr])+"</i><br/> <small>"+QStyleSheet::escape(ule.description)+"</small>", icons_manager.loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
			else
				addHint("<b>"+ule.altnick+" </b>"+tr("changed status to")+" <i>"+qApp->translate("@default", statustext[statusnr])+"</i>", icons_manager.loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		kdebugf2();
		return;
	}

	if (config_file.readBoolEntry("Hints", "NotifyHintUnavailable") && (!availstatus && availoldstatus))
	{
		if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
			addHint(parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true), icons_manager.loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		else
			if (ifStatusWithDescription(status) && config_file.readBoolEntry("Hints","NotifyHintDescription"))
				addHint("<b>"+ule.altnick+" </b>"+tr("is unavailable")+"<br/> <small>"+QStyleSheet::escape(ule.description)+"</small>", icons_manager.loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
			else
				addHint("<b>"+ule.altnick+" </b>"+tr("is unavailable"), icons_manager.loadIcon(gg_icons[statusnr]), QFont(config[statusnr][0], config[statusnr][1].toInt()), QColor(config[statusnr][2]), QColor(config[statusnr][3]), config[statusnr][4].toInt());
		kdebugf2();
		return;
	}
	kdebugf2();
}

void HintManager::loadConfig(void)
{
	kdebugf();
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
	kdebugf2();
}

void HintManager::setGridOrigin()
{
	kdebugf();
	QPoint detected_pos;
	emit searchingForPosition(detected_pos);
	switch(config_file.readNumEntry("Hints","NewHintUnder"))
	{
		case 0:
			if (detected_pos.isNull() || useposition)
			{
				if (position.y() < QApplication::desktop()->size().height()/2)
					grid->setOrigin(QGridLayout::TopLeft);
				else
					grid->setOrigin(QGridLayout::BottomLeft);
			}
			else			
			{
				if (detected_pos.y() < QApplication::desktop()->size().height()/2)
					grid->setOrigin(QGridLayout::TopLeft);
				else
					grid->setOrigin(QGridLayout::BottomLeft);
			}
			break;
		case 1:
			grid->setOrigin(QGridLayout::BottomLeft);
			break;
		case 2:
			grid->setOrigin(QGridLayout::TopLeft);
			break;
	}
	kdebugf2();
}

void HintManager::initModule(void)
{
	kdebugf();

/* Zak³adka konfiguracyjna */
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Hints"));
	ConfigDialog::addCheckBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "Enable hints"), "Hints", true);
	ConfigDialog::addVGroupBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "Hints options"));
	ConfigDialog::addGrid("Hints", "Hints options", "grid-options", 1);
	ConfigDialog::addCheckBox("Hints", "grid-options", QT_TRANSLATE_NOOP("@default", "Enable icons in hints"), "Icons", true);
	ConfigDialog::addCheckBox("Hints", "grid-options", QT_TRANSLATE_NOOP("@default", "Show connection errors in hints"),"Errors", true);
	ConfigDialog::addCheckBox("Hints", "grid-options", QT_TRANSLATE_NOOP("@default", "Notify about new chat"), "NotifyNewChat", true);
	ConfigDialog::addCheckBox("Hints", "grid-options", QT_TRANSLATE_NOOP("@default", "Notify about new message"), "NotifyNewMessage", false);
	ConfigDialog::addCheckBox("Hints", "grid-options", QT_TRANSLATE_NOOP("@default", "Show message content in hint"), "ShowContentMessage", false);
	ConfigDialog::addVGroupBox("Hints", "Hints options", QT_TRANSLATE_NOOP("@default", "Message content in hint"));
	ConfigDialog::addCheckBox("Hints", "Hints options", QT_TRANSLATE_NOOP("@default", "Enable status notification by hint"), "NotifyHint", true);
	ConfigDialog::addVGroupBox("Hints", "Hints options", QT_TRANSLATE_NOOP("@default", "Notification options"));
	ConfigDialog::addGrid("Hints", "Notification options", "grid-notify-status", 1);
	ConfigDialog::addCheckBox("Hints", "grid-notify-status", QT_TRANSLATE_NOOP("@default", "Notify about users' status changes"), "NotifyHintChange", false);
	ConfigDialog::addCheckBox("Hints", "grid-notify-status", QT_TRANSLATE_NOOP("@default", "Notify about users becoming available"), "NotifyHintAvailable", true);
	ConfigDialog::addCheckBox("Hints", "grid-notify-status", QT_TRANSLATE_NOOP("@default", "Notify about users becoming unavailable"), "NotifyHintUnavailable", false);
	//ConfigDialog::addCheckBox("grid-notify-status", "Prevent autoaway notify",  "NotifyHintPreventAutoaway", true);
	ConfigDialog::addCheckBox("Hints", "grid-notify-status", QT_TRANSLATE_NOOP("@default", "Add description to hint if exists"), "NotifyHintDescription", false);
	ConfigDialog::addCheckBox("Hints", "grid-notify-status", QT_TRANSLATE_NOOP("@default", "Use custom syntax"), "NotifyHintUseSyntax", false);
	ConfigDialog::addLineEdit("Hints",  "Notification options", QT_TRANSLATE_NOOP("@default", "Hint syntax"), "NotifyHintSyntax", "");
	
	ConfigDialog::addVGroupBox("Hints", "Hints options", QT_TRANSLATE_NOOP("@default", "Mouse buttons"));
	ConfigDialog::addComboBox("Hints", "Mouse buttons", QT_TRANSLATE_NOOP("@default", "Left button"));
	ConfigDialog::addComboBox("Hints", "Mouse buttons", QT_TRANSLATE_NOOP("@default", "Middle button"));
	ConfigDialog::addComboBox("Hints", "Mouse buttons", QT_TRANSLATE_NOOP("@default", "Right button"));

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

	config_file.addVariable("Hints", "LeftButton", 1);
	config_file.addVariable("Hints", "RightButton", 2);
	config_file.addVariable("Hints", "MiddleButton", 3);

	/* FIXME */
	HintManagerSlots *hintmanagerslots = new HintManagerSlots(); 
	ConfigDialog::registerSlotOnCreate(hintmanagerslots,SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(hintmanagerslots,SLOT(onDestroyConfigDialog()));
	kdebugf2();
}

void HintManagerSlots::onCreateConfigDialog()
{
	kdebugf();
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
	QCheckBox *b_showcontent = ConfigDialog::getCheckBox("Hints", "Show message content in hint");
	QVGroupBox *messagegrp = ConfigDialog::getVGroupBox("Hints", "Message content in hint");
	QComboBox* cb_left = ConfigDialog::getComboBox("Hints", "Left button");
	QComboBox* cb_right = ConfigDialog::getComboBox("Hints", "Right button");
	QComboBox* cb_middle = ConfigDialog::getComboBox("Hints", "Middle button");

	messagegrp->setEnabled(b_showcontent->isChecked());
	QHBox *h_msg = new QHBox(messagegrp);
	h_msg->setSpacing(2);
	
	/*QLabel *l_citesign = */new QLabel(tr("Number of quoted characters"),h_msg);

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
	/*QLabel *labelx = */new QLabel("x = ",hxybox);
	e_posx = new QSpinBox(-2048, 2048, 1, hxybox);
	e_posx->setValue(hintsposition.x());
	/*QLabel *labely = */new QLabel("y = ",hxybox);
	e_posy = new QSpinBox(-2048, 2048, 1, hxybox);
	e_posy->setValue(hintsposition.y());
	
	hintgrp->setEnabled(b_hint->isChecked());
	notifygrp->setEnabled(b_notify->isChecked());
	e_syntax->parentWidget()->setEnabled(b_syntax->isChecked());

	QHBox *hb_0 = new QHBox(hintgrp);
	hb_0->setSpacing(2);

	cb_notify = new QComboBox(hb_0);
	cb_notify->insertItem(tr("Set for all"));
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

	/*QLabel *l_timeout = */new QLabel(tr("Hint timeout"), hb_0);

	sb_timeout = new QSpinBox(1,600,1,hb_0);
	sb_timeout->setSuffix(" s");

	QHBox *hb_1 = new QHBox(hintgrp);
	hb_1->setSpacing(2);

	QHGroupBox *hgb_0 = new QHGroupBox(tr("Preview"),hb_1);
	hgb_0->setAlignment(Qt::AlignCenter);

	preview = new QLabel(tr("<b>Text</b> preview"),hgb_0);
	preview->setAlignment(Qt::AlignCenter);

	QVBox *vb_0 = new QVBox(hb_1);
	vb_0->setSpacing(2);
	QPushButton *pb_fontcolor = new QPushButton(tr("Change font color"),vb_0);
	QPushButton *pb_bgcolor = new QPushButton(tr("Change background color"),vb_0);
	QPushButton *pb_font = new QPushButton(tr("Change font"),vb_0);

	cb_left->insertItem(tr("Nothing"));
	cb_left->insertItem(tr("Open chat"));
	cb_left->insertItem(tr("Delete hint"));
	cb_left->insertItem(tr("Delete all hints"));
	cb_left->setCurrentItem(config_file.readNumEntry("Hints", "LeftButton"));
	
	cb_right->insertItem(tr("Nothing"));
	cb_right->insertItem(tr("Open chat"));
	cb_right->insertItem(tr("Delete hint"));
	cb_right->insertItem(tr("Delete all hints"));
	cb_right->setCurrentItem(config_file.readNumEntry("Hints", "RightButton"));
	
	cb_middle->insertItem(tr("Nothing"));
	cb_middle->insertItem(tr("Open chat"));
	cb_middle->insertItem(tr("Delete hint"));
	cb_middle->insertItem(tr("Delete all hints"));
	cb_middle->setCurrentItem(config_file.readNumEntry("Hints", "MiddleButton"));

/* Ustawiamy poczatkowe parametry podgladu dla ustawionej pozycji */
	activatedChanged(cb_notify->currentItem());

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
	kdebugf2();
}

void HintManagerSlots::onDestroyConfigDialog()
{
	kdebugf();

	kdebugm(KDEBUG_FUNCTION_START, "Hint 1 timeout: %s\n", hint[0][4].latin1());
	config_file.writeEntry("Hints", "NewHintUnder", combobox2->currentItem());
	config_file.writeEntry("Hints", "UseUserPosition", b_useposition->isChecked());
	config_file.writeEntry("Hints", "HintsPosition", QPoint(e_posx->value(),e_posy->value()));
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

	QComboBox* cb_left = ConfigDialog::getComboBox("Hints", "Left button");
	QComboBox* cb_right = ConfigDialog::getComboBox("Hints", "Right button");
	QComboBox* cb_middle = ConfigDialog::getComboBox("Hints", "Middle button");
	
	config_file.writeEntry("Hints", "LeftButton", cb_left->currentItem());
	config_file.writeEntry("Hints", "RightButton", cb_right->currentItem());
	config_file.writeEntry("Hints", "MiddleButton", cb_middle->currentItem());

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
	kdebugf2();
}

void HintManagerSlots::activatedChanged(int index)
{
	if (index != 0)
	{
		preview->setFont(QFont(hint[index-1][0],hint[index-1][1].toInt()));
		preview->setPaletteForegroundColor(QColor(hint[index-1][2]));
		preview->setPaletteBackgroundColor(QColor(hint[index-1][3]));
		sb_timeout->setValue(hint[index-1][4].toInt());
	}
}

void HintManagerSlots::changeFontColor()
{
	QColor color = QColorDialog::getColor(preview->paletteForegroundColor(), 0, tr("Color dialog"));
	if (color.isValid())
	{
		preview->setPaletteForegroundColor(color);
		if (cb_notify->currentItem() == 0)
			for (int i = 0; i < 12; i++)
				hint[i][2] = color.name();
		else
			hint[cb_notify->currentItem()-1][2] = color.name();
	}
}

void HintManagerSlots::changeBackgroundColor()
{
	QColor color = QColorDialog::getColor(preview->paletteBackgroundColor(), 0, tr("Color dialog"));
	if (color.isValid())
	{
		preview->setPaletteBackgroundColor(color);
		if (cb_notify->currentItem() == 0)
			for (int i = 0; i < 12; i++)
				hint[i][3] = color.name();
		else
			hint[cb_notify->currentItem()-1][3] = color.name();
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
		if (cb_notify->currentItem() == 0)
			for (int i = 0; i < 12; i++)
			{
				hint[i][0] = font2.family();
				hint[i][1] = QString::number(font2.pointSize());
			}
		else
		{
			hint[cb_notify->currentItem()-1][0] = font2.family();
			hint[cb_notify->currentItem()-1][1] = QString::number(font2.pointSize());
		}
	}
}

void HintManagerSlots::changeTimeout(int value)
{
	if (cb_notify->currentItem() == 0)
		for (int i = 0; i < 12; i++)
			hint[i][4] = QString::number(value);
	else
		hint[cb_notify->currentItem()-1][4] = QString::number(value);
}

HintManager *hintmanager = NULL;

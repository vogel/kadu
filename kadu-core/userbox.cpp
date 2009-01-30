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
/*
QImage *UserBox::backgroundImage = 0;

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
}*/

// CreateNotifier UserBox::createNotifier;


// 	showDescriptionAction = new ActionDescription(
// 		ActionDescription::TypeUserList, "descriptionsAction",
// 		this, SLOT(showDescriptionsActionActivated(QAction *, bool)),
// 		"ShowDescription", tr("Hide descriptions"),
// 		true, tr("Show descriptions")
// 	);
// 	connect(showDescriptionAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(setDescriptionsActionState()));

// 	setDescriptionsActionState();

// 	addCompareFunction("Status", tr("Statuses"), compareStatus);
// 	if (brokenStringCompare)
// 		addCompareFunction("AltNick", tr("Nicks, case insensitive"), compareAltNickCaseInsensitive);
// 	else
// 		addCompareFunction("AltNick", tr("Nicks"), compareAltNick);

// 	connect(&pending, SIGNAL(messageFromUserAdded(Contact)), this, SLOT(messageFromUserAdded(Contact)));
// 	connect(&pending, SIGNAL(messageFromUserDeleted(Contact)), this, SLOT(messageFromUserAdded(Contact)));

// void UserBox::showDescriptionsActionActivated(QActiogn *sender, bool toggle)
// {
// 	config_file.writeEntry("Look", "ShowDesc", !toggle);
// 	KaduListBoxPixmap::setShowDesc(!toggle);
// 	UserBox::refreshAllLater();
// 	setDescriptionsActionState();
// }

// void UserBox::setDescriptionsActionState()
// {
// 	foreach (KaduAction *action, showDescriptionAction->actions())
// 		action->setChecked(!KaduListBoxPixmap::ShowDesc);
// }

// void UserBox::messageFromUserAdded(Contact elem)
// {
// 	if (visibleUsers()->contains(UserListElement::fromContact(elem, AccountManager::instance()->defaultAccount())))
// 		refreshLater();
// }

// void UserBox::messageFromUserDeleted(Contact elem)
// {
// 	if (visibleUsers()->contains(UserListElement::fromContact(elem, AccountManager::instance()->defaultAccount())))
// 		refreshLater();
// }

// void UserBox::refreshBackground()
// {
	//hack:
// 	viewport()->setStyleSheet(QString("QWidget {background-color:%1}").arg(config_file.readColorEntry("Look","UserboxBgColor").name()));
// 	setStyleSheet(QString("QFrame {color:%1}").arg(config_file.readColorEntry("Look","UserboxFgColor").name()));
	
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
// }

// void UserBox::setColorsOrBackgrounds()
// {
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
// 	foreach(UserBox *userbox, UserBoxes)
// 		userbox->refreshBackground();
// }
/*
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
}*/

// bool UserBox::moveUpCompareFunction(const QString &id)
// {
// 	kdebugf();
// 	CmpFuncDesc d;
// 	int pos = 0;
// 	bool found = false;
// 	QList<CmpFuncDesc>::iterator c;
// 	for (c = comparer->CmpFunctions.begin(); c != comparer->CmpFunctions.end(); ++c)
// 	{
//  		if ((*c).id == id)
//  		{
//  			found = true;
//  			if (pos == 0)
//  				break;
//  			d = *c;
//  			--c;
//  			c = comparer->CmpFunctions.insert(c, d);
//  			c += 2;
//  			comparer->CmpFunctions.remove(c);
//  			refreshLater();
//  			break;
//  		}
//  		++pos;
// 	}
// 	kdebugf2();
// 	return found;
// }

// bool UserBox::moveDownCompareFunction(const QString &id)
// {
// 	kdebugf();
// 	CmpFuncDesc d;
// 	int pos = 0;
// 	int cnt = comparer->CmpFunctions.count();
// 	bool found = false;
// 	QList<CmpFuncDesc>::iterator c;
// 	for (c = comparer->CmpFunctions.begin(); c != comparer->CmpFunctions.end(); ++c)
// 	{
//  		if ((*c).id == id)
//  		{
//  			found = true;
//  			if (pos == cnt - 1)
//  				break;
//  			d = *c;
//  			++c;
//  			c = comparer->CmpFunctions.insert(c, d);
//  			c -= 2;
//  			comparer->CmpFunctions.remove(c);
//  			refreshLater();
//  			break;
//  		}
//  		++pos;
// 	}
// 	kdebugf2();
// 	return found;
// }

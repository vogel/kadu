/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtextedit.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "userlist.h"
#include "icons_manager.h"
#include "keys_manager.h"
#include "encryption.h"
#include "misc.h"
#include "message_box.h"

KeysManager::KeysManager(QDialog *parent, const char *name) : QHBox(parent, name),
	layoutHelper(new LayoutHelper()), lv_keys(0), e_key(0), pb_del(0), pb_on(0)
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Manage keys"));
	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	QVBox *left = new QVBox(this);
	left->setMargin(10);
	left->setSpacing(10);

	QLabel *l_icon = new QLabel(left);
	QWidget *blank=new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBox *center = new QVBox(this);
	center->setMargin(10);
	center->setSpacing(10);

	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager->loadIcon("ManageKeysWindowIcon"));
	l_info->setText(tr("This dialog box allows you to manage your keys."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)


        // our QListView
	lv_keys = new QListView(center);
	lv_keys->addColumn(tr("Contact"), 240);
	lv_keys->addColumn(tr("Uin"), 180);
	lv_keys->addColumn(tr("Encryption is on"), 184);
	lv_keys->setAllColumnsShowFocus(true);
	// end our QListView

	// our QVGroupBox
	QVGroupBox *vgb_key = new QVGroupBox(center);
	vgb_key->setTitle(tr("Key"));
	// end our QVGroupBox
	
	// our QTextEdit
	e_key = new QTextEdit(vgb_key);
	e_key->setTextFormat(Qt::LogText);
	// end our QTextEdit
                                                                                                                                                                        
	// buttons
	QHBox *bottom = new QHBox(center);
	QWidget *blank2 = new QWidget(bottom);
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	pb_del = new QPushButton(icons_manager->loadIconSet("DeleteKeyButton"), tr("Delete"), bottom);
	pb_del->setEnabled(false);
	pb_on = new QPushButton(icons_manager->loadIconSet("DecryptedChat"), tr("On"), bottom);
	pb_on->setEnabled(false);
	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons

	connect(pb_del, SIGNAL(clicked()), this, SLOT(removeKey()));
	connect(pb_on, SIGNAL(clicked()), this, SLOT(turnEncryption()));
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	connect(lv_keys, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
	connect(lv_keys, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(turnEncryption(QListViewItem *)));
	
	// refreshing (loading) QListView
	refreshKeysList();

	layoutHelper->addLabel(l_info);

 	loadGeometry(this, "General", "KeysManagerDialogGeometry", 0, 0, 680, 460);
	kdebugf2();
}

KeysManager::~KeysManager()
{
	kdebugf();
	saveGeometry(this, "General", "KeysManagerDialogGeometry");
	delete layoutHelper;
	delete lv_keys;
	delete e_key;
	delete pb_del;
	kdebugf2();
}

void KeysManager::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void KeysManager::resizeEvent(QResizeEvent * /*e*/)
{
	layoutHelper->resizeLabels();
}

void KeysManager::getKeysList(QStringList &uins)
{
	kdebugf();
	
	QDir dir(ggPath("keys/"), "*.pem", QDir::Name, QDir::Files);
	QStringList list = dir.entryList();
	QFile file;
	
	QString temp;
	QString GGUIN = QString::number(config_file.readNumEntry("General", "UIN"));
	
	CONST_FOREACH(fileName, list)
	{
		file.setName(ggPath("keys/").append(*fileName));
		if (*fileName!=QString("private.pem") && *fileName!=GGUIN+QString(".pem") && file.open(IO_ReadOnly))
		{
			temp=*fileName;
			uins << (temp.remove(QRegExp(".pem$")));
			file.close();
		}
	}
}

void KeysManager::turnContactEncryptionText(QString id, bool on)
{
	QListViewItem *item = lv_keys->findItem(id, 1);
	if(item)
	{
		item->setText(2, bool2text(on));
		if (lv_keys->selectedItem() == item)
			turnEncryptionBtn(on);
	}
}

void KeysManager::turnEncryption()
{

	turnEncryption(lv_keys->selectedItem());
}

void KeysManager::turnEncryption(QListViewItem* item)
{
	bool on = (item->text(2) == tr("Yes") ? false : true);

	if (lv_keys->selectedItem() == item)
		turnEncryptionBtn(on);

	UserListElements ules(userlist->byID("Gadu", item->text(1)));
	UserGroup group(ules);
	emit turnEncryption(&group, on);
}

void KeysManager::turnEncryptionBtn(bool on)
{
	if(on)
	{
		pb_on->setText(tr("Off"));
		pb_on->setIconSet(icons_manager->loadIcon("EncryptedChat"));
	}
	else
	{
		pb_on->setText(tr("On"));
		pb_on->setIconSet(icons_manager->loadIcon("DecryptedChat"));
	}
}

void KeysManager::refreshKeysList()
{
	// begin filling QListView
	QStringList uins;
	getKeysList(uins);

	UserListElement ule;
	bool ok;
	bool encrypt;
	
	// clearing list
	lv_keys->clear();
	
	CONST_FOREACH(strUin, uins)
	{
		UinType uin = (*strUin).toUInt(&ok);
		if (ok)
		{
			ule = userlist->byID("Gadu", QString::number(uin));
			if (ule.data("EncryptionEnabled").isValid())
				encrypt = ule.data("EncryptionEnabled").toString() == "true";
			else
				encrypt = config_file.readBoolEntry("Chat", "Encryption");

			new QListViewItem(lv_keys, (userlist->contains(ule) ? ule.altNick() : QString::null), ule.ID("Gadu"), bool2text(encrypt));
		}
	}
	// end filling QListView

	// forcing selectionChanged() becouse QListView doesn't emit signal
	selectionChanged();
}

void KeysManager::removeKey()
{
	kdebugf();
	if (MessageBox::ask(tr("Are you sure you want to delete the selected key?")))
	{
		QString id = lv_keys->selectedItem()->text(1);
		QString path = ggPath("keys/")+id+QString(".pem");
		QFile *file = new QFile(path);
		if (file->remove())
		{
			refreshKeysList();
			emit keyRemoved(userlist->byID("Gadu", id));
		}
		else
			MessageBox::msg(tr("Cannot remove key\nCheck if you have access to file \"%1\"").arg(path));
		delete file;
	}
	kdebugf2();
}

void KeysManager::selectionChanged()
{
	e_key->clear();
	if (lv_keys->selectedItem() != NULL)
	{
		pb_del->setEnabled(true);
		pb_on->setEnabled(true);
		turnEncryptionBtn(lv_keys->selectedItem()->text(2) == tr("Yes"));
		getKeyInfo();
	}
	else if (lv_keys->childCount() > 0)
		lv_keys->setSelected(lv_keys->firstChild(), true);
	else
	{
		pb_del->setEnabled(false);
		pb_on->setEnabled(false);
	}
}

void KeysManager::getKeyInfo()
{
	QFile *file = new QFile(ggPath("keys/")+(lv_keys->selectedItem()->text(1))+QString(".pem"));
	if (file->open(IO_ReadOnly))
	{
		e_key->append(file->readAll());
		file->close();
	}
	delete file;
}

QString KeysManager::bool2text(bool on)
{
	return (on ? tr("Yes") : tr("No"));
}

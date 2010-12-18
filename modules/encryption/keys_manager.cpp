/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>
#include <QTreeWidget>
#include <QtGui/QVBoxLayout>

#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "userlist.h"
#include "icons-manager.h"
#include "encryption.h"
#include "misc.h"
#include "message_box.h"

#include "keys_manager.h"

KeysManager::KeysManager(QDialog *parent)
	: QWidget(parent),
	lv_keys(0), e_key(0), pb_del(0), pb_on(0)
{
	kdebugf();

	setWindowTitle(tr("Manage keys"));
	setAttribute(Qt::WA_DeleteOnClose);

	// create main QLabel widgets (icon and app info)

	QWidget *left = new QWidget();

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(IconsManager::instance()->loadPixmap("ManageKeysWindowIcon"));

	QWidget *blank = new QWidget;
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->addWidget(l_icon);
	left_layout->addWidget(blank);
	left->setLayout(left_layout);

	QWidget *center = new QWidget;

	QLabel *l_info = new QLabel();

	l_info->setText(tr("This dialog box allows you to manage your keys."));
	l_info->setWordWrap(true);
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	// end create main QLabel widgets (icon and app info)


        // our QTreeWidget
	lv_keys = new QTreeWidget(center);

	QStringList headers;
	headers << tr("Contact") << tr("Uin") << tr("Encryption is on");

	lv_keys->setHeaderLabels(headers);
	lv_keys->setAllColumnsShowFocus(true);
	lv_keys->setIndentation(false);
	// end our QTreeWidget

	// our QGroupBox
	QGroupBox *vgb_key = new QGroupBox(tr("Key"), center);
	QVBoxLayout *keyLayout = new QVBoxLayout(vgb_key);
	// end our QVroupBox

	// our QTextEdit
	e_key = new QTextEdit(vgb_key);
	e_key->setReadOnly(true);
	keyLayout->addWidget(e_key);
//	e_key->setTextFormat(Qt::LogText);
	// end our QTextEdit

	// buttons
	QWidget *bottom = new QWidget;

	QWidget *blank2 = new QWidget;
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	pb_del = new QPushButton(IconsManager::instance()->loadIcon("DeleteKeyButton"), tr("Delete"), bottom);
	pb_del->setEnabled(false);
	pb_on = new QPushButton(IconsManager::instance()->loadIcon("DecryptedChat"), tr("On"), bottom);
	pb_on->setEnabled(false);
	QPushButton *pb_close = new QPushButton(IconsManager::instance()->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");

	QHBoxLayout *bottom_layout = new QHBoxLayout(bottom);
	bottom_layout->addWidget(blank2);
	bottom_layout->addWidget(pb_del);
	bottom_layout->addWidget(pb_on);
	bottom_layout->addWidget(pb_close);
	// end buttons

	QVBoxLayout *center_layout = new QVBoxLayout(center);
	center_layout->addWidget(l_info);
	center_layout->addWidget(lv_keys);
	center_layout->addWidget(vgb_key);
	center_layout->addWidget(bottom);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(left);
	layout->addWidget(center);

	connect(pb_del, SIGNAL(clicked()), this, SLOT(removeKey()));
	connect(pb_on, SIGNAL(clicked()), this, SLOT(turnEncryption()));
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	connect(lv_keys, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
	connect(lv_keys, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(turnEncryption(QTreeWidgetItem *)));

	// refreshing (loading) QTreeWidget
	refreshKeysList();

 	loadWindowGeometry(this, "General", "KeysManagerDialogGeometry", 0, 50, 680, 460);
	kdebugf2();
}

KeysManager::~KeysManager()
{
	kdebugf();
	saveWindowGeometry(this, "General", "KeysManagerDialogGeometry");
	delete lv_keys;
	delete e_key;
	delete pb_del;
	kdebugf2();
}

void KeysManager::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		event->accept();
		close();
	}
	else
		QWidget::keyPressEvent(event);
}

QTreeWidgetItem * KeysManager::getSelected()
{
	if (lv_keys->selectedItems().count())
		return lv_keys->selectedItems()[0];
	else
		return 0;
}

void KeysManager::getKeysList(QStringList &uins)
{
	kdebugf();

	QDir dir(profilePath("keys/"), "*.pem", QDir::Name, QDir::Files);
	QStringList list = dir.entryList();
	QFile file;

	QString temp;
	QString GGUIN = QString::number(config_file.readNumEntry("General", "UIN"));

	foreach(const QString &fileName, list)
	{
		file.setName(profilePath("keys/").append(fileName));
		if (fileName != QString("private.pem") && fileName != GGUIN + QString(".pem") && file.open(QIODevice::ReadOnly))
		{
			temp = fileName;
			uins << (temp.remove(QRegExp(".pem$")));
			file.close();
		}
	}
}

void KeysManager::turnContactEncryptionText(QString id, bool on)
{
	QList <QTreeWidgetItem *> items = lv_keys->findItems(id, Qt::MatchExactly, 1);
	if (items.count())
	{
		items[0]->setText(2, bool2text(on));
		if (getSelected() == items[0])
			turnEncryptionBtn(on);
	}
}

void KeysManager::turnEncryption()
{
	turnEncryption(getSelected());
}

void KeysManager::turnEncryption(QTreeWidgetItem *item)
{
	bool on = (item->text(2) == tr("Yes") ? false : true);

	if (getSelected() == item)
		turnEncryptionBtn(on);

	UserListElements ules(userlist->byID("Gadu", item->text(1)));
	UserGroup group(ules);
	emit turnEncryption(&group, on);
}

void KeysManager::turnEncryptionBtn(bool on)
{
	if (on)
	{
		pb_on->setText(tr("Off"));
		pb_on->setIcon(IconsManager::instance()->loadIcon("DecryptedChat_off"));
	}
	else
	{
		pb_on->setText(tr("On"));
		pb_on->setIcon(IconsManager::instance()->loadIcon("DecryptedChat"));
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

	foreach(const QString &strUin, uins)
	{
		UinType uin = strUin.toUInt(&ok);
		if (ok)
		{
			ule = userlist->byID("Gadu", QString::number(uin));
			if (ule.data("EncryptionEnabled").isValid())
				encrypt = ule.data("EncryptionEnabled").toString() == "true";
			else
				encrypt = config_file.readBoolEntry("Chat", "Encryption");

			QStringList strings;
			strings << (userlist->contains(ule) ? ule.altNick() : QString()) << ule.ID("Gadu") << bool2text(encrypt);

			new QTreeWidgetItem(lv_keys, strings);
		}
	}
	// end filling QListView

	// forcing selectionChanged() becouse QListView doesn't emit signal
	selectionChanged();
}

void KeysManager::removeKey()
{
	kdebugf();
	if (MessageDialog::ask(tr("Are you sure you want to delete the selected key?")))
	{
		QString id = getSelected()->text(1);
		QString path = profilePath("keys/")+id+QString(".pem");
		QFile *file = new QFile(path);
		if (file->remove())
		{
			refreshKeysList();
			emit keyRemoved(userlist->byID("Gadu", id));
		}
		else
			MessageDialog::msg(tr("Cannot remove key\nCheck if you have access to file \"%1\"").arg(path));

		delete file;
	}
	kdebugf2();
}

void KeysManager::selectionChanged()
{
	e_key->clear();
	if (lv_keys->selectedItems().count())
	{
		pb_del->setEnabled(true);
		pb_on->setEnabled(true);
		turnEncryptionBtn(lv_keys->selectedItems()[0]->text(2) == tr("Yes"));
		getKeyInfo();
	}
//	else if (lv_keys->childCount() > 0)
//		lv_keys->setSelected(lv_keys->firstChild(), true);
	else
	{
		pb_del->setEnabled(false);
		pb_on->setEnabled(false);
	}
}

void KeysManager::getKeyInfo()
{
	QFile *file = new QFile(profilePath("keys/") + (lv_keys->selectedItems()[0]->text(1)) + QString(".pem"));
	if (file->open(QIODevice::ReadOnly))
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

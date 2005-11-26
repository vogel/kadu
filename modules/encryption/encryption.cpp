/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qfile.h>
#include <qhgroupbox.h>
#include <qmessagebox.h>
#include <stdlib.h>

#include "action.h"
#include "chat.h"
#include "chat_manager.h"
#include "config_dialog.h"
#include "debug.h"
#include "encryption.h"
#include "gadu.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "userbox.h"

extern "C"
{
#include "simlite.h"
};

// uzywamy mkdir z sys/stat.h - nie ma w Qt mozliwosci ustawienia praw do kat.
#include <sys/stat.h>

EncryptionManager* encryption_manager;

extern "C" int encryption_init()
{
	encryption_manager=new EncryptionManager(NULL, "encryption_manager");
	return 0;
}

extern "C" void encryption_close()
{
	delete encryption_manager;
	encryption_manager=NULL;
}

EncryptionManager::EncryptionManager(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	ConfigDialog::addCheckBox("Chat", "Chat",
			QT_TRANSLATE_NOOP("@default", "Use encryption"), "Encryption", false);
	ConfigDialog::addHGroupBox("Chat", "Chat",
			QT_TRANSLATE_NOOP("@default", "Encryption properties"));
	ConfigDialog::addComboBox("Chat", "Encryption properties",
			QT_TRANSLATE_NOOP("@default", "Keys length"), "EncryptionKeyLength", QStringList("1024"), QStringList("1024"));
	ConfigDialog::addPushButton("Chat", "Encryption properties",
			QT_TRANSLATE_NOOP("@default", "Generate keys"));
	ConfigDialog::addColorButton("Chat", "Encryption properties",
			QT_TRANSLATE_NOOP("@default", "Color of encrypted messages"), "EncryptionColor", QColor("#0000FF"));

	ConfigDialog::registerSlotOnCreateTab("Chat", this,SLOT(createConfigDialogSlot()));
	ConfigDialog::connectSlot("Chat", "Generate keys", SIGNAL(clicked()), this, SLOT(generateMyKeys()));
	ConfigDialog::connectSlot("Chat", "Use encryption", SIGNAL(toggled(bool)), this, SLOT(onUseEncryption(bool)));

	connect(chat_manager, SIGNAL(chatCreated(const UserGroup *)), this, SLOT(chatCreated(const UserGroup *)));
	connect(gadu, SIGNAL(messageFiltering(Protocol *, UserListElements, QCString&, QByteArray&, bool&)),
			this, SLOT(receivedMessageFilter(Protocol *, UserListElements, QCString&, QByteArray&, bool&)));
	connect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(userBoxMenuPopup()));

	Action* action = new Action(icons_manager->loadIcon("EncryptedChat"),
		tr("Enable encryption for this conversation"), "encryptionAction");
	action->setDockAreaGroupRestriction("chatDockAreaGroup");
	connect(action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(encryptionActionActivated(const UserGroup*)));
	KaduActions.insert("encryptionAction", action);
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "encryptionAction", 4);

	UserBox::userboxmenu->addItemAtPos(2,"SendPublicKey", tr("Send my public key"), this, SLOT(sendPublicKey()));

	sim_key_path = strdup(ggPath("keys/").local8Bit());

	// uzywamy mkdir z sys/stat.h - nie ma w QT mozliwosci ustawienia praw do kat.
	mkdir(ggPath("keys").local8Bit().data(), 0700);

	kdebugf2();
}

EncryptionManager::~EncryptionManager()
{
	kdebugf();
	int sendkeyitem = UserBox::userboxmenu->getItem(tr("Send my public key"));
	UserBox::userboxmenu->removeItem(sendkeyitem);
	KaduActions.remove("encryptionAction");

	disconnect(chat_manager, SIGNAL(chatCreated(const UserGroup *)), this, SLOT(chatCreated(const UserGroup *)));
	disconnect(gadu, SIGNAL(messageFiltering(Protocol *, UserListElements, QCString&, QByteArray&, bool&)),
			this, SLOT(receivedMessageFilter(Protocol *, UserListElements, QCString&, QByteArray&, bool&)));
	disconnect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userBoxMenuPopup()));

	ConfigDialog::disconnectSlot("Chat", "Generate keys", SIGNAL(clicked()), this, SLOT(generateMyKeys()));
	ConfigDialog::disconnectSlot("Chat", "Use encryption", SIGNAL(toggled(bool)), this, SLOT(onUseEncryption(bool)));
	ConfigDialog::unregisterSlotOnCreateTab("Chat", this, SLOT(createConfigDialogSlot()));

	ConfigDialog::removeControl("Chat", "Color of encrypted messages");
	ConfigDialog::removeControl("Chat", "Generate keys");
	ConfigDialog::removeControl("Chat", "Keys length");
	ConfigDialog::removeControl("Chat", "Encryption properties");
	ConfigDialog::removeControl("Chat", "Use encryption");
	kdebugf2();
}

void EncryptionManager::createConfigDialogSlot()
{
	kdebugf();
	ConfigDialog::getHGroupBox("Chat", "Encryption properties")
			->setEnabled(ConfigDialog::getCheckBox("Chat", "Use encryption")->isChecked());
	kdebugf2();
}

void EncryptionManager::generateMyKeys()
{
	kdebugf();
	int myUin=config_file.readNumEntry("General","UIN");
	QString keyfile_path;
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(myUin));
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);

	if (keyfile.permission(QFileInfo::WriteUser))
		if(QMessageBox::warning(0, "Kadu",
			tr("Keys exist. Do you want to overwrite them?"),
			tr("Yes"), tr("No"),QString::null, 0, 1)==1)
				return;

	if (sim_key_generate(myUin) < 0)
	{
		QMessageBox::critical(0, "Kadu", tr("Error generating keys"), tr("OK"), QString::null, 0);
		return;
	}

	QMessageBox::information(0, "Kadu", tr("Keys have been generated and written"), tr("OK"), QString::null, 0);
	kdebugf2();
}

void EncryptionManager::onUseEncryption(bool toggled)
{
	ConfigDialog::getHGroupBox("Chat", "Encryption properties")->setEnabled(toggled);
}

void EncryptionManager::chatCreated(const UserGroup *group)
{
	kdebugf();
	QString keyfile_path;
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append((*(*group).constBegin()).ID("Gadu"));
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);
	bool encryption_possible =
		(keyfile.permission(QFileInfo::ReadUser) && group->count() == 1);

	Chat* chat = chat_manager->findChat(group);
	connect(chat, SIGNAL(messageFiltering(const UserGroup *, QCString &, bool &)),
			this, SLOT(sendMessageFilter(const UserGroup *, QCString &, bool &)));

	bool encrypt=false;
	if (encryption_possible)
	{
		QVariant v=chat_manager->getChatProperty(group, "EncryptionEnabled");
		if (v.isValid())
			encrypt=v.toBool();
		else
			encrypt=config_file.readBoolEntry("Chat", "Encryption");
	}

	setupEncryptButton(chat, encrypt);
	QValueList<ToolButton*> buttons =
		KaduActions["encryptionAction"]->toolButtonsForUserListElements(
			group->toUserListElements());
	for (QValueList<ToolButton*>::iterator i = buttons.begin(); i != buttons.end(); i++)
		(*i)->setEnabled(encryption_possible);

	kdebugf2();
}

void EncryptionManager::setupEncryptButton(Chat* chat,bool enabled)
{
	kdebugf();
	EncryptionEnabled[chat] = enabled;
	QValueList<ToolButton*> buttons =
		KaduActions["encryptionAction"]->toolButtonsForUserListElements(
			chat->users()->toUserListElements());
	for (QValueList<ToolButton*>::iterator i = buttons.begin(); i != buttons.end(); i++)
	{
		QToolTip::remove(*i);
		if (enabled)
		{
			QToolTip::add(*i, tr("Disable encryption for this conversation"));
			(*i)->setPixmap(icons_manager->loadIcon("EncryptedChat"));
		}
		else
		{
			QToolTip::add(*i, tr("Enable encryption for this conversation"));
			(*i)->setPixmap(icons_manager->loadIcon("DecryptedChat"));
		}
	}
	chat_manager->setChatProperty(chat->users(), "EncryptionEnabled", QVariant(enabled));
	kdebugf2();
}

void EncryptionManager::encryptionActionActivated(const UserGroup* users)
{
	kdebugf();
	Chat* chat= chat_manager->findChat(users);
	setupEncryptButton(chat,!EncryptionEnabled[chat]);
	kdebugf2();
}

void EncryptionManager::receivedMessageFilter(Protocol *protocol,
			UserListElements senders, QCString& msg, QByteArray& formats, bool& stop)
{
	kdebugf();
	if (!strncmp(msg, "-----BEGIN RSA PUBLIC KEY-----", 20))
	{
		(new SavePublicKey(senders[0], msg, NULL))->show();
		stop = true;
		kdebugf2();
		return;
	}

	kdebugm(KDEBUG_INFO, "Decrypting encrypted message...(%d)\n", msg.length());
	const char* msg_c = msg;
	char* decoded = sim_message_decrypt((const unsigned char*)msg_c, senders[0].ID(protocol->protocolID()).toUInt());
	kdebugm(KDEBUG_DUMP, "Decrypted message is(len:%d): %s\n", decoded ? strlen(decoded) : 0, decoded);
	if (decoded != NULL)
	{
		msg = decoded;
		free(decoded);

		gg_msg_richtext_format format;
		format.position = 0;
		format.font = GG_FONT_COLOR;
		gg_msg_richtext_color color;

		QColor new_color= config_file.readColorEntry("Chat", "EncryptionColor");
		color.red = new_color.red();
		color.green = new_color.green();
		color.blue = new_color.blue();

		QByteArray new_formats(
			formats.size()+sizeof(format)+sizeof(color));
		char* dst = new_formats.data();
		memcpy(dst, &format, sizeof(format));
		dst += sizeof(format);
		memcpy(dst, &color, sizeof(color));
		dst += sizeof(color);
		memcpy(dst, formats.data(), formats.size());
		formats = new_formats;
	}
	kdebugf2();
}

void EncryptionManager::enableEncryptionBtnForUsers(UserListElements users)
{
	kdebugf();
	QValueList<ToolButton*> buttons =
		KaduActions["encryptionAction"]->toolButtonsForUserListElements(users);
	for (QValueList<ToolButton*>::iterator i = buttons.begin(); i != buttons.end(); i++)
		(*i)->setEnabled(true);
	kdebugf2();
}

void EncryptionManager::sendMessageFilter(const UserGroup *users, QCString &msg, bool &stop)
{
	Chat* chat = chat_manager->findChat(users);
//	kdebugm(KDEBUG_INFO, "length: %d\n", msg.length());
	if (users->count() == 1 && EncryptionEnabled[chat])
	{
		char *msg_c = sim_message_encrypt((const unsigned char*)(const char*)msg, (*(*users).constBegin()).ID("Gadu").toUInt());
		if (msg_c == NULL)
		{
			kdebugm(KDEBUG_ERROR, "sim_message_encrypt returned NULL! sim_errno=%d sim_strerror=%s\n", sim_errno, sim_strerror(sim_errno));
			stop = true;
			MessageBox::wrn(tr("Cannot encrypt message. sim_message_encrypt returned: \"%1\" (sim_errno=%2)").arg(sim_strerror(sim_errno)).arg(sim_errno), true);
		}
		else
		{
			msg = msg_c;
			free(msg_c);
		}
	}
//	kdebugm(KDEBUG_INFO, "length: %d\n", msg.length());
}

void EncryptionManager::userBoxMenuPopup()
{
	kdebugf();
	int sendkeyitem = UserBox::userboxmenu->getItem(tr("Send my public key"));

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
		return;

	QString keyfile_path;
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(config_file.readNumEntry("General", "UIN")));
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);

	UserListElements users = activeUserBox->selectedUsers();
	UserListElement user = users[0];

	bool sendKeyEnabled = user.usesProtocol("Gadu") &&
			keyfile.permission(QFileInfo::ReadUser) &&
			(users.count() == 1) &&
			!gadu->currentStatus().isOffline() &&
			config_file.readUnsignedNumEntry("General", "UIN") != user.ID("Gadu").toUInt();

	UserBox::userboxmenu->setItemEnabled(sendkeyitem, sendKeyEnabled);
	kdebugf2();
}

void EncryptionManager::sendPublicKey()
{
	kdebugf();
	QString keyfile_path;
	QString mykey;
	QFile keyfile;

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
		return;

	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(config_file.readEntry("General", "UIN"));
	keyfile_path.append(".pem");

	keyfile.setName(keyfile_path);

	if (keyfile.open(IO_ReadOnly))
	{
		QTextStream t(&keyfile);
		mykey = t.read();
		keyfile.close();
		QCString tmp(mykey.local8Bit());
		UserListElements users(activeUserBox->selectedUsers()[0]);
		gadu->sendMessage(users, tmp.data());
		QMessageBox::information(kadu, "Kadu",
			tr("Your public key has been sent"), tr("OK"), QString::null, 0);
	}
	kdebugf2();
}

SavePublicKey::SavePublicKey(UserListElement user, QString keyData, QWidget *parent, const char *name) :
	QDialog(parent, name, Qt::WDestructiveClose), user(user), keyData(keyData)
{
	kdebugf();

	setCaption(tr("Save public key"));
	resize(200, 80);

	QLabel *l_info;
	QString text = tr("User %1 is sending you his public key. Do you want to save it?").arg(user.altNick());

	l_info = new QLabel(text, this);

	QPushButton *yesbtn = new QPushButton(tr("Yes"), this);
	QPushButton *nobtn = new QPushButton(tr("No"), this);

	QObject::connect(yesbtn, SIGNAL(clicked()), this, SLOT(yesClicked()));
	QObject::connect(nobtn, SIGNAL(clicked()), this, SLOT(reject()));

	QGridLayout *grid = new QGridLayout(this, 2, 2, 3, 3);
	grid->addMultiCellWidget(l_info, 0, 0, 0, 1);
	grid->addWidget(yesbtn, 1, 0);
	grid->addWidget(nobtn, 1, 1);

	kdebugf2();
}

void SavePublicKey::yesClicked()
{
	kdebugf();

	QFile keyfile;
	QString keyfile_path;

	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(user.ID("Gadu"));
	keyfile_path.append(".pem");
	keyfile.setName(keyfile_path);

	if (!(keyfile.open(IO_WriteOnly)))
	{
		QMessageBox::critical(this, tr("Error"), tr("Error writting the key"), tr("OK"), QString::null, 0);
		kdebugmf(KDEBUG_ERROR, "Error opening key file %s\n", (const char *)keyfile_path.local8Bit());
		return;
	}
	else
	{
		keyfile.writeBlock(keyData.local8Bit(), keyData.length());
		keyfile.close();
		UserListElements users(user);
		encryption_manager->enableEncryptionBtnForUsers(users);
	}
	accept();

	kdebugf2();
}

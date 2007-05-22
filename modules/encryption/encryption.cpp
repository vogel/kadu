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
#include <qpushbutton.h>
#include <qtooltip.h>
#include <stdlib.h>

#include "action.h"
#include "chat.h"
#include "chat_manager.h"
// #include "config_dialog.h"
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

// for mkdir
#include <sys/stat.h>

/**
 * @ingroup encryption
 * @{
 */

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

EncryptionManager::EncryptionManager(QObject *parent, const char *name) : QObject(parent, name),
	EncryptionEnabled()
{
	kdebugf();
// 	ConfigDialog::addVGroupBox("Chat", "Chat",
// 			QT_TRANSLATE_NOOP("@default", "Encryption properties"));
// 	ConfigDialog::addCheckBox("Chat", "Encryption properties",
// 			QT_TRANSLATE_NOOP("@default", "Encrypt by default"), "Encryption", false);
// 	ConfigDialog::addCheckBox("Chat", "Encryption properties",
// 			QT_TRANSLATE_NOOP("@default", "Encrypt after receive encrypted message"), "EncryptAfterReceiveEncryptedMessage", false);
// 	ConfigDialog::addHBox("Chat", "Encryption properties", "key_generator");
// 	ConfigDialog::addComboBox("Chat", "key_generator",
// 			QT_TRANSLATE_NOOP("@default", "Keys length"), "EncryptionKeyLength", QStringList("1024"), QStringList("1024"), "1024");
// 	ConfigDialog::addPushButton("Chat", "key_generator",
// 			QT_TRANSLATE_NOOP("@default", "Generate keys"));

// 	ConfigDialog::addColorButton("Look", "Chat window",
// 			QT_TRANSLATE_NOOP("@default", "Color of encrypted messages"), "EncryptionColor", QColor("#0000FF"));

// 	ConfigDialog::connectSlot("Chat", "Generate keys", SIGNAL(clicked()), this, SLOT(generateMyKeys()));

	userlist->addPerContactNonProtocolConfigEntry("encryption_enabled", "EncryptionEnabled");

	connect(chat_manager, SIGNAL(chatCreated(Chat *)), this, SLOT(chatCreated(Chat *)));
	connect(gadu, SIGNAL(messageFiltering(Protocol *, UserListElements, QCString&, QByteArray&, bool&)),
			this, SLOT(receivedMessageFilter(Protocol *, UserListElements, QCString&, QByteArray&, bool&)));
	connect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(userBoxMenuPopup()));

	action = new Action(icons_manager->loadIcon("EncryptedChat"),
		tr("Enable encryption for this conversation"), "encryptionAction", Action::TypeChat);
	action->setToggleAction(true);
	connect(action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(encryptionActionActivated(const UserGroup*)));
	connect(action, SIGNAL(addedToToolbar(const UserGroup*, ToolButton*, ToolBar*)),
		this, SLOT(setupEncrypt(const UserGroup*)));
	KaduActions.insert("encryptionAction", action);
	KaduActions.addDefaultToolbarAction("Chat toolbar 1", "encryptionAction", 4);

	UserBox::userboxmenu->addItemAtPos(2,"SendPublicKey", tr("Send my public key"), this, SLOT(sendPublicKey()));

	sim_key_path = strdup(ggPath("keys/").local8Bit());

	// use mkdir from sys/stat.h - there's no easy way to set permissions through Qt
	mkdir(ggPath("keys").local8Bit().data(), 0700);

	kdebugf2();
}

EncryptionManager::~EncryptionManager()
{
	kdebugf();
	int sendkeyitem = UserBox::userboxmenu->getItem(tr("Send my public key"));
	UserBox::userboxmenu->removeItem(sendkeyitem);
	KaduActions.remove("encryptionAction");

	disconnect(chat_manager, SIGNAL(chatCreated(Chat *)), this, SLOT(chatCreated(Chat *)));
	disconnect(gadu, SIGNAL(messageFiltering(Protocol *, UserListElements, QCString&, QByteArray&, bool&)),
			this, SLOT(receivedMessageFilter(Protocol *, UserListElements, QCString&, QByteArray&, bool&)));
	disconnect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userBoxMenuPopup()));

// 	ConfigDialog::disconnectSlot("Chat", "Generate keys", SIGNAL(clicked()), this, SLOT(generateMyKeys()));

// 	ConfigDialog::removeControl("Look", "Color of encrypted messages");
// 	ConfigDialog::removeControl("Chat", "Generate keys");
// 	ConfigDialog::removeControl("Chat", "Keys length");
// 	ConfigDialog::removeControl("Chat", "key_generator");
// 	ConfigDialog::removeControl("Chat", "Encrypt after receive encrypted message");
// 	ConfigDialog::removeControl("Chat", "Encrypt by default");
// 	ConfigDialog::removeControl("Chat", "Encryption properties");
	delete action;
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
		if(MessageBox::ask(tr("Keys exist. Do you want to overwrite them?"), false, "Warning")
				return;

	if (sim_key_generate(myUin) < 0)
	{
		MessageBox::msg(tr("Error generating keys"), false, "Warning");
		return;
	}

	MessageBox::msg(tr("Keys have been generated and written"), false, "NotifyTab");
	kdebugf2();
}

void EncryptionManager::chatCreated(Chat *chat)
{
	connect(chat, SIGNAL(messageFiltering(const UserGroup *, QCString &, bool &)),
			this, SLOT(sendMessageFilter(const UserGroup *, QCString &, bool &)));
}

void EncryptionManager::setupEncrypt(const UserGroup *group)
{
	kdebugf();
	QString keyfile_path;
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append((*(*group).constBegin()).ID("Gadu"));
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);
	bool encryption_possible =
		(keyfile.permission(QFileInfo::ReadUser) && group->count() == 1);

	bool encrypt = false;
	if (encryption_possible)
	{
		QVariant v = chat_manager->getChatProperty(group, "EncryptionEnabled");
		if (v.isValid())
			encrypt = v.toBool();
		else if ((*(group->constBegin())).data("EncryptionEnabled").isValid())
			encrypt = (*(group->constBegin())).data("EncryptionEnabled").toString() == "true";
		else
			encrypt = config_file.readBoolEntry("Chat", "Encryption");
	}

	setupEncryptButton(chat_manager->findChat(group), encrypt);
	QValueList<ToolButton*> buttons =
		KaduActions["encryptionAction"]->toolButtonsForUserListElements(group->toUserListElements());
	CONST_FOREACH(i, buttons)
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
	CONST_FOREACH(i, buttons)
	{
		QToolTip::remove(*i);
		if (enabled)
		{
			QToolTip::add(*i, tr("Disable encryption for this conversation"));
			(*i)->setPixmap(icons_manager->loadIcon("EncryptedChat"));
			(*i)->setOn(true);
		}
		else
		{
			QToolTip::add(*i, tr("Enable encryption for this conversation"));
			(*i)->setPixmap(icons_manager->loadIcon("DecryptedChat"));
			(*i)->setOn(false);
		}
	}
	chat_manager->setChatProperty(chat->users(), "EncryptionEnabled", QVariant(enabled));
	if (chat->users()->count() == 1)
		(*(chat->users()->begin())).setData("EncryptionEnabled", enabled ? "true" : "false");
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
	if (msg.length() < 30)
	{
		kdebugf2();
		return;
	}
	if (!strncmp(msg, "-----BEGIN RSA PUBLIC KEY-----", 30))
	{
		(new SavePublicKey(senders[0], msg, NULL))->show();
		stop = true;
		kdebugf2();
		return;
	}

	kdebugm(KDEBUG_INFO, "Decrypting encrypted message...(%d)\n", msg.length());
	const char* msg_c = msg;
	char* decoded = sim_message_decrypt((const unsigned char*)msg_c, senders[0].ID(protocol->protocolID()).toUInt());
	kdebugm(KDEBUG_DUMP, "Decrypted message is(len:%u): %s\n", decoded ? strlen(decoded) : 0, decoded);
	if (decoded != NULL)
	{
		msg = decoded;
		free(decoded);

		gg_msg_richtext_format format;
		format.position = 0;
		format.font = GG_FONT_COLOR;
		gg_msg_richtext_color color;

		QColor new_color= config_file.readColorEntry("Look", "EncryptionColor");
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

		Chat* chat=chat_manager->findChat(senders);
		if (config_file.readBoolEntry("Chat", "EncryptAfterReceiveEncryptedMessage"))
		{
			if (chat)
				setupEncryptButton(chat_manager->findChat(senders), true);
			else
			{
				UserGroup userGroup(senders);
				chat_manager->setChatProperty(&userGroup, "EncryptionEnabled", QVariant(true));
				senders[0].setData("EncryptionEnabled", "true");
			}
		}
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
			MessageBox::msg(tr("Cannot encrypt message. sim_message_encrypt returned: \"%1\" (sim_errno=%2)").arg(sim_strerror(sim_errno)).arg(sim_errno), true, "Warning");
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

	bool sendKeyEnabled = true;
	if (keyfile.permission(QFileInfo::ReadUser) && !gadu->currentStatus().isOffline())
	{
		unsigned int myUin = config_file.readUnsignedNumEntry("General", "UIN");
		UserListElements users = activeUserBox->selectedUsers();

		CONST_FOREACH(user, users)
			if (!(*user).usesProtocol("Gadu") || (*user).ID("Gadu").toUInt() == myUin)
			{
				sendKeyEnabled = false;
				break;
			}
	}
	else
		sendKeyEnabled = false;

	UserBox::userboxmenu->setItemVisible(sendkeyitem, sendKeyEnabled);
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

		UserListElements users = activeUserBox->selectedUsers();
		CONST_FOREACH(user, users)
			gadu->sendMessage(*user, tmp.data());

		MessageBox::msg(tr("Your public key has been sent"), false, "NotifyTab", kadu);
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
		MessageBox::msg(tr("Error writting the key"), false, "Warning", this);
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

/** @} */


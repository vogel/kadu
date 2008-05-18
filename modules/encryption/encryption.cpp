/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QCheckBox>
#include <QFile>
#include <QPushButton>
#include <QToolTip>
#include <QList>
#include <QLabel>
#include <QGridLayout>
#include <stdlib.h>

#include "action.h"
#include "chat_widget.h"
#include "chat_manager.h"
#include "debug.h"
#include "encryption.h"
#include "keys_manager.h"
#include "gadu.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "toolbar.h"
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
	encryption_manager = new EncryptionManager();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/encryption.ui"), encryption_manager);

	return 0;
}

extern "C" void encryption_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/encryption.ui"), encryption_manager);

	delete encryption_manager;
	encryption_manager = 0;
}

EncryptionManager::EncryptionManager()
	: MenuId(0), EncryptionEnabled(), EncryptionPossible(), KeysManagerDialog(0)
{
	kdebugf();

	createDefaultConfiguration();

	userlist->addPerContactNonProtocolConfigEntry("encryption_enabled", "EncryptionEnabled");

	connect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QCString&, QByteArray&, bool&)),
			this, SLOT(decryptMessage(Protocol *, UserListElements, QCString&, QByteArray&, bool&)));
	connect(gadu, SIGNAL(sendMessageFiltering(const UserListElements, QString &, bool &)),
			this, SLOT(sendMessageFilter(const UserListElements, QString &, bool &)));
	connect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(userBoxMenuPopup()));

	action = new ActionDescription(
				ActionDescription::TypeChat, "encryptionAction",
				this, SLOT(encryptionActionActivated(QAction *, const UserGroup*)),
				"EncrypteChat", tr("Enable encryption for this conversation"),
				true, tr("Disable encryption for this conversation"));
	
	connect(action, SIGNAL(addedToToolbar(const UserGroup*, ToolButton*, ToolBar*)),
		this, SLOT(setupEncrypt(const UserGroup*)));
	ToolBar::addDefaultAction("Chat toolbar 1", "encryptionAction", 4);

	UserBox::userboxmenu->addItemAtPos(2,"SendPublicKey", tr("Send my public key"), this, SLOT(sendPublicKey()));
	
	MenuId = kadu->mainMenu()->insertItem(icons_manager->loadIcon("KeysManager"), tr("Manage keys"), this, SLOT(showKeysManagerDialog()), 0, -1, 12);
	//icons_manager->registerMenuItem(kadu->mainMenu(), tr("Manage keys"), "KeysManager");

	sim_key_path = strdup(ggPath("keys/").local8Bit());

	// use mkdir from sys/stat.h - there's no easy way to set permissions through Qt
	mkdir(ggPath("keys").local8Bit().data(), 0700);

	kdebugf2();
}

EncryptionManager::~EncryptionManager()
{
	kdebugf();
	kadu->mainMenu()->removeItem(MenuId);
	int sendkeyitem = UserBox::userboxmenu->getItem(tr("Send my public key"));
	UserBox::userboxmenu->removeItem(sendkeyitem);

	disconnect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QCString&, QByteArray&, bool&)),
			this, SLOT(decryptMessage(Protocol *, UserListElements, QCString&, QByteArray&, bool&)));
	disconnect(gadu, SIGNAL(sendMessageFiltering(const UserListElements, QString &, bool &)),
			this, SLOT(sendMessageFilter(const UserListElements, QString &, bool &)));
	disconnect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userBoxMenuPopup()));

	delete action;
	action = 0;
	kdebugf2();
}

void EncryptionManager::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widgetById("encryption/generateKeys"), SIGNAL(clicked()), this, SLOT(generateMyKeys()));

	configurationWindow = mainConfigurationWindow;
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

	if (keyfile.permission(QFileInfo::WriteUser) && !MessageBox::ask(tr("Keys exist. Do you want to overwrite them?"), "Warning", configurationWindow))
		return;

	if (sim_key_generate(myUin) < 0)
	{
		MessageBox::msg(tr("Error generating keys"), false, "Warning", configurationWindow);
		return;
	}

	MessageBox::msg(tr("Keys have been generated and written"), false, "Information", configurationWindow);
	kdebugf2();
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
		QVariant v = chat_manager->getChatWidgetProperty(group, "EncryptionEnabled");
		if (v.isValid())
			encrypt = v.toBool();
		else if ((*(group->constBegin())).data("EncryptionEnabled").isValid())
			encrypt = (*(group->constBegin())).data("EncryptionEnabled").toString() == "true";
		else
			encrypt = config_file.readBoolEntry("Chat", "Encryption");
	}

	ChatWidget *chat = chat_manager->findChatWidget(group);
	setupEncryptButton(chat, encrypt);
	setupEncryptionButtonForUsers(group->toUserListElements(), encryption_possible);
	EncryptionPossible[chat] = encryption_possible;

	kdebugf2();
}

void EncryptionManager::setupEncryptButton(ChatWidget* chat,bool enabled)
{
	kdebugf();
	//TODO
	/*EncryptionEnabled[chat] = enabled;
	QList<ToolButton*> buttons =
		KaduActions["encryptionAction"]->toolButtonsForUserListElements(
			chat->users()->toUserListElements());
	CONST_FOREACH(i, buttons)
	{
		QToolTip::remove(*i);
		if (enabled)
		{
			QToolTip::add(*i, tr("Disable encryption for this conversation"));
			(*i)->setIconSet(icons_manager->loadIconSet("EncryptedChat"));
			(*i)->setOn(true);
		}
		else
		{
			QToolTip::add(*i, tr("Enable encryption for this conversation"));
			(*i)->setIconSet(icons_manager->loadIconSet("DecryptedChat"));
			(*i)->setOn(false);
		}
	}
	chat_manager->setChatWidgetProperty(chat->users(), "EncryptionEnabled", QVariant(enabled));
	if (chat->users()->count() == 1)
		(*(chat->users()->begin())).setData("EncryptionEnabled", enabled ? "true" : "false");*/
	kdebugf2();
}

void EncryptionManager::encryptionActionActivated(QAction *action, const UserGroup* users)
{
	kdebugf();
	ChatWidget* chat= chat_manager->findChatWidget(users);
	setupEncryptButton(chat,!EncryptionEnabled[chat]);
	if (KeysManagerDialog!=NULL)
		KeysManagerDialog->turnContactEncryptionText((*(*users).constBegin()).ID("Gadu"), EncryptionEnabled[chat]);
	kdebugf2();
}

void EncryptionManager::decryptMessage(Protocol *protocol, UserListElements senders, QString &msg, QByteArray &formats, bool &ignore)
{
	kdebugf();
	if (msg.length() < 30)
	{
		kdebugf2();
		return;
	}
	if (!strncmp(msg, "-----BEGIN RSA PUBLIC KEY-----", 30))
	{
		SavePublicKey *save_public_key = new SavePublicKey(senders[0], msg, 0);
		save_public_key->show();
		connect(save_public_key, SIGNAL(keyAdded(UserListElement)), this, SLOT(keyAdded(UserListElement)));

		ignore = true;
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

		// FIXME: remove
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

		if (config_file.readBoolEntry("Chat", "EncryptAfterReceiveEncryptedMessage"))
		{
			ChatWidget *chat = chat_manager->findChatWidget(senders);
			if (!chat || EncryptionPossible[chat])
			{
				UserGroup group(senders);
				turnEncryption(&group, true);
			}
		}
	}
	kdebugf2();
}

void EncryptionManager::setupEncryptionButtonForUsers(UserListElements users, bool enabled)
{
	kdebugf();
	//TODO
	/*QList<ToolButton*> buttons =
		KaduActions["encryptionAction"]->toolButtonsForUserListElements(users);
	CONST_FOREACH(i, buttons)
		(*i)->setEnabled(enabled);*/
	kdebugf2();
}

void EncryptionManager::turnEncryption(UserGroup *group, bool on)
{
	ChatWidget *chat = chat_manager->findChatWidget(group->toUserListElements());
	if (chat)
		setupEncryptButton(chat, on);
	else
	{
		chat_manager->setChatWidgetProperty(group, "EncryptionEnabled", QVariant(on));
		(*(group->begin())).setData("EncryptionEnabled", (on ? "true" : "false"));
	}
	if (KeysManagerDialog!=NULL)
		KeysManagerDialog->turnContactEncryptionText((*(*group).constBegin()).ID("Gadu"), on);
}

void EncryptionManager::sendMessageFilter(const UserListElements users, QString &msg, bool &stop)
{

	ChatWidget* chat = chat_manager->findChatWidget(users);
//	kdebugm(KDEBUG_INFO, "length: %d\n", msg.length());
	if (users.count() == 1 && EncryptionEnabled[chat])
	{
		char *msg_c = sim_message_encrypt((const unsigned char *)msg.data(), (*users.constBegin()).ID("Gadu").toUInt());
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
		UserListElements users = activeUserBox->selectedUsers();
		CONST_FOREACH(user, users)
			gadu->sendMessage(*user, mykey);

		MessageBox::msg(tr("Your public key has been sent"), false, "Information", kadu);
	}
	kdebugf2();
}

void EncryptionManager::createDefaultConfiguration()
{
	config_file.addVariable("Chat", "Encryption", true);
	config_file.addVariable("Chat", "EncryptAfterReceiveEncryptedMessage", true);
	config_file.addVariable("Look", "EncryptionColor", QColor(0, 127, 0));
}

void EncryptionManager::showKeysManagerDialog()
{
	kdebugf();
	if (KeysManagerDialog==NULL)
	{
		KeysManagerDialog=new KeysManager();
		connect(KeysManagerDialog,SIGNAL(destroyed()),this,SLOT(keysManagerDialogDestroyed()));
		connect(KeysManagerDialog,SIGNAL(keyRemoved(UserListElement)),this,SLOT(keyRemoved(UserListElement)));
		connect(KeysManagerDialog,SIGNAL(turnEncryption(UserGroup*, bool)),this,SLOT(turnEncryption(UserGroup*, bool)));
		KeysManagerDialog->show();
	}
	else
	{
		KeysManagerDialog->setActiveWindow();
		KeysManagerDialog->raise();
	}
	kdebugf2();
}

void EncryptionManager::keysManagerDialogDestroyed()
{
	kdebugf();
	KeysManagerDialog=NULL;
	kdebugf2();
}

void EncryptionManager::keyRemoved(UserListElement ule)
{
	UserListElements ules(ule);
	ChatWidget *chat = chat_manager->findChatWidget(ules);
	if (chat)
	{
		EncryptionPossible[chat] = false;
		setupEncryptButton(chat, false);
		setupEncryptionButtonForUsers(ules, false);
	}
}

void EncryptionManager::keyAdded(UserListElement ule)
{
	UserListElements ules(ule);
	EncryptionPossible[chat_manager->findChatWidget(ules)] = true;
	setupEncryptionButtonForUsers(ule, true);
	if (KeysManagerDialog)
		KeysManagerDialog->refreshKeysList();
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
	this->setLayout(grid);

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
		emit keyAdded(user);
	}
	accept();

	kdebugf2();
}

/** @} */


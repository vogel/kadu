/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

#include "action.h"
#include "chat_widget.h"
#include "chat_manager.h"
#include "chat_edit_box.h"
#include "config_file.h"
#include "debug.h"
#include "keys_manager.h"
#include "gadu.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "toolbar.h"
#include "userbox.h"

#include "encryption.h"

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

extern "C" int encryption_init(bool firstLoad)
{
	encryption_manager = new EncryptionManager(firstLoad);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/encryption.ui"), encryption_manager);

	return 0;
}

extern "C" void encryption_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/encryption.ui"), encryption_manager);

	delete encryption_manager;
	encryption_manager = 0;
}

bool disableSendKey(KaduAction *action)
{
	kdebugf();

	UserListElements ules=action->userListElements();

	if (!ules.count())
		return false;

	QString keyfile_path;
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(config_file.readNumEntry("General", "UIN")));
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);

	if (!keyfile.permission(QFileInfo::ReadUser) || gadu->currentStatus().isOffline())
		return false;

	unsigned int myUin = config_file.readUnsignedNumEntry("General", "UIN");

	foreach(const UserListElement &user, ules)
		if (!user.usesProtocol("Gadu") || user.ID("Gadu").toUInt() == myUin)
			return false;

	return true;
}

EncryptionManager::EncryptionManager(bool firstLoad)
	: MenuId(0), EncryptionEnabled(), EncryptionPossible(), KeysManagerDialog(0)
{
	kdebugf();

	createDefaultConfiguration();

	userlist->addPerContactNonProtocolConfigEntry("encryption_enabled", "EncryptionEnabled");

	connect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QString&, QByteArray&, bool&)),
			this, SLOT(decryptMessage(Protocol *, UserListElements, QString&, QByteArray&, bool&)));
	connect(gadu, SIGNAL(sendMessageFiltering(const UserListElements, QString &, bool &)),
			this, SLOT(sendMessageFilter(const UserListElements, QString &, bool &)));

	encryptionActionDescription = new ActionDescription(
                	ActionDescription::TypeChat, "encryptionAction",
			this, SLOT(encryptionActionActivated(QAction *, bool)),
			"DecryptedChat", tr("Enable encryption for this conversation"),
			true, tr("Disable encryption for this conversation"),
			disableSendKey
	);

	if (firstLoad)
		ChatEditBox::addAction("encryptionAction");

	sendPublicKeyActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "sendPublicKeyAction",
		this, SLOT(sendPublicKeyActionActivated(QAction *, bool)),
		"SendPublicKey", tr("Send my public key"), false, QString::null,
		disableSendKey
	);
	UserBox::insertActionDescription(2, sendPublicKeyActionDescription);

	keysManagerActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "keysManagerAction",
		this, SLOT(showKeysManagerDialog(QAction *, bool)),
		"KeysManager", tr("Manage keys")
	);
	kadu->insertMenuActionDescription(12, keysManagerActionDescription);

	sim_key_path = strdup(qPrintable(ggPath("keys/")));

	// use mkdir from sys/stat.h - there's no easy way to set permissions through Qt
	mkdir(qPrintable(ggPath("keys")), 0700);

	kdebugf2();
}

EncryptionManager::~EncryptionManager()
{
	kdebugf();

	kadu->removeMenuActionDescription(keysManagerActionDescription);
	delete keysManagerActionDescription;

	disconnect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QString&, QByteArray&, bool&)),
			this, SLOT(decryptMessage(Protocol *, UserListElements, QString&, QByteArray&, bool&)));
	disconnect(gadu, SIGNAL(sendMessageFiltering(const UserListElements, QString &, bool &)),
			this, SLOT(sendMessageFilter(const UserListElements, QString &, bool &)));

	delete encryptionActionDescription;
	delete sendPublicKeyActionDescription;

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
		QVariant v = chat_manager->chatWidgetProperty(group, "EncryptionEnabled");
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

void EncryptionManager::setupEncryptButton(ChatWidget* chat, bool enabled)
{
	kdebugf();
/* TODO
	EncryptionEnabled[chat] = enabled;
	QList<ToolButton*> buttons =
		KaduActions["encryptionAction"]->toolButtonsForUserListElements(
			chat->users()->toUserListElements());
	CONST_FOREACH(i, buttons)
	{
		if (enabled)
		{
			(*i)->setToolTip(tr("Disable encryption for this conversation"));
			(*i)->setIconSet(icons_manager->loadIconSet("EncryptedChat"));
			(*i)->setOn(true);
		}
		else
		{
			(*i)->setToolTip(tr("Enable encryption for this conversation"));
			(*i)->setIconSet(icons_manager->loadIconSet("DecryptedChat"));
			(*i)->setOn(false);
		}
	}
*/
	chat_manager->setChatWidgetProperty(chat->users(), "EncryptionEnabled", QVariant(enabled));
	if (chat->users()->count() == 1)
		(*(chat->users()->begin())).setData("EncryptionEnabled", enabled ? "true" : "false");
	kdebugf2();
}

void EncryptionManager::encryptionActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *kaduMainWindow = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!kaduMainWindow)
		return;

	ChatWidget *chatWidget = chat_manager->findChatWidget(kaduMainWindow->userListElements());
	if (!chatWidget)
		return;
	setupEncryptButton(chatWidget,!EncryptionEnabled[chatWidget]);
	if (KeysManagerDialog)
		KeysManagerDialog->turnContactEncryptionText((*chatWidget->users()->constBegin()).ID("Gadu"), EncryptionEnabled[chatWidget]);
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
			formats.size() + sizeof(format) + sizeof(color));
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
/* TODO
	QList<ToolButton*> buttons = KaduActions["encryptionAction"]->toolButtonsForUserListElements(users);
	CONST_FOREACH(i, buttons)
		(*i)->setEnabled(enabled);
*/
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
	if (KeysManagerDialog)
		KeysManagerDialog->turnContactEncryptionText((*(*group).constBegin()).ID("Gadu"), on);
}

void EncryptionManager::sendMessageFilter(const UserListElements users, QString &msg, bool &stop)
{

	ChatWidget* chat = chat_manager->findChatWidget(users);
//	kdebugm(KDEBUG_INFO, "length: %d\n", msg.length());
	if (users.count() == 1 && EncryptionEnabled[chat])
	{
		char *msg_c = sim_message_encrypt((const unsigned char *)msg.data(), (*users.constBegin()).ID("Gadu").toUInt());
		if (!msg_c)
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

void EncryptionManager::sendPublicKeyActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	QString keyfile_path;
	QString mykey;
	QFile keyfile;

	KaduMainWindow *kaduMainWindow = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!kaduMainWindow)
		return;

	UserListElements users = kaduMainWindow->userListElements();

	if (!users.count())
		return;

	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(config_file.readEntry("General", "UIN"));
	keyfile_path.append(".pem");

	keyfile.setName(keyfile_path);

	if (keyfile.open(QIODevice::ReadOnly))
	{
		QTextStream t(&keyfile);
		mykey = t.read();
		keyfile.close();
		foreach(const UserListElement &user, users)
			gadu->sendMessage(user, Message(mykey));

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

void EncryptionManager::showKeysManagerDialog(QAction *sender, bool toggled)
{
	kdebugf();
	if (!KeysManagerDialog)
	{
		KeysManagerDialog = new KeysManager();
		connect(KeysManagerDialog, SIGNAL(destroyed()), this, SLOT(keysManagerDialogDestroyed()));
		connect(KeysManagerDialog, SIGNAL(keyRemoved(UserListElement)), this, SLOT(keyRemoved(UserListElement)));
		connect(KeysManagerDialog, SIGNAL(turnEncryption(UserGroup*, bool)), this, SLOT(turnEncryption(UserGroup*, bool)));
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
	KeysManagerDialog = 0;
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

SavePublicKey::SavePublicKey(UserListElement user, QString keyData, QWidget *parent)
	 : QDialog(parent), 
	user(user), keyData(keyData)
{
	kdebugf();

	setWindowTitle(tr("Save public key"));
	setAttribute(Qt::WA_DeleteOnClose);

	resize(200, 80);

	QString text = tr("User %1 is sending you his public key. Do you want to save it?").arg(user.altNick());

	QLabel *l_info = new QLabel(text, this);

	QPushButton *yesbtn = new QPushButton(tr("Yes"), this);
	QPushButton *nobtn = new QPushButton(tr("No"), this);

	connect(yesbtn, SIGNAL(clicked()), this, SLOT(yesClicked()));
	connect(nobtn, SIGNAL(clicked()), this, SLOT(reject()));

	QGridLayout *grid = new QGridLayout(this);
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

	if (!(keyfile.open(QIODevice::WriteOnly)))
	{
		MessageBox::msg(tr("Error writting the key"), false, "Warning", this);
		kdebugmf(KDEBUG_ERROR, "Error opening key file %s\n", qPrintable(keyfile_path));
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


/*
 * %kadu copyright begin%
 * Copyright 2006, 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004, 2005, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#include <QtCore/QCoreApplication>
#include <QtCrypto>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

#include "action.h"
#include "activate.h"
#include "chat_widget.h"
#include "chat_manager.h"
#include "chat_edit_box.h"
#include "config_file.h"
#include "debug.h"
#include "keys_manager.h"
#include "gadu.h"
#include "icons-manager.h"
#include "kadu.h"
#include "message_box.h"
#include "toolbar.h"
#include "userbox.h"

#include "encryption.h"
#include "kadu_encryption_factory.h"

// for mkdir
#include <sys/stat.h>

/**
 * @ingroup encryption
 * @{
 */

EncryptionManager* encryption_manager;

extern "C" KADU_EXPORT int encryption_init(bool firstLoad)
{
	encryption_manager = new EncryptionManager(firstLoad);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/encryption.ui"), encryption_manager);

	return encryption_manager->encryptionAvailable() ? 0 : 1;
}

extern "C" KADU_EXPORT void encryption_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/encryption.ui"), encryption_manager);

	delete encryption_manager;
	encryption_manager = 0;

	qRemovePostRoutine(QCA::deinit);
}

void disableSendKey(KaduAction *action)
{
	kdebugf();

	UserListElements ules=action->userListElements();

	if (!ules.count())
	{
		action->setEnabled(false);
		return;
	}

	QString keyfile_path;
	keyfile_path.append(profilePath("keys/"));
	keyfile_path.append(QString::number(config_file.readNumEntry("General", "UIN")));
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);

	if (!keyfile.permission(QFileInfo::ReadUser))
	{
		action->setEnabled(false);
		return;
	}

	unsigned int myUin = config_file.readUnsignedNumEntry("General", "UIN");

	foreach(const UserListElement &user, ules)
	{
		if (!user.usesProtocol("Gadu") || user.ID("Gadu").toUInt() == myUin)
		{
			action->setEnabled(false);
			return;
		}
	}

	action->setEnabled(true);
}

EncryptionManager::EncryptionManager(bool firstLoad)
	: MenuId(0), EncryptionEnabled(), EncryptionPossible(), KeysManagerDialog(0)
{
	kdebugf();

	createDefaultConfiguration();

	userlist->addPerContactNonProtocolConfigEntry("encryption_enabled", "EncryptionEnabled");

	connect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QString&, QByteArray&, bool&)),
			this, SLOT(decryptMessage(Protocol *, UserListElements, QString&, QByteArray&, bool&)));
	connect(gadu, SIGNAL(filterOutgoingMessage(const UserListElements, QByteArray &, bool &)),
			this, SLOT(filterOutgoingMessage(const UserListElements, QByteArray &, bool &)));

	encryptionActionDescription = new ActionDescription(
                	ActionDescription::TypeChat, "encryptionAction",
			this, SLOT(encryptionActionActivated(QAction *, bool)),
			"security-high", "security-low", tr("Enable encryption for this conversation"),
			true, tr("Disable encryption for this conversation"),
			disableSendKey
	);
	connect(encryptionActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(setupEncrypt(KaduAction *)));

	if (firstLoad)
		ChatEditBox::addAction("encryptionAction");

	sendPublicKeyActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "sendPublicKeyAction",
		this, SLOT(sendPublicKeyActionActivated(QAction *, bool)),
		"kadu/security-high", "kadu/security-high", tr("Send my public key"), false, QString(),
		disableSendKey
	);
	UserBox::insertActionDescription(2, sendPublicKeyActionDescription);

	keysManagerActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "keysManagerAction",
		this, SLOT(showKeysManagerDialog(QAction *, bool)),
		"kadu/security-high", tr("Manage keys")
	);
	kadu->insertMenuActionDescription(12, keysManagerActionDescription);

	KaduEncryptionFactory *factory = KaduEncryptionFactory::instance();
	EncryptionObject = factory->createEncryptionObject(KaduEncryptionFactory::SIMLite,
							   QDir::toNativeSeparators(profilePath("keys/")));
	if(EncryptionObject == 0)
	{
		MessageDialog::msg(factory->errorInfo(), false, "32x32/dialog-warning", configurationWindow);
	}

	// use mkdir from sys/stat.h - there's no easy way to set permissions through Qt
#ifdef Q_OS_WIN
	QDir().mkdir(profilePath("keys"));
#else
	mkdir(qPrintable(profilePath("keys")), 0700);
#endif

	kdebugf2();
}

EncryptionManager::~EncryptionManager()
{
	kdebugf();

	if(EncryptionObject != 0)
		delete EncryptionObject;

	kadu->removeMenuActionDescription(keysManagerActionDescription);
	delete keysManagerActionDescription;

	disconnect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QString&, QByteArray&, bool&)),
			this, SLOT(decryptMessage(Protocol *, UserListElements, QString&, QByteArray&, bool&)));
	disconnect(gadu, SIGNAL(sendMessageFiltering(const UserListElements, QByteArray &, bool &)),
			this, SLOT(sendMessageFilter(const UserListElements, QByteArray &, bool &)));

	delete encryptionActionDescription;
	UserBox::removeActionDescription(sendPublicKeyActionDescription);
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
	QString myUin=QString::number(config_file.readNumEntry("General","UIN"));
	QString keyfile_path;
	keyfile_path.append(profilePath("keys/"));
	keyfile_path.append(myUin);
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);

	if (keyfile.permission(QFileInfo::WriteUser) && !MessageDialog::ask(tr("Keys exist. Do you want to overwrite them?"), "32x32/dialog-warning", configurationWindow))
		return;

	if (!EncryptionObject->generateKeys(myUin))
	{
		MessageDialog::msg(EncryptionObject->errorDescription(), false, "32x32/dialog-error_big", configurationWindow);
		MessageDialog::msg(tr("Error generating keys"), false, "32x32/dialog-warning", configurationWindow);
		return;
	}

	MessageDialog::msg(tr("Keys have been generated and written"), false, "32x32/dialog-information", configurationWindow);
	kdebugf2();
}

void EncryptionManager::setupEncrypt(KaduAction *action)
{
	kdebugf();

	ChatEditBox *chatEditBox = dynamic_cast<ChatEditBox *>(action->parent());
	if (!chatEditBox)
		return;

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	const UserGroup group = chatWidget->users();

	QString keyfile_path;
	keyfile_path.append(profilePath("keys/"));
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

	setupEncryptButton(chatEditBox, encrypt);
	setupEncryptionButtonForUsers(group->toUserListElements(), encryption_possible);
	EncryptionPossible[chatWidget] = encryption_possible;

	kdebugf2();
}

void EncryptionManager::setupEncryptButton(ChatEditBox* chatEditBox, bool enabled)
{
	kdebugf();

	ChatWidget *chatWidget = chatEditBox->chatWidget();
	if (!chatWidget)
		return;

	EncryptionEnabled[chatWidget] = enabled;

	QAction *action = encryptionActionDescription->action(chatEditBox);
	if (action)
	{
		if (enabled)
			action->setChecked(true);
		else
			action->setChecked(false);
	}

	chat_manager->setChatWidgetProperty(chatWidget->users(), "EncryptionEnabled", QVariant(enabled));
	if (chatWidget->users()->count() == 1)
		(*(chatWidget->users()->begin())).setData("EncryptionEnabled", enabled ? "true" : "false");
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
	setupEncryptButton(chatWidget->getChatEditBox(), !EncryptionEnabled[chatWidget]);
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

	//the encrypted message is Base64 encoded, thus it consists only ASCII characters
	QByteArray message = msg.toAscii();
	if (EncryptionObject->decrypt(message))
	{
		//TODO: this should be moved outside this function, as this function may not
		//know what chararacter encoding is used
		//QTextStream encodingConverter(&msg);
		//encodingConverter.setCodec("Windows-1250");
		//encodingConverter << message;
		msg = message;

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
	foreach (KaduAction *action, encryptionActionDescription->actions())
	{
		if (action->userListElements() == users)
			action->setEnabled(enabled);
	}
	kdebugf2();
}

void EncryptionManager::turnEncryption(UserGroup group, bool on)
{
	ChatWidget *chat = chat_manager->findChatWidget(group->toUserListElements());
	if (chat)
		setupEncryptButton(chat->getChatEditBox(), on);
	else
	{
		chat_manager->setChatWidgetProperty(group, "EncryptionEnabled", QVariant(on));
		(*(group->begin())).setData("EncryptionEnabled", (on ? "true" : "false"));
	}
	if (KeysManagerDialog)
		KeysManagerDialog->turnContactEncryptionText((*(*group).constBegin()).ID("Gadu"), on);
}

void EncryptionManager::sendMessageFilter(const UserListElements users, QByteArray &msg, bool &stop)
{

	ChatWidget* chat = chat_manager->findChatWidget(users);
//	kdebugm(KDEBUG_INFO, "length: %d\n", msg.length());
	if (users.count() == 1 && EncryptionEnabled[chat])
	{
		if(!EncryptionObject->encrypt(msg, (*users.constBegin()).ID("Gadu")))
		{
			kdebugm(KDEBUG_ERROR, "EncryptionObject->encrypt() failed! error=%d errorDescription=%s\n", EncryptionObject->error(), EncryptionObject->errorDescription());
			stop = true;
			MessageDialog::msg(tr("Cannot encrypt message. sim_message_encrypt returned: \"%1\" (error=%2)").arg(EncryptionObject->errorDescription()).arg(EncryptionObject->error()), true, "32x32/dialog-warning");
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

	keyfile_path.append(profilePath("keys/"));
	keyfile_path.append(config_file.readEntry("General", "UIN"));
	keyfile_path.append(".pem");

	keyfile.setName(keyfile_path);

	if (keyfile.open(QIODevice::ReadOnly))
	{
		QTextStream t(&keyfile);
		mykey = t.read();
		keyfile.close();
		foreach(const UserListElement &user, users)
			(dynamic_cast<Protocol *>(gadu))->sendMessage(user, mykey);

		MessageDialog::msg(tr("Your public key has been sent"), false, "32x32/dialog-information", kadu);
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
		connect(KeysManagerDialog, SIGNAL(turnEncryption(UserGroup, bool)), this, SLOT(turnEncryption(UserGroup, bool)));
		KeysManagerDialog->show();
	}
	else
	{
		KeysManagerDialog->setActiveWindow();
		_activateWindow(KeysManagerDialog);
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
		setupEncryptButton(chat->getChatEditBox(), false);
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

	keyfile_path.append(profilePath("keys/"));
	keyfile_path.append(user.ID("Gadu"));
	keyfile_path.append(".pem");
	keyfile.setName(keyfile_path);

	if (!(keyfile.open(QIODevice::WriteOnly)))
	{
		MessageDialog::msg(tr("Error writing the key"), false, "32x32/dialog-warning", this);
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


#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <qdialog.h>

#include "keys_manager.h"
#include "main_configuration_window.h"
#include "misc.h"
#include "usergroup.h"

class Action;
class ChatWidget;
class Protocol;

/**
 * @defgroup encryption Encryption
 * @{
 */
class EncryptionManager : public ConfigurationUiHandler
{
	Q_OBJECT

	int MenuId;

	QMap<ChatWidget *, bool> EncryptionEnabled;
	QMap<ChatWidget *, bool> EncryptionPossible;

	void setupEncryptButton(ChatWidget* chat, bool enabled);
	void setupEncryptionButtonForUsers(UserListElements users, bool enabled);
	Action* action;

	QWidget *configurationWindow;
	void createDefaultConfiguration();

	KeysManager *KeysManagerDialog;

private slots:
	void decryptMessage(Protocol *protocol, UserListElements senders, QCString &msg, QByteArray &formats, bool &ignore);
	void sendMessageFilter(const UserListElements users, QString &msg, bool &stop);
	void setupEncrypt(const UserGroup *group);

	void generateMyKeys();
	void sendPublicKey();
	void encryptionActionActivated(const UserGroup* users);
	void userBoxMenuPopup();

	void keysManagerDialogDestroyed();

	void turnEncryption(UserGroup *group, bool on);
	void keyRemoved(UserListElement ule);
	void keyAdded(UserListElement ule);

public:
	EncryptionManager();
	virtual ~EncryptionManager();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

public slots:
	void showKeysManagerDialog();

};

class SavePublicKey : public QDialog
{
	Q_OBJECT

	UserListElement user;
	QString keyData;

private slots:
	void yesClicked();

public:
	SavePublicKey(UserListElement user, QString keyData, QWidget *parent = 0, const char *name = "save_public_key");

signals:
	void keyAdded(UserListElement ule);

};

/** @} */

#endif

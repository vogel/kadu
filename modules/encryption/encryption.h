#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QtGui/QDialog>

#include "main_configuration_window.h"

class ActionDescription;
class ChatEditBox;
class ChatWidget;
class KeysManager;
class Protocol;
class UserGroup;
class KaduEncryptionInterface;

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

		void setupEncryptButton(ChatEditBox* chatEditBox, bool enabled);
		void setupEncryptionButtonForUsers(UserListElements users, bool enabled);

		ActionDescription* sendPublicKeyActionDescription;
		ActionDescription* encryptionActionDescription;
		ActionDescription* keysManagerActionDescription;

		QWidget *configurationWindow;
		void createDefaultConfiguration();

		KeysManager *KeysManagerDialog;
		KaduEncryptionInterface *EncryptionObject;

	private slots:
		void decryptMessage(Protocol *protocol, UserListElements senders, QString &msg, QByteArray &formats, bool &ignore);
		void filterOutgoingMessage(const UserListElements users, QByteArray &msg, bool &stop);
		void setupEncrypt(KaduAction *action);

		void generateMyKeys();
		void sendPublicKeyActionActivated(QAction *sender, bool toggled);
		void encryptionActionActivated(QAction *sender, bool toggled);

		void keysManagerDialogDestroyed();

		void turnEncryption(UserGroup group, bool on);
		void keyRemoved(UserListElement ule);
		void keyAdded(UserListElement ule);

	public:
		EncryptionManager(bool firstLoad);
		virtual ~EncryptionManager();

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
		bool encryptionAvailable() { return EncryptionObject != 0; }

	public slots:
		void showKeysManagerDialog(QAction *sender, bool toggled);

};

class SavePublicKey : public QDialog
{
	Q_OBJECT

		UserListElement user;
		QString keyData;

	private slots:
		void yesClicked();

	public:
		SavePublicKey(UserListElement user, QString keyData, QWidget *parent = 0);

	signals:
		void keyAdded(UserListElement ule);

};

/** @} */

#endif

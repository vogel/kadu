#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <qdialog.h>

#include "chat_widget.h"
#include "main_configuration_window.h"
#include "misc.h"

class Action;

/**
 * @defgroup encryption Encryption
 * @{
 */
class EncryptionManager : public ConfigurationUiHandler
{
	Q_OBJECT

	QMap<ChatWidget*,bool> EncryptionEnabled;
	void setupEncryptButton(ChatWidget* chat,bool enabled);
	Action* action;

	QWidget *configurationWindow;
	void createDefaultConfiguration();

private slots:
	void decryptMessage(Protocol *protocol, UserListElements senders, QCString &msg, QByteArray &formats, bool &ignore);
	void sendMessageFilter(const UserListElements users, QString &msg, bool &stop);
	void setupEncrypt(const UserGroup *group);
	void chatCreated(ChatWidget *chat);

	void generateMyKeys();
	void sendPublicKey();
	void encryptionActionActivated(const UserGroup* users);
	void userBoxMenuPopup();

public:
	EncryptionManager();
	virtual ~EncryptionManager();
	void enableEncryptionBtnForUsers(UserListElements users);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
};

class SavePublicKey : public QDialog
{
	Q_OBJECT

	UserListElement user;
	QString keyData;

private slots:
	void yesClicked();

public:
	SavePublicKey(UserListElement user, QString keyData, QWidget *parent = 0, const char *name = 0);

};

/** @} */

#endif

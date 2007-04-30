#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <qdialog.h>
#include "chat.h"
#include "misc.h"

class Action;

/**
 * @defgroup encryption Encryption
 * @{
 */
class EncryptionManager : public QObject
{
	Q_OBJECT

	private:
		QMap<Chat*,bool> EncryptionEnabled;
		void setupEncryptButton(Chat* chat,bool enabled);
		Action* action;

	private slots:
		void receivedMessageFilter(Protocol *protocol, UserListElements senders,
								QCString& msg, QByteArray& formats, bool& stop);
		void sendMessageFilter(const UserGroup *users, QCString &msg, bool &stop);
		void setupEncrypt(const UserGroup *group);
		void chatCreated(Chat *chat);

		void generateMyKeys();
		void sendPublicKey();
		void encryptionActionActivated(const UserGroup* users);
		void userBoxMenuPopup();

	public:
		EncryptionManager(QObject *parent=0, const char *name=0);
		~EncryptionManager();
		void enableEncryptionBtnForUsers(UserListElements users);
};

class SavePublicKey : public QDialog {
	Q_OBJECT
	public:
		SavePublicKey(UserListElement user, QString keyData, QWidget *parent = 0, const char *name = 0);

	private:
		UserListElement user;
		QString keyData;

	private slots:
		void yesClicked();
};

/** @} */

#endif

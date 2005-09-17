#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <qdialog.h>
#include "chat.h"
#include "misc.h"

class EncryptionManager : public QObject
{
	Q_OBJECT

	private:
		QMap<Chat*,bool> EncryptionEnabled;
		void setupEncryptButton(Chat* chat,bool enabled);

	private slots:
		void receivedMessageFilter(Protocol *protocol, UserListElements senders,
								QCString& msg, QByteArray& formats, bool& stop);
		void sendMessageFilter(const UserGroup *users, QCString &msg, bool &stop);
		void chatCreated(const UserGroup *group);

		void generateMyKeys();
		void sendPublicKey();
		void onUseEncryption(bool toggled);
		void encryptionActionActivated(const UserGroup* users);
		void userBoxMenuPopup();
		void createConfigDialogSlot();

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


#endif

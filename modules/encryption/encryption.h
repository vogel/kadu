#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <qdialog.h>
#include "chat.h"
#include "misc.h"

class EncryptionManager : public QObject
{
	Q_OBJECT

	private:
		QMap<const QPushButton*,Chat*> EncryptionButtonChat;
		QMap<Chat*,bool> EncryptionEnabled;
		void setupEncryptButton(Chat* chat,bool enabled);
	
	private slots:
		void createConfigDialogSlot();
		void generateMyKeys();
		void onUseEncryption(bool toggled);
		void chatCreated(const UinsList& uins);
		void encryptionButtonClicked();
		void receivedMessageFilter(const UinsList& senders,
			QCString& msg,QByteArray& formats,bool& stop);
		void sendMessageFilter(const UinsList& uins,QCString& msg,bool& stop);
		void sendPublicKey();
		void userBoxMenuPopup();

	public:
		EncryptionManager();
		~EncryptionManager();
		void enableEncryptionBtnForUins(UinsList uins);
};

class SavePublicKey : public QDialog {
	Q_OBJECT
	public:
		SavePublicKey(UinType uin, QString keyData, QWidget *parent = 0, const char *name = 0);

	private:
		UinType uin;
		QString keyData;

	private slots:
		void yesClicked();
};


#endif

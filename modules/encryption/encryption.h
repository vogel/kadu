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
		void receivedMessageFilter(const UinsList& senders,
			QCString& msg,QByteArray& formats,bool& stop);
		void sendMessageFilter(const UinsList& uins,QCString& msg,bool& stop);
		void chatCreated(const UinsList& uins);

		void generateMyKeys();
		void sendPublicKey();
		void onUseEncryption(bool toggled);
		void encryptionButtonClicked();
		void userBoxMenuPopup();
		void createConfigDialogSlot();

	public:
		EncryptionManager(QObject *parent=0, const char *name=0);
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

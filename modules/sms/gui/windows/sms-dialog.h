#ifndef SMS_DIALOG_H
#define SMS_DIALOG_H

#include <QtCore/QBuffer>
#include <QtCore/QMap>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtNetwork/QHttp>

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "http_client.h"
#include "gui/actions/action.h"
#include "gui/windows/main-configuration-window.h"

#include "sms_exports.h"
#include "sms-gateway.h"
#include "sms-sender.h"

class QCheckBox;
class QComboBox;
class QListWidget;
class QProcess;
class QTextEdit;

class SMSAPI SmsDialog : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

		QTextEdit *body;
		QLineEdit *recipient;
		QComboBox *list;
		QLabel *smslen;
		QLabel *l_contact;
		QLineEdit *e_contact;
		QLabel *l_signature;
		QLineEdit *e_signature;
		QPushButton *b_send;
		QCheckBox *c_saveInHistory;
		QProcess *smsProcess;
		SmsSender Sender;
		QCheckBox *AutoSelectProvider;
		QComboBox *ProvidersList;


	private slots:
		void updateRecipient(const QString &);
		void updateList(const QString& newnumber);
		void editReturnPressed();
		void sendSms();
		void updateCounter();
		void smsSigHandler();
		void onSmsSenderFinished(bool success);

	protected:
		virtual void configurationUpdated();
		virtual void keyPressEvent(QKeyEvent *e);

	public:
		SmsDialog(const QString& altnick, QWidget* parent = 0);
		~SmsDialog();
		void setRecipient(const QString& phone);
};

/** @} */

#endif // SMS_DIALOG_H

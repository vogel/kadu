#ifndef __KADU_FIREWALL_H
#define __KADU_FIREWALL_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QRegExp>

#include "configuration_aware_object.h"
#include "../modules/gadu_protocol/gadu.h"
#include "main_configuration_window.h"
class ChatWidget;
class QListWidget;

class Firewall : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	public:
		Firewall();
		virtual ~Firewall();
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
		
	private:
		QStringList secured, secured_temp_allowed;
		QString last_uin;	//TODO: pozbyæ siê tego
		UserListElements passed;
		unsigned int flood_messages;
		QTime lastMsg;
		QTime lastNotify;
		bool right_after_connection;
		QRegExp pattern;
		QListWidget* allList;
		QListWidget* secureList;
		QTextEdit *questionEdit;
		QLineEdit *answerEdit;

		void loadSecuredList(void);
		void saveSecuredList(void);
		bool isSecured(const QString &id);
		
		bool checkChat(Protocol *protocol, const QString &message, const UserListElements senders, const QString &user, bool &stop);
		bool checkConference(const QString &message, const UserListElements senders, const QString &user);
		bool checkEmoticons(const QString &message);
		bool checkFlood();
		
		void showHint(const QString &u, const QString &m);
		void writeLog(const QString &u, const QString &m);

		void defaultSettings();
		
	private slots:
		void messageFiltering(Protocol *protocol, UserListElements senders, QString& msg, QByteArray& formats, bool& stop);
		void chatDestroyed(ChatWidget *);
		void sendMessageFilter(UserListElements,QByteArray&,bool&);

		void userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool);
		void userAdded(UserListElement, bool, bool);
		void userRemoved(UserListElement, bool, bool);
		
		void connected();
		void connecting();
		void changeRight_after_connection();
		
		void _Left(QListWidgetItem *);
		void _Right(QListWidgetItem *);
		void _AllLeft();
		void _AllRight();

		void configurationApplied();

	protected:
		virtual void configurationUpdated();
};

#endif

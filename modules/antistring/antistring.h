#ifndef ANTISTRING_H
#define ANTISTRING_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtGui/QSpinBox>

#include "usergroup.h"

#include "configuration_aware_object.h"
#include "main_configuration_window.h"

class Protocol;
class QListWidget;
class QSpinBox;
class QLineEdit;

class Antistring : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	private:
		QListWidget *conditionList;
		QSpinBox *factor;
		QLineEdit *condition;
		QMap<int,QString> conditions1;
		QMap<int,int> conditions;

		int points(QString& msg);
		void writeLog(UserListElements uin, QString msg);
		void conditionsSave();
		void conditionsRead();
		void updateConditionList();
		void addDefaultConfiguration();

	private slots:
		void addCondition();
		void changeCondition();
		void deleteCondition();
		void wordSelected(QListWidgetItem * item);

	public:
		Antistring();
		~Antistring();
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	public slots:
		void messageFiltering(Protocol *protocol, UserListElements senders, QString& msg, QByteArray& formats, bool& stop);

	protected:
		virtual void configurationUpdated();
};

#endif


#ifndef CENZOR_H
#define CENZOR_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "usergroup.h"
#include "configuration_aware_object.h"
#include "main_configuration_window.h"

class Chat;
class Protocol;
class QListWidget;
class QListWidgetItem;
class QLineEdit;
class QStringList;

class Cenzor : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT;
    
	private:
		QStringList swearList;
		QStringList exclusionList;

		QListWidget *swearListBox;
		QListWidget *exclusionListBox;
		QLineEdit *swear;
		QLineEdit *exclusion;

		/*!
			\fn int check(QString& msg)
			\param msg the message to receive
		*/
		int check(QString& msg);
		
		void words_read();
		void words_save();
		int checkOkWords(QString txt);
		void update_swearList();
		void update_exclusionList();
		void addDefaultConfiguration();

	public:
		//! Default constructor
		/*! 
			\param pk pointer to PowerKadu object
		*/
		Cenzor();

		//! Default destructor
		~Cenzor();

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);


	private slots:
		void swearwordSelected(QListWidgetItem *item);
		void addSwearword();
		void changeSwearword();
		void deleteSwearword();

		void exlusionSelected(QListWidgetItem *item);
		void addExclusion();
		void changeExclusion();
		void deleteExclusion();

	public slots:
		void messageFiltering(Protocol *protocol, UserListElements senders, QString& msg, QByteArray& formats, bool& stop);

	protected:
		virtual void configurationUpdated();
};

extern Cenzor *cenzor;

#endif

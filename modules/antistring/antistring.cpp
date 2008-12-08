/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "antistring.h"
#include "../notify/notify.h"

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "misc.h"
#include "debug.h"
#include "protocols/protocol.h"
#include "config_file.h"
#include "chat_manager.h"
#include "icons_manager.h"

#include <QtCore/QMap>
#include <QtCore/QRegExp>
#include <QtCore/QProcess>
#include <QtCore/QTextStream>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>

Antistring *antistring;

extern "C" int antistring_init()
{
	antistring = new Antistring();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/antistring.ui"), antistring);
	notification_manager->registerEvent("Antistring", "Antistring notifications", CallbackNotRequired);

	return 0;
}

extern "C" void antistring_close()
{
	notification_manager->unregisterEvent("Antistring");
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/antistring.ui"), antistring);
	delete antistring;
	antistring = 0;
}

Antistring::Antistring()
{
	addDefaultConfiguration();
	conditionsRead();

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	connect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QString&, QByteArray&, bool&)),
			this,
			SLOT(messageFiltering(Protocol *, UserListElements, QString&, QByteArray&, bool&)));
}

Antistring::~Antistring()
{
	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();

	disconnect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QString&, QByteArray&, bool&)),
			this,
			SLOT(messageFiltering(Protocol *, UserListElements, QString&, QByteArray&, bool&)));
	conditionsSave();
}

void Antistring::messageFiltering(Protocol *protocol, UserListElements senders, QString& msg, QByteArray& formats, bool& stop)
{
	kdebugf();

	if (config_file.readBoolEntry("PowerKadu", "enable_antistring")) {
		if (points(msg) >= 3) {

			Notification *notification = new Notification("Antistring", "", senders);
			notification->setText(tr("Antistring"));
			notification->setDetails(tr("Your interlocutor send you love letter"));
			notification_manager->notify(notification);

			protocol->sendMessage(senders, config_file.readEntry("PowerKadu", "admonish_tresc_config"));

			if (config_file.readEntry("PowerKadu", "message stop") == "true") {
				stop = true;
			}

			if (config_file.readEntry("PowerKadu", "log message") == "true") {
				writeLog(senders, msg);
			}
		}
	}
	kdebugf2();
}

void Antistring::writeLog(UserListElements uin, QString msg)
{
	kdebugf();

	QFile log_file(config_file.readEntry("PowerKadu", "log file", ggPath("antistring.log")));
	if (!log_file.exists()) {
		log_file.open(IO_WriteOnly);
		QTextStream stream( &log_file );
		stream << tr("     DATA AND TIME      ::   UIN   ::    MESSAGE\n") <<
				"====================================================\n";
		log_file.close();
	}

	log_file.open(IO_WriteOnly | IO_Append);
	QTextStream stream( &log_file );
	stream << QDateTime::currentDateTime(Qt::LocalTime).toString() << " :: " << uin[0].ID("Gadu") << " :: " <<
			msg << "\n";
	log_file.close();
	kdebugf2();
}

int Antistring::points(QString& msg)
{
	kdebugf();
	int il_p = 0;

	if (msg.length() > 600 )	{ il_p +=1; }
	for (uint i = 0; i < conditions1.keys().count(); i++) {
		if (msg.find(QRegExp(conditions1[i],true)) >= 0 ) {
			il_p += conditions[i];
		}
	}

	return il_p;
	kdebugf2();
}

void Antistring::conditionsSave()
{
	QStringList conditions_list;
	for (uint i = 0; i < conditions.keys().count(); i++) {
		conditions_list.append(QString::number(conditions[i])+"\t"+conditions1[i]);
	}
	config_file.writeEntry("PowerKadu", "antistring conditions", conditions_list.join("\t\t"));
}

void Antistring::conditionsRead()
{
	kdebugf();
	int i=0;
	bool ok;
	QString data = config_file.readEntry("PowerKadu", "antistring conditions");
	QStringList list = QStringList::split("\t\t", data);
	if (!list.count()) {
		QFile defList(dataPath("kadu/modules/data/antistring/ant_conditions.conf"));
		if (defList.open(IO_ReadOnly)) {
			QTextStream s(&defList);
			QStringList pair;
			while (!s.atEnd())
			{
				pair = QStringList::split('\t', s.readLine());
				if (pair.count() <= 0)
					continue;
				conditions[i]  = pair[0].toInt( &ok, 10);
				conditions1[i] = pair[1];
				i++;
			}
			defList.close();
		}
		else
		{
			kdebug(QString("Can't open file: %1").arg(defList.name()));
		}
	}
	else
	{
		for (uint i = 0; i < list.count(); i++) {
			QStringList sp = QStringList::split('\t', list[i]);
			conditions[i]  = sp[0].toInt( &ok, 10);
			conditions1[i] = sp[1];
		}
	}
	kdebugf2();
}

void Antistring::updateConditionList()
{
	conditionList->clear();
	for (uint i = 0; i < conditions.keys().count(); i++) {
		conditionList->addItem("(" + QString::number(conditions[i]) + ") " + conditions1[i]);
	}
}

void Antistring::wordSelected(QListWidgetItem * item)
{
	int index = conditionList->currentIndex().row();
	factor->setValue(conditions[index]);
	condition->setText(conditions1[index]);
}

void Antistring::changeCondition()
{
	bool ok;
	QListWidgetItem *item = conditionList->currentItem();
	int index = conditionList->currentIndex().row();
	
	QString Condition = condition->text();
	QString wartosc = factor->text();

	if (Condition.isEmpty())
		return;

	item->setText("("+wartosc+") "+Condition);

	conditions[index]=wartosc.toInt(&ok, 10);
	conditions1[index]=Condition;

	factor->setValue(0);
	condition->setText("");
}

void Antistring::deleteCondition()
{
	
	QListWidgetItem *item = conditionList->currentItem();
	int index = conditionList->currentIndex().row();

	for (uint i = index ; i < conditions.keys().count()-1 ; i++) {
		conditions[i]=conditions[i+1];
		conditions1[i]=conditions1[i+1];
	}

	conditions1.remove(conditions.keys().count()-1);
	conditions.remove(conditions.keys().count()-1);

	updateConditionList();
}

void Antistring::addCondition()
{
	kdebugf();

	QString Condition = condition->text();
	QString wartosc = factor->text();

	if (Condition.isEmpty())
		return;

	conditionList->addItem("(" + wartosc + ") " + Condition);

	int ilosc = conditions.keys().count();
	bool ok;
	conditions[ilosc]=wartosc.toInt(&ok, 10);
	conditions1[ilosc]=Condition;

	factor->setValue(0);
	condition->setText("");

	kdebugf2();
}

void Antistring::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	kdebugf();

	ConfigGroupBox *groupBox = mainConfigurationWindow->configGroupBox("Chat", "Antistring", "Conditions");

	QWidget *widget = new QWidget(groupBox->widget());
	widget->setContentsMargins(0, 0, 0, 0);

	QGridLayout *layout = new QGridLayout;

	conditionList = new QListWidget;
	layout->addMultiCellWidget(conditionList, 0, 0, 0, 3);

	condition = new QLineEdit;
	layout->addWidget(new QLabel(tr("Condition"), widget), 1, 0);
	layout->addWidget(condition, 1, 1, 1, 3);

	factor = new QSpinBox(widget);
	factor->setMinValue(0);
	factor->setMaxValue(5);
	factor->setSpecialValueText(tr("Don't use"));
	layout->addWidget(new QLabel(tr("Factor"), widget), 2, 0);
	layout->addWidget(factor, 2, 1);

	QPushButton *addConditionButton = new QPushButton(tr("Add"), widget);
	QPushButton *changeConditionButton = new QPushButton(tr("Change"), widget);
	QPushButton *deleteConditionButton = new QPushButton(tr("Delete"), widget);
	layout->addWidget(addConditionButton, 3, 1);
	layout->addWidget(changeConditionButton, 3, 2);
	layout->addWidget(deleteConditionButton, 3, 3);

	widget->setLayout(layout);
	groupBox->addWidget(widget);
	
	connect(conditionList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(wordSelected(QListWidgetItem *)));
	connect(addConditionButton, SIGNAL(clicked()), this, SLOT(addCondition()));
	connect(changeConditionButton, SIGNAL(clicked()), this, SLOT(changeCondition()));
	connect(deleteConditionButton, SIGNAL(clicked()), this, SLOT(deleteCondition()));

	connect(mainConfigurationWindow->widgetById("antistring/enable_log"), SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("antistring/log_file"), SLOT(setEnabled(bool)));

	updateConditionList();
	kdebugf2();
}

void Antistring::configurationUpdated()
{
}

void Antistring::addDefaultConfiguration()
{
	kdebugf();

	config_file.addVariable("PowerKadu", "log file", ggPath("antistring.log"));
	config_file.addVariable("PowerKadu", "admonish_tresc_config", 
		tr("http://www.olsztyn.mm.pl/~silentman/lancuszki.htm "));

	kdebugf2();
}

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "protocols/protocol.h"
#include "debug.h"
#include "misc/misc.h"
#include "modules/notify/notify.h"
#include "icons-manager.h"

#include "cenzor.h"

#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtGui/QSpinBox>
#include <QtGui/QListWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QListWidgetItem>

Cenzor *cenzor;

extern "C" int cenzor_init()
{
	cenzor = new Cenzor();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/cenzor.ui"), cenzor);
	notification_manager->registerEvent("Cenzor", "Cenzor notifications", CallbackNotRequired);

	return 0;
}

extern "C" void cenzor_close()
{
	notification_manager->unregisterEvent("Cenzor");
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/cenzor.ui"), cenzor);
	delete cenzor;
	cenzor = 0;
}

Cenzor::Cenzor()
{
	kdebugf();

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocolHandler();
	connect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QString&, QByteArray&, bool&)), this, SLOT(messageFiltering(Protocol *, UserListElements, QString&, QByteArray&, bool&)));

	words_read();
	addDefaultConfiguration();

	kdebugf2();
}

Cenzor::~Cenzor()
{
	kdebugf();

	words_save();

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	disconnect(gadu, SIGNAL(rawGaduReceivedMessageFilter(Protocol *, UserListElements, QString&, QByteArray&, bool&)), this, SLOT(messageFiltering(Protocol *, UserListElements, QString&, QByteArray&, bool&)));

	kdebugf2();
}

void Cenzor::messageFiltering(Protocol *protocol, UserListElements senders, QString& msg, QByteArray& formats, bool& stop)
{
	kdebugf();

	if (config_file.readEntry("PowerKadu", "enable_cenzor") == "true")
	{
		if ( check(msg) >= 1 )
    		{
			protocol->sendMessage(senders, config_file.readEntry("PowerKadu", "admonition_content_cenzor"));

			Notification *notification = new Notification("Cenzor", "", senders);
			notification->setText("Cenzor");
			notification->setDetails(tr("Your interlocutor used obscene word and became admonished"));
			notification_manager->notify(notification);
		}
	}
	kdebugf2();
}

int Cenzor::check(QString& msg)
{
	kdebugf();
	int il_w = 0;

	QStringList list = QStringList::split(" ", msg);

	for( QStringList::Iterator it = list.begin(); it != list.end(); it++)
	{
		QString word = (*it).toLower();
		for (QStringList::Iterator itw = swearList.begin(); itw != swearList.end(); itw++)
			if ((word.find(QRegExp((*itw), true)) >= 0) && (!checkOkWords(word)))
			{
				il_w += 1;
			}
	}

	return il_w;
}

int Cenzor::checkOkWords(QString txt)
{
	kdebugf();

	for (QStringList::Iterator p = exclusionList.begin(); p != exclusionList.end(); p++)
		if (txt.find(QRegExp((*p), true)) >= 0)
			return 1;

	return 0;
	kdebugf2();
}

void Cenzor::words_read()
{
	kdebugf();

	QString data = config_file.readEntry("PowerKadu", "cenzor swearwords");
	swearList = QStringList::split("\t", data);

	if (!swearList.count())
	{
		QFile file(dataPath("kadu/modules/data/cenzor/cenzor_words.conf"));

		if (file.open(IO_ReadOnly))
		{
			QTextStream stream(&file);
			while (!stream.atEnd())
			{
				swearList += stream.readLine();
			}
			file.close();
		}
		else
		{
			kdebug(QString("Can't open file: %1").arg(file.name()));
		}
	}
	
	data = config_file.readEntry("PowerKadu", "cenzor exclusions");
	exclusionList = QStringList::split("\t", data);
	if (!exclusionList.count())
	{
		QFile file_ok(dataPath("kadu/modules/data/cenzor/cenzor_words_ok.conf"));
		if (file_ok.open(IO_ReadOnly))
		{
			QTextStream stream_ok(&file_ok);

			while (!stream_ok.atEnd())
			{
				exclusionList += stream_ok.readLine();
			}
			file_ok.close();
		}
		else
		{
			kdebug(QString("Can't open file: %1").arg(file_ok.name()));
		}
	}

	kdebugf2();
}

void Cenzor::words_save()
{
	kdebugf();

	QStringList swearwords_list;
	QStringList exclusions_list;

	for (QStringList::Iterator it = swearList.begin(); it != swearList.end(); it++)
		swearwords_list.append(*it);
	config_file.writeEntry("PowerKadu", "cenzor swearwords", swearwords_list.join("\t"));

	for (QStringList::Iterator it = exclusionList.begin(); it != exclusionList.end(); it++)
		exclusions_list.append(*it);
	config_file.writeEntry("PowerKadu", "cenzor exclusions", exclusions_list.join("\t"));

	kdebugf2();
}

void Cenzor::update_swearList()
{
	kdebugf();

	swearListBox->clear();
	for (QStringList::Iterator it = swearList.begin(); it != swearList.end(); it++)
	{
		swearListBox->addItem(*it);
	}

	kdebugf2();
}

void Cenzor::update_exclusionList()
{
	kdebugf();

	exclusionListBox->clear();
	for (QStringList::Iterator it = exclusionList.begin(); it != exclusionList.end(); it++)
	{
		exclusionListBox->addItem(*it);
	}

	kdebugf2();
}

void Cenzor::swearwordSelected(QListWidgetItem * item)
{
	kdebugf();
	swear->setText(item->text());
	kdebugf2();
}


void Cenzor::addSwearword()
{
	kdebugf();

	if (swear->text().isEmpty())
		return;

	swearListBox->addItem(swear->text());
	swearList += swear->text();
	swear->setText("");

	kdebugf2();
}


void Cenzor::changeSwearword()
{
	kdebugf();

	QListWidgetItem *item = swearListBox->currentItem();
	if (!item || item->text().isEmpty())
		return;

	int index = swearList.indexOf(item->text());
	item->setText(swear->text());
	swearList[index] = swear->text();
	swear->setText("");

	kdebugf2();
}

void Cenzor::deleteSwearword()
{
	kdebugf();

	QListWidgetItem *item = swearListBox->currentItem();
	if (!item)
		return;

	int index = swearList.indexOf(item->text());
	for (uint i = index; i < swearList.count()-1; i++)
		swearList[i] = swearList[i+1];

	swearList.pop_back();
	swear->setText("");
	update_swearList();

	kdebugf2();
}

void Cenzor::exlusionSelected(QListWidgetItem *item)
{
	kdebugf();
	exclusion->setText(item->text());
	kdebugf2();
}

void Cenzor::addExclusion()
{
	kdebugf();

	if (exclusion->text().isEmpty())
		return;

	exclusionListBox->addItem(exclusion->text());
	exclusionList += exclusion->text();
	exclusion->setText("");

	kdebugf2();
}

void Cenzor::changeExclusion()
{
	kdebugf();

	QListWidgetItem *item = exclusionListBox->currentItem();
	if (!item || item->text().isEmpty())
		return;

	int index = exclusionList.indexOf(item->text());

	item->setText(exclusion->text());
	exclusionList[index] = exclusion->text();
	exclusion->setText("");

	kdebugf2();
}

void Cenzor::deleteExclusion()
{
	kdebugf();

	QListWidgetItem *item = exclusionListBox->currentItem();
	int index = exclusionList.indexOf(item->text());

	for (uint i = index; i < exclusionList.count() - 1; i++)
		exclusionList[i] = exclusionList[i + 1];

	exclusionList.pop_back();
	update_exclusionList();
	exclusion->setText("");

	kdebugf2();
}

void Cenzor::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	// Swearwords
	ConfigGroupBox *groupBox = mainConfigurationWindow->configGroupBox("Chat", "Cenzor", "Swearwords");

	QWidget *swearwordsWidget = new QWidget(groupBox->widget());
	
	QGridLayout *layout = new QGridLayout(swearwordsWidget);
	layout->setSpacing(5);
	layout->setMargin(5);

	swearListBox = new QListWidget(swearwordsWidget);
	layout->addMultiCellWidget(swearListBox, 0, 0, 0, 4);

	swear = new QLineEdit(swearwordsWidget);
	layout->addWidget(swear, 2, 0);

	QPushButton *addSwearwordButton = new QPushButton(tr("Add"), swearwordsWidget);
	QPushButton *changeSwearwordButton = new QPushButton(tr("Change"), swearwordsWidget);
	QPushButton *deleteSwearwordButton = new QPushButton(tr("Delete"), swearwordsWidget);
	layout->addWidget(addSwearwordButton, 2, 1);
	layout->addWidget(changeSwearwordButton, 2, 2);
	layout->addWidget(deleteSwearwordButton, 2, 3);

	groupBox->addWidgets(0, swearwordsWidget);
	
	connect(swearListBox, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(swearwordSelected(QListWidgetItem *)));
	connect(addSwearwordButton, SIGNAL(clicked()), this, SLOT(addSwearword()));
	connect(changeSwearwordButton, SIGNAL(clicked()), this, SLOT(changeSwearword()));
	connect(deleteSwearwordButton, SIGNAL(clicked()), this, SLOT(deleteSwearword()));


	// Exclusions
	ConfigGroupBox *groupBox2 = mainConfigurationWindow->configGroupBox("Chat", "Cenzor", "Exclusions");
	QWidget *exclusionsWidget = new QWidget(groupBox2->widget());
	QGridLayout *layout2 = new QGridLayout(exclusionsWidget);
	layout2->setSpacing(5);
	layout2->setMargin(5);

	exclusionListBox = new QListWidget(exclusionsWidget);
	layout2->addMultiCellWidget(exclusionListBox, 0, 0, 0, 4);

	exclusion = new QLineEdit(exclusionsWidget);

	QPushButton *addExclusionButton = new QPushButton(tr("Add"), swearwordsWidget);
	QPushButton *changeExclusionButton = new QPushButton(tr("Change"), swearwordsWidget);
	QPushButton *deleteExclusionButton = new QPushButton(tr("Delete"), swearwordsWidget);
	layout2->addWidget(exclusion, 1, 0);
	layout2->addWidget(addExclusionButton, 1, 1);
	layout2->addWidget(changeExclusionButton, 1, 2);
	layout2->addWidget(deleteExclusionButton, 1, 3);

	groupBox2->addWidgets(0, exclusionsWidget);
	
	connect(exclusionListBox, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(exlusionSelected(QListWidgetItem *)));
	connect(addExclusionButton, SIGNAL(clicked()), this, SLOT(addExclusion()));
	connect(changeExclusionButton, SIGNAL(clicked()), this, SLOT(changeExclusion()));
	connect(deleteExclusionButton, SIGNAL(clicked()), this, SLOT(deleteExclusion()));

	// Update lists
	update_swearList();
	update_exclusionList();

	kdebugf2();
}

void Cenzor::configurationUpdated()
{
}

void Cenzor::addDefaultConfiguration()
{
	config_file.addVariable("PowerKadu", "admonition_content_cenzor", tr("Cenzor: Watch your mouth!! <nonono>"));
}

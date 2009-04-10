#ifndef HISTORY_H
#define HISTORY_H

#include <QtCore/QObject>
#include <QtGui/QLabel>
#include <QtGui/QKeyEvent>
#include <QtGui/qcheckbox.h>
#include <QtCore/qmap.h>
#include <QtCore/qpair.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvariant.h>
#include <QtCore/qdatetime.h>
#include <QtGui/qdialog.h>

#include "action.h"
#include "protocols/protocol.h"
#include "main_configuration_window.h"
#include "configuration_aware_object.h"

enum HistoryEntryType
{
	EntryTypeMessage = 0x00000001,
	EntryTypeStatus = 0x00000010,
	EntryTypeSms = 0x00000020,
	EntryTypeAll = 0x0000003f
};


#include "gui/windows/history-dialog.h"
#include "gui/widgets/contacts-list-widget-menu-manager.h"
#include "storage/history-storage.h"

class Account;
class HistoryDlg;

class History : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	static History *Instance;
	HistoryStorage *CurrentStorage;
	HistoryDlg *HistoryDialog;
	ActionDescription *ShowHistoryActionDescription;

	History();
	~History();

	void createActionDescriptions();
	void deleteActionDescriptions();

	virtual void configurationUpdated();
	void showHistoryActionActivated(QAction *sender, bool toggled);
	void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

private slots:

	void accountRegistered(Account *);
	void accountUnregistered(Account *);

public:
	static History * instance();
	void registerStorage(HistoryStorage *storage);
	void unregisterStorage(HistoryStorage *storage);

};

	void disableNonHistoryContacts(KaduAction *action);

#endif

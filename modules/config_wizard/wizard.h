#ifndef KADU_CONFIG_WIZARD_H
#define KADU_CONFIG_WIZARD_H

#include <qwizard.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>

#include "../account_management/register.h"
#include "chat_widget.h"
#include "misc.h"

class QGrid;

/**
 * @defgroup config_wizard Config wizard
 * @{
 */
class Wizard : public QWizard
{
    Q_OBJECT

	QPushButton *ggRegisterAccount;

	QRadioButton *haveNumber;
	QLineEdit *ggNumber;
	QLineEdit *ggPassword;
	QCheckBox *ggImportContacts;
	QRadioButton *dontHaveNumber;
	QLineEdit *ggNewPassword;
	QLineEdit *ggReNewPassword;
	QLineEdit *ggEMail;

	QValueList<QWidget *> haveNumberWidgets;
	QValueList<QWidget *> dontHaveNumberWidgets;

	QComboBox *browserCombo;
	QLineEdit *browserCommandLineEdit;
	QComboBox *mailCombo;
	QLineEdit *mailCommandLineEdit;

	void createGGAccountPage();
	void createApplicationsPage();
	void createSoundPage();

	void loadGGAccountOptions();
	void loadApplicationsOptions();
	void loadSoundOptions();

	void saveGGAccountOptions();
	void saveApplicationsOptions();
// 	void saveSoundOptions();

	void tryImport();

private slots:
	void haveNumberChanged(bool haveNumber);
	void registerGGAccount();
	void registeredGGAccount(bool ok, UinType uin);

	void finishClicked();
	void cancelClicked();

	void browserChanged(int index);
	void emailChanged(int index);

protected:
	void closeEvent(QCloseEvent *e);

public:
	Wizard(QWidget *parent = 0, const char *name = 0, bool modal = false);
	~Wizard();

	void addPage(QWidget *page, const QString &title, const QString &description, bool lastOne);

public slots:
	void wizardStart();

};

class WizardStarter : public QObject
{
	Q_OBJECT

	int menuPos;

public:
	WizardStarter(QObject *parent=0, const char *name=0);
	~WizardStarter();

public slots:
	void start();

	void userListImported(bool ok, QValueList<UserListElement> list);
	void connected();

};

extern WizardStarter *wizardStarter;
extern Wizard *startWizardObj;

/** @} */

#endif

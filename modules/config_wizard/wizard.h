#ifndef WIZARD_H
#define WIZARD_H

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
#include "chat.h"

class Wizard : public QWizard
{
    Q_OBJECT

public:
	Wizard( QWidget *parent = 0, const char *name = 0 );	/* konstruktor */
	~Wizard();	/* destruktor */
	bool noNewAccount;
	int menuPos;

protected:
	/*  te funkcje wyswietlaja poszczegolne okna wizarda */ 
	void showWelcomePage();
	void showGGNumberSelect();
	void showGGCurrentNumberPage();
	void showGGNewNumberPage();
	void showLanguagePage();
	void showWWWOpionsPage();
	void showChatOpionsPage();
	void showSoundOptionsPage();
	void showGeneralOptionsPage();
	void showHintsOptionsPage();
	void showColorsPage();
	void showInfoPanelPage();
	void showQtStylePage();
	void showGreetingsPage();
	
	/* a tu sa okna */
    QVBox *welcomePage, *ggNumberSelect, *ggCurrentNumberPage, *ggNewNumberPage, *languagePage, *chatOptionsPage, *wwwOptionsPage, 
		*soundOptionsPage, *generalOptionsPage, *greetingsPage, *hintsOptionsPage, *colorsPage, *qtStylePage, *infoPanelPage;
	QRadioButton *rb_haveNumber, *rb_dontHaveNumber;
	QLineEdit *l_ggNumber, *l_ggPassword, *l_ggNewPasssword, *l_ggNewPassswordRetyped, *l_email, *l_customBrowser;
	QCheckBox *c_importContacts, *c_waitForDelivery, *c_enterSendsMessage, *c_openOnNewMessage, *c_flashTitleOnNewMessage, *c_ignoreAnonyms,
		*c_logMessages, *c_logStatusChanges, *c_privateStatus, *c_showBlocked, *c_showBlocking, *c_startDocked, *c_enableSounds, *c_playWhilstChatting,
		*c_playWhenInvisible, *c_showInfoPanel, *c_showScrolls;
	QComboBox *cb_browser, *cb_browserOptions, *cb_hintsTheme, *cb_hintsType, *cb_colorTheme, *cb_iconTheme, *cb_qtTheme, *cb_panelTheme;
	QLabel *preview, *preview2, *preview4, *iconPreview, *iconPreview2, *iconPreview3, *iconPreview4, *infoPreview;
	QString customHint, customPanel;
	/* a tu poszczegolne funkcje zapisujace konfiguracje */
	void tryImport();
	void setGaduAccount();
	void setChatOptions();
	void setSoundOptions();
	void setGeneralOptions();
	void setBrowser();
	void setHints();
	void setColorsAndIcons();
	void setPanelTheme();
	/* funkcje pomocnicze */
	QString toDisplay(QString);
	QString toSave(QString);

protected slots:
	/* a tu implementacja slotow */
	void setLanguage(int);	/* ustawia jezyk */
	void registeredAccount(bool, UinType);	/* jak zarejestrowal konto */
	void previewHintsTheme(int);
	void previewHintsType(int);
	void previewColorTheme(int);
	void previewIconTheme(int);
	void previewQtTheme(int);
	void previewPanelTheme(int);
	//void addScrolls(bool);

	void wizardStart();
	void finishPressed();
	void nextPressed();
	void backPressed();

	void findAndSetWebBrowser(int selectedBrowser);
	void findAndSetBrowserOption(int selectedOption);

	void userListImported(bool ok, UserList& userList);
	void connected();
};


extern Wizard *startWizardObj;
#endif

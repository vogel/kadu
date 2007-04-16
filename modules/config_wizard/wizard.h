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
#include "chat.h"
#include "misc.h"

/**
 * @defgroup config_wizard Config wizard
 * @{
 */
class Wizard : public QWizard
{
    Q_OBJECT

		enum HintType {
			HintConnectionError,
			HintNewChat,
			HintNewMessage,
			HintStatusChangedToOnline,
			HintStatusChangedToBusy,
			HintStatusChangedToInvisible,
			HintStatusChangedToOffline
		};

		enum ColorType {
			ColorForeground = 0,
			ColorBackground = 1
		};

		QString currentColors[8];	//tu przechowujemy aktualny zestaw kolorow
		QString currentHints[13][2]; //a tu aktualne kolorki hintow
		bool registered;	//potrzebne przy blokowaniu/odblokowywaniu przyciska Dalej

	public:
		Wizard(QWidget *parent = 0, const char *name = 0, bool modal = false);
		~Wizard();

	public slots:
		void wizardStart();
	protected:
		bool noNewAccount;

		/*  tworz± poszczegolne okna wizarda */
		void createWelcomePage();
		void createGGNumberSelect();
		void createGGCurrentNumberPage();
		void createGGNewNumberPage();
		void createLanguagePage();
		void createWWWOpionsPage();
		void createChatOpionsPage();
		void createSoundOptionsPage();
		void createGeneralOptionsPage();
		void createHintsOptionsPage();
		void createColorsPage();
		void createInfoPanelPage();
		void createQtStylePage();
		void createGreetingsPage();

		/* a tu sa okna */
		QVBox *welcomePage, *ggNumberSelect, *ggCurrentNumberPage, *ggNewNumberPage, *languagePage, *chatOptionsPage, *wwwOptionsPage,
			*soundOptionsPage, *generalOptionsPage, *greetingsPage, *hintsOptionsPage, *colorsPage, *qtStylePage, *infoPanelPage;
		QRadioButton *rb_haveNumber, *rb_dontHaveNumber;
		QLineEdit *l_ggNumber, *l_ggPassword, *l_ggNewPasssword, *l_ggNewPassswordRetyped, *l_email, *l_customBrowser;
		QCheckBox *c_importContacts, *c_waitForDelivery, *c_enterSendsMessage, *c_openOnNewMessage, *c_flashTitleOnNewMessage, *c_ignoreAnonyms,
			*c_logMessages, *c_logStatusChanges, *c_privateStatus, *c_showBlocked, *c_showBlocking, *c_startDocked, *c_enableSounds, *c_playWhilstChatting,
			*c_playWhenInvisible, *c_showInfoPanel, *c_showScrolls;
		QComboBox *cb_browser, *cb_browserOptions, *cb_hintsTheme, *cb_hintsType, *cb_colorTheme, *cb_iconTheme, *cb_qtTheme, *cb_panelTheme, *cb_soundModule;
		QLabel *previewHintStatusChangedToBusy, *previewHintConnectionError, *previewHintStatusChangedSyntax, *iconPreview, *iconPreview2, *iconPreview3, *iconPreview4, *moduleInfo;
		QString customHint, customPanel;
		KaduTextBrowser *infoPreview;
		QPushButton *registerAccount;

		/* zapisuj± konfiguracjê */
		void tryImport();
		void setChatOptions();
		void setSoundOptions();
		void setGeneralOptions();
		void setBrowser();
		void setHints();
		void setColorsAndIcons();
		void setPanelTheme();
		void setOldGaduAccount();

		/* funkcje pomocnicze */
		QString toDisplay(QString);
		QString toSave(QString);
		virtual void closeEvent(QCloseEvent *e);

	protected slots:
		void setLanguage(int);
		void setGaduAccount();
		void registeredAccount(bool, UinType);
		void previewHintsTheme(int);
		void previewHintsType(int);
		void previewColorTheme(int);
		void previewIconTheme(int);
		void previewQtTheme(int);
		void previewPanelTheme(int);
		void addScrolls(bool);

		void finishClicked();
		void nextClicked();
		void backClicked();
		void cancelClicked();

		void findAndSetWebBrowser(int selectedBrowser);
		void findAndSetBrowserOption(int selectedOption);
		void setSoundModule(int);
};

class WizardStarter : public QObject
{
	Q_OBJECT
	private:
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

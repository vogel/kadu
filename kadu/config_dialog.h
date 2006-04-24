#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <qasciidict.h>
#include <qdict.h>
#include <qhbox.h>
#include <qkeysequence.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qvbox.h>

#include "config_file.h"

class QApplication;
class QCheckBox;
class QComboBox;
class QGrid;
class QHButtonGroup;
class QHGroupBox;
class QLabel;
class QListBox;
class QListView;
class QScrollView;
class QSlider;
class QSpinBox;
class QTextEdit;
class QVGroupBox;
class QVButtonGroup;

/**
    Kontrolka umo¿liwiajaca wybranie skrótu klawiszowego
**/
class HotKey : public QLineEdit
{
	public:
		HotKey(QWidget *parent =0, const char* name =0);
		/**
		  Pobiera skrót klawiszowy
		**/
		QString getShortCutString() const;

		/**
		  Pobiera skrót klawiszowy
		**/
		QKeySequence getShortCut() const;

		/**
		  Ustawia skrót klawiszowy
		**/
		void setShortCut(const QString& shortcut);

		/**
		  Ustawia skrót klawiszowy
		**/
		void setShortCut(const QKeySequence& shortcut);

		/**
		 Zwraca skrót klawiszowy jaki zosta³ naci¶niêty przy zdarzeniu QKeyEvent
		**/
		static QString keyEventToString(QKeyEvent *e);

		/**
		 Pobiera skrót klawiszowy, z pliku konfiguracyjnego
		 z grupy "groupname", o polu "name"
		**/
		static QKeySequence shortCutFromFile(const QString& groupname, const QString &name);

		/**
		 Sprawdza czy skrót naci¶niêty przy zdarzeniu QKeyEvent zgadza siê
		 ze skrótem klawiszowym, z pliku konfiguracyjnego, o polu "name"
		**/
		static bool shortCut(QKeyEvent *e, const QString& groupname, const QString &name);
	protected:
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void keyReleaseEvent(QKeyEvent *e);
};
/**
    Kontrolka do wyboru koloru
**/
class ColorButton : public QPushButton
{
	Q_OBJECT
	private:
		QColor actualcolor;
	public:
		/**
			Konstruktor obiektu w którym ustawia sie kolor "color",
			który ma rodzica "parent" oraz nazywa sie "name"
		**/
		ColorButton(const QColor &color, QWidget *parent =0, const char* name =0);

		/**
			Pobiera kolor z przycisku
		**/
		const QColor &color() const;

	private slots:
		void onClick();
	public slots:
		/**
			Ustawia kolor na przycisku
		**/
		void setColor(const QColor &color);
	signals:
		/**
			Sygna³ jest emitowany przy zmianie koloru, przesy³a on nazwê komponentu oraz nowy kolor
		**/
		void changed(const char *name, const QColor& color);
};

class SelectFont : public QHBox
{
	Q_OBJECT
	public:
		SelectFont(const QString &text, const QFont &def_val, QWidget *parent = 0,  const char *name = 0, const char *tip = 0);
		const QFont &font() const;
	public slots:
		void setFont(const QFont &font);
	private slots:
		void onClick();
	signals:
		/**
			Sygna³ jest emitowany przy zmianie czcionki, przesy³a on nazwê komponentu oraz now± czcionkê
		**/
		void changed(const char *name, const QFont& font);
	private:
		QFont currentFont;
		QLineEdit *fontEdit;
};

class LayoutHelper;
class QResizeEvent;

class SelectPaths : public QHBox
{
	Q_OBJECT
	protected:
		QStringList releaseList;
		QListBox *pathListBox;
		QPushButton *pb_ok, *pb_cancel;
		QPushButton *pb_add, *pb_change, *pb_remove;
		QLineEdit *pathEdit;
		QPushButton *pb_choose;
		LayoutHelper *layoutHelper;

		virtual void resizeEvent(QResizeEvent *);

	protected slots:
		void okButton();
		void cancelButton();
		void addPath();
		void replacePath();
		void deletePath();
		void choosePath();
		void closeEvent(QCloseEvent *e);
		void keyPressEvent(QKeyEvent *e);

	public:
		SelectPaths(QWidget *parent =0, const char* name =0);
		~SelectPaths();
		/**
			Pobiera listê ¶cie¿ek katalogów
		**/
		const QStringList &getPathList() const;

	public slots:
		/**
			Ustawia ¶cie¿ki z katalogami
		**/
		void setPathList(const QStringList& list);

	signals:

		/**
			Sygna³ jest emitowany przy zamkniêciu okna przez "ok",
			przesy³a on zmienion± listê ¶cie¿ek katalogów
	    **/
		void changed(const QStringList& paths);
};

enum UserLevel {
	Beginner,
	Advanced,
	Expert
};

/**
	Okno z konfiguracj± Kadu
**/
class ConfigDialog : public QVBox
{
	Q_OBJECT

	private:

		enum RegisteredControlType
		{
			CONFIG_CHECKBOX,
			CONFIG_COLORBUTTON,
			CONFIG_COMBOBOX,
			CONFIG_GRID,
			CONFIG_HBOX,
			CONFIG_HGROUPBOX,
			CONFIG_HRADIOGROUP,
			CONFIG_HOTKEYEDIT,
			CONFIG_LABEL,
			CONFIG_LINEEDIT,
			CONFIG_LINEEDIT2,
			CONFIG_TEXTEDIT,
			CONFIG_LISTBOX,
			CONFIG_LISTVIEW,
			CONFIG_PUSHBUTTON,
			CONFIG_RADIOGROUP,
			CONFIG_SELECTPATHS,
			CONFIG_SLIDER,
			CONFIG_SPINBOX,
			CONFIG_TAB,
			CONFIG_VBOX,
			CONFIG_VGROUPBOX,
			CONFIG_VRADIOGROUP,
			CONFIG_SELECTFONT,
			CONFIG_NULL
		};

		struct ElementConnections
		{
			ElementConnections(const char *signal, const QObject *receiver, const char *slot);
			ElementConnections();
			const char *signal;
			const QObject* receiver;
			const char *slot;
			bool operator== (const ElementConnections& r) const;
		};
		struct RegisteredControl
		{
			RegisteredControlType type;
			RegisteredControl(RegisteredControlType t = CONFIG_NULL,
				const char *groupname = 0,
				const char *parent = 0,
				const char *caption = 0,
				const char *name = 0,
				const UserLevel userLevelRequired = Beginner);
			const char *parent;
			const char *name;
			const char *caption;
			const char *group;
			const char *entry;
			QString defaultS;
			const char *tip;

			//dodatkowe dane
			QValueList<QVariant> additionalParams;
			QValueListIterator<RegisteredControl> parentControl;
			QWidget *widget;

			//w przypadku kontrolek sk³adaj±cych siê z kilku mniejszych,
			// wska¼nik na rodzica, w przypadku pojedynczej kontrolki == widget
			QWidget *entireWidget;

			int nrOfControls;
			UserLevel userLevelRequired;

			ConfigFile* config;
			QValueList <ElementConnections> ConnectedSlots;
		};

		static QAsciiDict<QValueList <ElementConnections> > SlotsOnCreateTab;
		static QAsciiDict<QValueList <ElementConnections> > SlotsOnCloseTab;
		static QAsciiDict<QValueList <ElementConnections> > SlotsOnApplyTab;
		static QApplication* appHandle;

		static QAsciiDict<QValueList <RegisteredControl> > Tabs;
		static QValueList<const char *> TabNames;
		static QAsciiDict<int> TabSizes;
		static QDict<QSignal> tabChangesIn, tabChangesOut;//jakie¶ lepsze nazwy by siê przyda³y ;)


		QVGroupBox* box;
		/**
		    Dodaje kontrolkê do listy
		**/
		static bool addControl(const char *groupname, ConfigDialog::RegisteredControl& control);

		void createWidget(QValueListIterator <RegisteredControl> control);
		void createTabAndWidgets(const char *tab);

	public:
		/**
		    Konstruktor
		**/

		ConfigDialog(QApplication *application, QWidget *parent = 0, const char *name = 0);
		~ConfigDialog();
		/**
		    Pokazuje okno konfiguracji
		**/
		static void showConfigDialog(QApplication* application);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto¶æ kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------

			Domy¶ln± warto¶ci± kontrolki przy pierwszym uruchomieniu programu
			jest "defaultS".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
 		static void addCheckBox(
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const bool defaultS = false, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addCheckBox(ConfigFile* config,
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const bool defaultS = false, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Kolor przycisku jest ustawiony na "color"
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addColorButton(
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QColor& color, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addColorButton(ConfigFile *config,
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QColor& color, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addComboBox(
				const char *groupname, const char *parent, const char *caption,
			    const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".

			Warto¶æ kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Lista opcji do wyboru to "options", którym w pliku konfiguracyjnym odpowiadaj± "values".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addComboBox(
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QStringList &options, const QStringList &values,
				const QString &defaultS = QString::null, const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addComboBox(ConfigFile* config,
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QStringList &options, const QStringList &values,
				const QString &defaultS = QString::null, const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Ilo¶æ kolumn w kontrolce ustwiona jest na nrColumns
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addGrid(
				const char *groupname, const char *parent, const char *caption,
				const int nrColumns = 3, const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addHBox(
				const char *groupname, const char *parent, const char *caption,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addHGroupBox(
				const char *groupname, const char *parent, const char *caption,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);


		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto¶æ kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------

			Domy¶ln± warto¶ci± kontrolki przy pierwszym uruchomieniu programu
			jest "defaultS".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addHotKeyEdit(
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const QString& defaultS = QString::null, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addHotKeyEdit(ConfigFile* config,
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const QString& defaultS = QString::null, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".

			Warto¶æ kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Lista opcji do wyboru to "options", którym w pliku konfiguracyjnym odpowiadaj± "values".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addHRadioGroup(
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QStringList &options, const QStringList &values,
				const QString &defaultS = QString::null, const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addHRadioGroup(ConfigFile* config,
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QStringList &options, const QStringList &values,
				const QString &defaultS = QString::null, const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addLabel(
				const char *groupname, const char *parent, const char *caption,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto¶æ kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------

			Domy¶ln± warto¶ci± kontrolki przy pierwszym uruchomieniu programu
			jest "defaultS".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addLineEdit(
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const QString& defaultS = QString::null, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addLineEdit(ConfigFile* config,
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const QString& defaultS = QString::null, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);


		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Domy¶ln± warto¶ci± kontrolki ustawion jest na "defaultS".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addLineEdit2(
				const char *groupname, const char *parent, const char *caption,
			    const QString& defaultS = QString::null, const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto¶æ kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------

			Domy¶ln± warto¶ci± kontrolki przy pierwszym uruchomieniu programu
			jest "defaultS".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addTextEdit(
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const QString& defaultS = QString::null, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addTextEdit(ConfigFile* config,
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const QString& defaultS = QString::null, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);


		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addListBox(
				const char *groupname, const char *parent, const char *caption,
				const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addListView(
				const char *groupname, const char *parent, const char *caption,
				const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
			@param iconname nazwa ikony z zestawu lub ¶cie¿ka do pliku
		**/

		static void addPushButton(
				const char *groupname, const char *parent, const char *caption,
			    const QString& iconFilename = QString::null, const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".

			Warto¶æ kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Lista opcji do wyboru to "options", którym w pliku konfiguracyjnym odpowiadaj± "values".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addRadioGroup(
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QStringList &options, const QStringList &values,
				int strips, Orientation orientation,
				const QString &defaultS = QString::null, const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addRadioGroup(ConfigFile* config,
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QStringList &options, const QStringList &values,
				int strips, Orientation orientation,
				const QString &defaultS = QString::null, const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia tekst kontrolki na "caption".
			Warto¶æ kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------

			Domy¶ln± warto¶ci± kontrolki przy pierwszym uruchomieniu programu
			jest "defaultS".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addSelectFont(
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QString& defaultS = QString::null, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addSelectFont(ConfigFile *config,
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QString& defaultS = QString::null, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addSelectPaths(
				const char *groupname, const char *parent, const char *caption,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto¶æ kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Minimalna warto¶æ kontrolki ustawiona jest na "minValue", maksymalna
			na "maxValue", krok jest ustawiony na "pageStep"
			Domy¶ln± warto¶ci± kontrolki przy pierwszym uruchomieniu programu
			jest "value"
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addSlider(
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const int minValue = 0, const int maxValue = 100,
			    const int pageStep = 1, const int value = 50, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addSlider(ConfigFile* config,
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const int minValue = 0, const int maxValue = 100,
			    const int pageStep = 1, const int value = 50, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);


		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto¶æ kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Minimalna warto¶æ kontrolki ustawiona jest na "minValue", maksymalna
			na "maxValue", krok jest ustawiony na "step"
			Domy¶ln± warto¶ci± kontrolki przy pierwszym uruchomieniu programu
			jest "value"
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addSpinBox(
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const int minValue = 0, const int maxValue = 100,
				const int step = 1, const int value = 50, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addSpinBox(ConfigFile* config,
				const char *groupname, const char *parent, const char *caption,
			    const char *entry, const int minValue = 0, const int maxValue = 100,
				const int step = 1, const int value = 50, const char *tip = 0,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje zak³adkê o nazwie "caption"
		    @param iconname nazwa ikony z zestawu lub ¶cie¿ka do pliku
		**/
		static void addTab(const char *caption, const QString& iconFilename = QString::null,
			UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addVBox(
				const char *groupname, const char *parent, const char *caption,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addVGroupBox(
				const char *groupname, const char *parent, const char *caption,
				const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolkê do zak³adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".

			Warto¶æ kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Lista opcji do wyboru to "options", którym w pliku konfiguracyjnym odpowiadaj± "values".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addVRadioGroup(
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QStringList &options, const QStringList &values,
				const QString &defaultS = QString::null, const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);
		static void addVRadioGroup(ConfigFile* config,
				const char *groupname, const char *parent, const char *caption,
				const char *entry, const QStringList &options, const QStringList &values,
				const QString &defaultS = QString::null, const char *tip = 0, const char *name = 0,
				UserLevel userLevelRequired = Beginner);

		/**
		    Pobiera wska¼nik do kontrolki ButtonGroup(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QButtonGroup* getButtonGroup(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki CheckBox(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QCheckBox*   getCheckBox(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki ColorButton(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static ColorButton* getColorButton(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki ComboBox(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QComboBox*   getComboBox(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki Grid(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QGrid*       getGrid(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki HBox(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QHBox* 	    getHBox(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki HGroupBox(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QHGroupBox*  getHGroupBox(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki HButtonGroup(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QHButtonGroup* getHButtonGroup(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki HotKeyEdit(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static HotKey* 	    getHotKeyEdit(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki LineEdit(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QLineEdit*   getLineEdit(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki TextEdit(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QTextEdit*   getTextEdit(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki Label(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QLabel*      getLabel(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki ListBox(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QListBox*    getListBox(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki ListView(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QListView*   getListView(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki PushButton(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QPushButton* getPushButton(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki SelectFont(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static SelectFont* getSelectFont(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki SelectPaths(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static SelectPaths* getSelectPaths(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki Slider(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QSlider*     getSlider(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki SpinBox(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QSpinBox*    getSpinBox(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki VBox(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QVBox*  	    getVBox(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki VGroupBox(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QVGroupBox*  getVGroupBox(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki VButtonGroup(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QVButtonGroup* getVButtonGroup(const char *groupname, const char *caption, const char *name = 0);

		/**
		    W przypadku kontrolki zawieraj±cej kilka mniejszych (z czego jedn± z nich zwraca jedna z funkcji getXXX)
			zwraca wska¼nik do niej, w przeciwnym wypadku zwraca t± (pojedyncz±) kontrolkê
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
			UWAGA: nie wolno rzutowaæ zwracanego wska¼nika na ¿adn± klasê dziedzicz±c± pod QWidget!
			(np. addSpinBox dodaje QHBox zawieraj±c± QLabel i QSpinBox, getSpinBox() zwróci SpinBoksa, getEntireWidget QHBoksa())
		**/
		static QWidget* getEntireWidget(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Pobiera wska¼nik do kontrolki Widget(groupname, caption, name)
			je¿eli okno konfiguracji jest zamkniête zwraca NULL
		**/
		static QWidget* widget(const char *groupname, const char *caption, const char *name = 0);

		/**
			zwraca informacjê czy kontrolka (groupname, caption, name) jest w konfiguracji
			przy okazji przekazuj±c jej iterator je¿eli control != 0
		**/
		static bool controlExists(const char *groupname, const char *caption, const char *name = 0, QValueListConstIterator<RegisteredControl> *control = 0);

		/**
			zwraca informacjê czy kontrolka (groupname, caption, name) jest w konfiguracji
			przy okazji przekazuj±c jej iterator je¿eli control != 0
		**/
		static bool controlExists(const char *groupname, const char *caption, const char *name, QValueListIterator<RegisteredControl> *control);

		/**
		    Wyszukuje pozycjê zak³adki "name",
			zwraca przez parametry jej iterator oraz informacjê o znalezieniu je¿eli found != 0
		**/
		static void tab(const char *name, QValueListConstIterator<RegisteredControl> &ret, bool *found = 0);

		/**
		    Wyszukuje pozycjê zak³adki "name",
			zwraca przez parametry jej iterator oraz informacjê o znalezieniu je¿eli found != 0
		**/
		static void tab(const char *name, QValueListIterator<RegisteredControl> &ret, bool *found = 0);

		/**
			Pod³±cza slot "slot" obiektu "receiver"
			pod sygna³ "signal" kontrolki (groupname, caption, name)
		**/
		static void connectSlot(const char *groupname, const char *caption, const char* signal,
			    const QObject* receiver, const char* slot,const char *name = 0);

		/**
			Od³±cza slot "slot" obiektu "receiver"
			od sygna³u "signal" kontrolki (groupname, caption, name)
		**/
		static void disconnectSlot(const char *groupname, const char *caption, const char* signal,
			    const QObject* receiver, const char* slot,const char *name = 0);

		/**
			Rejestruje slot "slot" obiektu "receiver",
			który jest wywo³ywany przytworzeniu zakladki
		**/
		static void registerSlotOnCreateTab(const char *tab, const QObject* receiver, const char* slot);


		/**
			Wyrejestrowuje slot "slot" obiektu "receiver",
			który jest wywo³ywany przytworzeniu zakladki
		**/

		static void unregisterSlotOnCreateTab(const char *tab, const QObject* receiver, const char* slot);


		/**
			Rejestruje slot "slot" obiektu "receiver",
			który jest wywo³ywany przy zamykaniu okna konfiguracji
		**/
		static void registerSlotOnCloseTab(const char *tab, const QObject* receiver, const char* slot);

		/**
			Wyrejestrowuje slot "slot" obiektu "receiver",
			który jest wywo³ywany przy zamykaniu okna konfiguracji
		**/
		static void unregisterSlotOnCloseTab(const char *tab, const QObject* receiver, const char* slot);

		/**
			Rejestruje slot "slot" obiektu "receiver",
			który jest wywo³ywany przy zapisywaniu konfiguracji
		**/
		static void registerSlotOnApplyTab(const char *tab, const QObject* receiver, const char* slot);

		/**
			Wyrejestrowuje slot "slot" obiektu "receiver",
			który jest wywo³ywany przy zapisywaniu konfiguracji
		**/
		static void unregisterSlotOnApplyTab(const char *tab, const QObject* receiver, const char* slot);

		/**
			Rejestruje 2 sloty obiektu "receiver", które s±
			wywo³ywane przy zmianie zak³adki "name"
			slotIn - przy zmianie aktywnej zak³adki na "name" (mo¿e byæ == 0)
			slotOut - przy zmianie aktywnej zak³adki z "name" na inn± (mo¿e byæ == 0)
		**/
		static void registerSlotsOnTabChange(const char *name, const QObject *receiver, const char *slotIn, const char *slotOut);

		/**
			Wyrejestrowuje 2 sloty obiektu "receiver", które s±
			wywo³ywane przy zmianie zak³adki "name"
			slotIn - przy zmianie aktywnej zak³adki na "name" (mo¿e byæ == 0)
			slotOut - przy zmianie aktywnej zak³adki z "name" na inn± (mo¿e byæ == 0)
		**/
		static void unregisterSlotsOnTabChange(const char *name, const QObject *receiver, const char *slotIn, const char *slotOut);

		/**
		    Usuwa kontrolkê z zak³adki "groupname", o etykiecie "caption" i nazwie "name".
		**/
		static void removeControl(const char *groupname, const char *caption, const char *name = 0);

		/**
		    Usuwa zak³adkê o nazwie caption
		**/
		static bool removeTab(const char *caption);

		/**
			Zwraca prawdê je¿eli zak³adka znajduje siê w konfiguracji
		**/
		static bool tabExists(const char *caption);

		static bool dialogOpened();
		static void closeDialog();

		//u¿ywaæ tylko w wyj±tkowych sytuacjach
		static ConfigDialog *configdialog;

	protected:
		UserLevel userLevel;
		QListBox* listBox;
		QScrollView* view;
		QPushButton* okButton;
		QPushButton* applyButton;
		QPushButton* cancelButton;
		static QCString currentTab;
		virtual void keyPressEvent(QKeyEvent *e);
		void updateUserLevel(const QValueList<ConfigDialog::RegisteredControl> &tab);



	signals:

		void createTab();
		/**
		    Sygna³ jest emitowany przy otwieraniu okna
		**/
		void create();

		/**
		    Sygna³ jest emitowany przy zamykaniu okna
		**/
		void destroy();

		/**
		   Sygna³ jest emitowany przy zapisywaniu konfiguracji
		**/
		void apply();

	protected slots:
		/**
		    Zapisuje warto¶ci kontrolek do pliku
		**/

		void updateConfig();

		/**
		    Zapisuje warto¶ci kontrolek do pliku i zamyka okno
		**/

		void updateAndCloseConfig();

		/**
		    Zmienia zak³adkê na "name"
		**/
		void changeTab(const QString& name);

		/**
		    Ukrywa kontrolki o poziomie mniejszym od newUserLevel
		**/
		void changeUserLevel(int newUserLevel);
};
#endif

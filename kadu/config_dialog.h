#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfileinfo.h>
#include <qgrid.h>
#include <qhbox.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qrect.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qstringlist.h>
#include <qtabwidget.h>
#include <qtextedit.h>
#include <qvaluelist.h>
#include <qvbox.h>
#include <qvgroupbox.h>
#include <qvbuttongroup.h>
#include <qhbuttongroup.h>
#include <qvariant.h>

#include "config_file.h"
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
		QString getShortCutString();

		/**
		  Pobiera skrót klawiszowy
		**/
		QKeySequence getShortCut();

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
		QColor color();

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
		SelectFont(const QString &text, const QFont &def_val, QWidget *parent=0,  const char *name=0, const QString &tip="");
		QFont font();
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
		QStringList getPathList();

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


/**
	Okno z konfiguracj± Kadu
**/
class ConfigDialog : public QVBox {
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
			CONFIG_DELETED
		};

		struct ElementConnections
		{
			ElementConnections(QString signal, const QObject *receiver, QString slot);
			ElementConnections();
			QString signal;
			const QObject* receiver;
			QString slot;
			bool operator== (const ElementConnections& r) const;
		};

		struct RegisteredControl
		{
			RegisteredControlType type;
			RegisteredControl(RegisteredControlType t=CONFIG_DELETED,
				const QString &groupname=QString::null,
				const QString &parent=QString::null,
				const QString &caption=QString::null,
				const QString &name=QString::null);
			QString parent;
			QString name;
			QString caption;
			QString group;
			QString entry;
			QString defaultS;
			QString tip;
			
			//dodatkowe dane
			QValueList<QVariant> additionalParams;
			
			QWidget* widget;
			int nrOfControls;
			ConfigFile* config;
			QValueList <ElementConnections> ConnectedSlots;
		};

		static QValueList <RegisteredControl> RegisteredControls;
		static QValueList <ElementConnections> SlotsOnCreate;
		static QValueList <ElementConnections> SlotsOnClose;
		static QValueList <ElementConnections> SlotsOnApply;
		static QApplication* appHandle;
		
		/**
		    Dodaje kontrolkê do listy RegisteredControls 
		**/
		static int addControl(const QString& groupname,const RegisteredControl& control);
		/**
		    Uaktualnia liczbê kontrolek bêd±cych dzieæmi kontrolki "parent"
		**/
		static void increaseNrOfControls(const int startpos, const int endpos, const QString& parent);
		static void decreaseNrOfControls(int control);

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
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const bool defaultS=false, const QString &tip="",
				const QString& name="");
		static void addCheckBox(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const bool defaultS=false, const QString &tip="",
				const QString& name="");

		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".		
			Kolor przycisku jest ustawiony na "color"
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		
		static void addColorButton(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QColor& color, const QString& tip="",
				const QString& name="");
		static void addColorButton(ConfigFile *config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QColor& color, const QString& tip="",
				const QString& name="");

		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addComboBox(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& tip="", const QString& name="");

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
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="");
		static void addComboBox(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="");

		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Ilo¶æ kolumn w kontrolce ustwiona jest na nrColumns
			Nazwa kontrolki ustawiona jest na "name".
		**/		
			    
		static void addGrid(
				const QString& groupname, const QString& parent, const QString& caption,
				const int nrColumns=3, const QString& name="");

		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addHBox(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="");

		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addHGroupBox(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="");

			    
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
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="");
		static void addHotKeyEdit(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="");

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
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="");
		static void addHRadioGroup(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="");

		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addLabel(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="");
			    
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
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="");
		static void addLineEdit(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="");


		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Domy¶ln± warto¶ci± kontrolki ustawion jest na "defaultS".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addLineEdit2(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& defaultS="", const QString& tip="", const QString& name="");
			    
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
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="");
		static void addTextEdit(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="");


		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addListBox(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& tip= "", const QString& name="");

		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addListView(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& tip= "", const QString& name="");
			    
		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied¼ kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
			@param iconname nazwa ikony z zestawu lub ¶cie¿ka do pliku
		**/		

		static void addPushButton(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& iconFileName="", const QString& tip="", const QString& name="");

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
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				int strips, Orientation orientation,
				const QString &defaultS="", const QString& tip="", const QString& name="");
		static void addRadioGroup(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				int strips, Orientation orientation,
				const QString &defaultS="", const QString& tip="", const QString& name="");

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
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& entry, const QString& defaultS="", const QString &tip="",
				const QString& name="");
		static void addSelectFont(ConfigFile *config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& entry, const QString& defaultS="", const QString &tip="",
				const QString& name="");

		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addSelectPaths(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="");

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
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const int minValue=0, const int maxValue=100,
			    const int pageStep=1, const int value=50, const QString& tip="",
				const QString& name="");
		static void addSlider(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const int minValue=0, const int maxValue=100,
			    const int pageStep=1, const int value=50, const QString& tip="",
				const QString& name="");
		

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
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const int minValue=0, const int maxValue=100,
				const int step=1, const int value=50, const QString& tip="",
				const QString& name="");
		static void addSpinBox(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const int minValue=0, const int maxValue=100,
				const int step=1, const int value=50, const QString& tip="",
				const QString& name="");

		/**
		    Dodaje zak³adkê o nazwie "caption"
		    @param iconname nazwa ikony z zestawu lub ¶cie¿ka do pliku
		**/
		static void addTab(const QString& caption, const QString& iconFileName="");
		
		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addVBox(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="");

		/**
		    Dodaje kontrolkê do zak³adki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addVGroupBox(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="");

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
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="");
		static void addVRadioGroup(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="");

		/**
			Pod³±cza slot "slot" obiektu "receiver"
			pod sygna³ "signal" kontrolki (groupname, caption, name) 
		**/
		static void connectSlot(const QString& groupname, const QString& caption, const char* signal,
			    const QObject* receiver, const char* slot,const QString& name="");

		/**
			Od³±cza slot "slot" obiektu "receiver"
			od sygna³u "signal" kontrolki (groupname, caption, name) 
		**/
		static void disconnectSlot(const QString& groupname, const QString& caption, const char* signal,
			    const QObject* receiver, const char* slot,const QString& name="");

		/**
			Rejestruje slot "slot" obiektu "receiver",
			który jest wywo³ywany przy otwieraniu okna konfiguracji
		**/
		static void registerSlotOnCreate(const QObject* receiver, const char* slot);

		/**
			Wyrejestrowuje slot "slot" obiektu "receiver",
			który jest wywo³ywany przy otwieraniu okna konfiguracji
		**/
		static void unregisterSlotOnCreate(const QObject* receiver, const char* slot);

		/**
			Rejestruje slot "slot" obiektu "receiver",
			który jest wywo³ywany przy zamykaniu okna konfiguracji
		**/
		static void registerSlotOnClose(const QObject* receiver, const char* slot);
		
		/**
			Wyrejestrowuje slot "slot" obiektu "receiver",
			który jest wywo³ywany przy zamykaniu okna konfiguracji
		**/
		static void unregisterSlotOnClose(const QObject* receiver, const char* slot);

		/**
			Rejestruje slot "slot" obiektu "receiver",
			który jest wywo³ywany przy zapisywaniu konfiguracji
		**/
		static void registerSlotOnApply(const QObject* receiver, const char* slot);
		
		/**
			Wyrejestrowuje slot "slot" obiektu "receiver",
			który jest wywo³ywany przy zapisywaniu konfiguracji
		**/
		static void unregisterSlotOnApply(const QObject* receiver, const char* slot);

		/**
		    Usuwa kontrolkê z zak³adki "groupname", o etykiecie "caption" i nazwie "name".
		**/
		static void removeControl(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Usuwa zak³adkê o nazwie caption
		**/
		static void removeTab(const QString& caption);

		/**
		    Pobiera wska¼nik do kontrolki ButtonGroup(groupname, caption, name)
		**/
		static QButtonGroup* getButtonGroup(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki CheckBox(groupname, caption, name)
		**/
		static QCheckBox*   getCheckBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki ColorButton(groupname, caption, name)
		**/
		static ColorButton* getColorButton(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki ComboBox(groupname, caption, name)
		**/
		static QComboBox*   getComboBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki Grid(groupname, caption, name)
		**/
		static QGrid*       getGrid(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki HBox(groupname, caption, name)
		**/
		static QHBox* 	    getHBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki HGroupBox(groupname, caption, name)
		**/
		static QHGroupBox*  getHGroupBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki HButtonGroup(groupname, caption, name)
		**/
		static QHButtonGroup* getHButtonGroup(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki HotKeyEdit(groupname, caption, name)
		**/
		static HotKey* 	    getHotKeyEdit(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki LineEdit(groupname, caption, name)
		**/
		static QLineEdit*   getLineEdit(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki TextEdit(groupname, caption, name)
		**/
		static QTextEdit*   getTextEdit(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki Label(groupname, caption, name)
		**/
		static QLabel*      getLabel(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki ListBox(groupname, caption, name)
		**/
		static QListBox*    getListBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki ListView(groupname, caption, name)
		**/
		static QListView*   getListView(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki PushButton(groupname, caption, name)
		**/
		static QPushButton* getPushButton(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki SelectFont(groupname, caption, name)
		**/
		static SelectFont* getSelectFont(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki SelectPaths(groupname, caption, name)
		**/
		static SelectPaths* getSelectPaths(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki Slider(groupname, caption, name)
		**/
		static QSlider*     getSlider(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki SpinBox(groupname, caption, name)
		**/
		static QSpinBox*    getSpinBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki VBox(groupname, caption, name)
		**/
		static QVBox*  	    getVBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki VGroupBox(groupname, caption, name)
		**/
		static QVGroupBox*  getVGroupBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki VButtonGroup(groupname, caption, name)
		**/
		static QVButtonGroup* getVButtonGroup(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wska¼nik do kontrolki Widget(groupname, caption, name)
		**/
		static QWidget*     getWidget(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Zwraca pozycjê kontrolki Widget(groupname, caption, name) w li¶cie RegisteredControls
		    je¶li nie istnieje taka kontrolka to zwracana jest warto¶æ -1
		**/
		static int existControl(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Wyszukuje pozycjê dowolnej zak³adki w li¶cie RegisteredControls 
			poczynaj±c od pozycji "startpos"
			
		    Je¶li nie znajdzie zak³adki to zwraca warto¶æ "-1"
		**/
		static int findNextTab(int startpos);
		/**
		    Wyszukuje pozycjê zak³adki "groupname" w li¶cie RegisteredControls 
			poczynaj±c od pozycji "startpos"

		    Je¶li nie znajdzie zak³adki to zwraca warto¶æ "-1"
		**/
		static int findTab(const QString& groupname, int startpos=0);
		/**
		    Wyszukuje pozycjê poprzedniej zak³adki w li¶cie RegisteredControls 
			poczynaj±c od pozycji "startpos"

		    Je¶li nie znajdzie zak³adki to zwraca warto¶æ "-1"
		**/
		static int findPreviousTab(int startpos);
		static bool dialogOpened();
		static void closeDialog();
		
		//u¿ywaæ tylko w wyj±tkowych sytuacjach
		static ConfigDialog *configdialog;
		
		//
	protected:
		QListBox* listBox;
		QScrollView* view;
		QPushButton* okButton;
		QPushButton* applyButton;
		QPushButton* cancelButton;
		static QString acttab;

	signals:
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
};
#endif

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
#include <qvaluelist.h>
#include <qvbox.h>
#include <qvgroupbox.h>

/**
    Kontrolka umozliwiajaca wybranie skrotu klawiszowego
**/
class HotKey : public QLineEdit
{
	public:
		HotKey::HotKey(QWidget *parent =0, const char* name =0);
		/**
		  Pobiera skrot klawiszowy
		**/
		QString getShortCutString();

		/**
		  Pobiera skrot klawiszowy
		**/
		QKeySequence getShortCut();

		/**
		  Ustawia skrot klawiszowy
		**/
		void setShortCut(const QString& shortcut);

		/**
		  Ustawia skrot klawiszowy
		**/
		void setShortCut(const QKeySequence& shortcut);

		/**
		 Zwraca skrot klawiszowy jaki zostal nacisniety przy zdarzeniu QKeyEvent
		**/
		static QString keyEventToString(QKeyEvent *e);

		/**
		 Pobiera skrot klawiszowy, z pliku konfiguracyjnego 
		 z grupy "groupname", o polu "name"
		**/
		static QKeySequence shortCutFromFile(const QString& groupname, const QString &name);

		/**
		 Sprawdza czy skrot nacisniety przy zdarzeniu QKeyEvent zgadza sie 
		 ze skrotem klawiszowym, z pliku konfiguracyjnego, o polu "name"
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
		Konstruktor obiektu w ktorym ustawia sie kolor "color",
		ktory ma rodzica "parent" oraz nazywa sie "name"
	    **/
	       ColorButton(const QColor &color, QWidget *parent =0, const char* name =0);

	    /**
		Pobiera kolor z przycisku
	    **/
	       QColor color();

	    /**
		Ustawia kolor na przycisku
	    **/
	       void setColor(const QColor &color);
	private slots:
	       void onClick();
	signals:
	    /**
		Sygnal jest emitowany przy zmianie koloru, przesyla on zmieniony kolor
	    **/
		void changed(const QColor& color);


};

class SelectPaths : public QDialog
{	
	Q_OBJECT
	protected:
	      QStringList releaseList;
	      QListBox *pathListBox;
	      QPushButton *ok, *cancel;
	      QPushButton *add, *change, *remove;
	      QLineEdit *pathEdit;
	      QPushButton *findPath;
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
	        Pobiera liste sciezek katalogow
	      **/
	      QStringList getPathList();

	      /**
		Ustawia sciezki z katalogami
	      **/
	      void setPathList(QStringList& list);

	signals:

	    /**
		Sygnal jest emitowany przy zamknieciu okna przez "ok",
		przesyla on zmieniony zmieniona liste sciezek katalogow
	    **/
	    void changed(const QStringList& paths);
};


/**
	Okno z konfiguracj± Kadu
**/
class ConfigDialog : public QDialog	{
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
			CONFIG_HOTKEYEDIT,
			CONFIG_LABEL,
			CONFIG_LINEEDIT,
			CONFIG_LINEEDIT2,
			CONFIG_LISTBOX,
			CONFIG_LISTVIEW,
			CONFIG_PUSHBUTTON,
			CONFIG_SELECTPATHS,
			CONFIG_SLIDER,
			CONFIG_SPINBOX,
			CONFIG_TAB,
			CONFIG_VBOX,
			CONFIG_VGROUPBOX
		};

		struct ElementConnections
		{
			QString signal;
			QObject* receiver;
			QString slot;
			bool operator== (const ElementConnections& r) const;
		};

		struct RegisteredControl
		{
			RegisteredControlType type;
			QString parent;
			QString name;
			QString caption;
			QString group;
			QString entry;
			QString defaultS;
			QString tip;
			QWidget* widget;
			int nrOfControls;
			QValueList <ElementConnections> ConnectedSlots;
		};

		static QValueList <RegisteredControl> RegisteredControls;
		static QValueList <ElementConnections> SlotsOnCreate;
		static QValueList <ElementConnections> SlotsOnDestroy;
		static QApplication* appHandle;
		
		/**
		    Wyszukuje pozycje dowonlnej zakladki w liscie RegisteredControls 
			poczynajac od pozycji "startpos"
			
		    Jesli nie znajdzie zakladki to zwraca wartosc "-1"
		**/
		static int findTab(const int startpos);
		/**
		    Wyszukuje pozycje zakladki "groupname" w liscie RegisteredControls 
			poczynajac od pozycji "startpos"

		    Jesli nie znajdzie zakladki to zwraca wartosc "-1"
		**/
		static int findTab(const QString& groupname, const int startpos=0);
		/**
		    Wyszukuje pozycje poprzedniej zakladki w liscie RegisteredControls 
			poczynajac od pozycji "startpos"

		    Jesli nie znajdzie zakladki to zwraca wartosc "-1"
		**/
		static int findPreviousTab(const int startpos=0);
		/**
		    Dodaje kontrolke do listy RegisteredControls 
		**/
		static int addControl(const QString& groupname,const RegisteredControl& control);
		/**
		    Uaktualnia liczbe kontrolek bedacych dziecmi kontrolki "parent"
		**/
		static void updateNrOfControls(const int startpos, const int endpos, const QString& parent);

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
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Wartosc kontrolki jest zapisana do pliku konfiguracyjnego w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			
			Domyslna wartoscia kontrolki przy pierwszym uruchomieniu programu 
			jest "defaultS".
			Podpowiedz kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		
 		static void addCheckBox(const QString& groupname, 
		    	    const QString& parent, const QString& caption,
			    const QString& entry, const bool defaultS=false, const QString &tip="", const QString& name="");
			    			    
		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".		
			Kolor przycisku jest ustawiony na "color"
			Podpowiedz kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		
			    
		static void addColorButton(const QString& groupname,
			    const QString& parent, const QString& caption,
			    const QColor& color, const QString& tip="", const QString& name="");
		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowiedz kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		
			    
		static void addComboBox(const QString& groupname, 
			    const QString& parent, const QString& caption,
			    const QString& tip="", const QString& name="");

		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Ilosc kolumn w kontrolce ustwiona jest na nrColumns
			Nazwa kontrolki ustawiona jest na "name".
		**/		
			    
		static void addGrid(const QString& groupname, 
			    const QString& parent, const QString& caption, const int nrColumns=3, const QString& name="");

		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addHBox(const QString& groupname, 
			    const QString& parent, const QString& caption, const QString& name="");

		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addHGroupBox(const QString& groupname, 
			    const QString& parent, const QString& caption, const QString& name="");
			    
		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Wartosc kontrolki jest zapisana do pliku konfiguracyjnego w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			
			Domyslna wartoscia kontrolki przy pierwszym uruchomieniu programu 
			jest "defaultS".
			Podpowiedz kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addHotKeyEdit(const QString& groupname, 
			    const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="", const QString& name="");
			    
		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addLabel(const QString& groupname, 
			    const QString& parent, const QString& caption, const QString& name="");
			    
		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Wartosc kontrolki jest zapisana do pliku konfiguracyjnego w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			
			Domyslna wartoscia kontrolki przy pierwszym uruchomieniu programu 
			jest "defaultS".
			Podpowiedz kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addLineEdit(const QString& groupname,
			    const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",const QString& name="");

		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Domyslna wartoscia kontrolki ustawion jest na "defaultS".
			Podpowiedz kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addLineEdit2(const QString& groupname,
			    const QString& parent, const QString& caption,
			    const QString& defaultS="", const QString& tip="",const QString& name="");
			    
		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowiedz kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addListBox(const QString& groupname, 
			    const QString& parent, const QString& caption, const QString& tip= "", const QString& name="");

		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowiedz kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addListView(const QString& groupname, 
			    const QString& parent, const QString& caption, const QString& tip= "", const QString& name="");
			    
		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowiedz kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
			@param iconname nazwa ikony z zestawu lub sciezka do pliku
		**/		

		static void addPushButton(const QString& groupname, 
			    const QString& parent, const QString& caption,
			    const QString& iconFileName="", const QString& tip="", const QString& name="");

		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addSelectPaths(const QString& groupname, 
			    const QString& parent, const QString& caption, const QString& name="");
			    

		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Wartosc kontrolki jest zapisana do pliku konfiguracyjnego w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Minimalna wartosc kontrolki ustawiona jest na "minValue", maxymalna 
			na "maxValue", krok jest ustawiony na "pageStep"
			Domyslna wartoscia kontrolki przy pierwszym uruchomieniu programu 
			jest "value"
			Podpowiedz kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addSlider(const QString& groupname, 
			    const QString& parent, const QString& caption,
			    const QString& entry,
			    const int minValue=0, const int maxValue=100,
			    const int pageStep=1, const int value=50, const QString& tip="", const QString& name="");

		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Wartosc kontrolki jest zapisana do pliku konfiguracyjnego w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Minimalna wartosc kontrolki ustawiona jest na "minValue", maxymalna 
			na "maxValue", krok jest ustawiony na "step"
			Domyslna wartoscia kontrolki przy pierwszym uruchomieniu programu 
			jest "value"
			Podpowiedz kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addSpinBox(const QString& groupname, 
			    const QString& parent, const QString& caption,
			    const QString& entry,
			    const int minValue=0, const int maxValue=100, const int step=1,
			    const int value=50, const QString& tip="", const QString& name="");

		/**
		    Usuwa kontrolke (groupname, caption, name)
		**/
		static void removeControl(const QString& groupname, const QString& caption,const QString& name="");

		/**
		    Dodaje zakladke o nazwie "caption"
		    @param iconname nazwa ikony z zestawu lub sciezka do pliku
		**/
		static void addTab(const QString& caption, const QString& iconFileName="");
		
		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addVBox(const QString& groupname, 
			    const QString& parent, const QString& caption, const QString& name="");

		/**
		    Dodaje kontrolke do zakladki "groupname", 
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/		

		static void addVGroupBox(const QString& groupname,
			    const QString& parent, const QString& caption, const QString& name="");

		/**
			Podlacza slot "slot" obiektu "receiver"
			pod sygnal "signal" kontrolki (groupname, caption, name) 
		**/
		static void connectSlot(const QString& groupname, const QString& caption, const char* signal,
			    const QObject* receiver, const char* slot,const QString& name="");

		/**
			Odlacza slot "slot" obiektu "receiver"
			od sygnalu "signal" kontrolki (groupname, caption, name) 
		**/
		static void disconnectSlot(const QString& groupname, const QString& caption, const char* signal,
			    const QObject* receiver, const char* slot,const QString& name="");

		/**
			Rejestruje slot "slot" obiektu "receiver",
			ktory jest wywolywany przy otwieraniu okna konfiguracji
		**/
		static void registerSlotOnCreate(const QObject* receiver, const char* slot);

		/**
			Wyrejestrowuje slot "slot" obiektu "receiver",
			ktory jest wywolywany przy otwieraniu okna konfiguracji
		**/
		static void unregisterSlotOnCreate(const QObject* receiver, const char* slot);

		/**
			Rejestruje slot "slot" obiektu "receiver",
			ktory jest wywolywany przy zamykaniu okna konfiguracji
		**/
		static void registerSlotOnDestroy(const QObject* receiver, const char* slot);
		
		/**
			Wyrejestrowuje slot "slot" obiektu "receiver",
			ktory jest wywolywany przy zamykaniu okna konfiguracji
		**/
		static void unregisterSlotOnDestroy(const QObject* receiver, const char* slot);

		/**
		    Pobiera wskaznik do kontrolki CheckBox(groupname, caption, name)
		**/
		static QCheckBox*   getCheckBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki ColorButton(groupname, caption, name)
		**/
		static ColorButton* getColorButton(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki ComboBox(groupname, caption, name)
		**/
		static QComboBox*   getComboBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki Grid(groupname, caption, name)
		**/
		static QGrid*       getGrid(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki HBox(groupname, caption, name)
		**/
		static QHBox* 	    getHBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki HGroupBox(groupname, caption, name)
		**/
		static QHGroupBox*  getHGroupBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki HotKeyEdit(groupname, caption, name)
		**/
		static HotKey* 	    getHotKeyEdit(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki LineEdit(groupname, caption, name)
		**/
		static QLineEdit*   getLineEdit(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki Label(groupname, caption, name)
		**/
		static QLabel*      getLabel(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki ListBox(groupname, caption, name)
		**/
		static QListBox*    getListBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki ListView(groupname, caption, name)
		**/
		static QListView*   getListView(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki PushButton(groupname, caption, name)
		**/
		static QPushButton* getPushButton(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki SelectPaths(groupname, caption, name)
		**/
		static SelectPaths* getSelectPaths(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki Slider(groupname, caption, name)
		**/
		static QSlider*     getSlider(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki SpinBox(groupname, caption, name)
		**/
		static QSpinBox*    getSpinBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki VBox(groupname, caption, name)
		**/
		static QVBox*  	    getVBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki VGroupBox(groupname, caption, name)
		**/
		static QVGroupBox*  getVGroupBox(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Pobiera wskaznik do kontrolki Widget(groupname, caption, name)
		**/
		static QWidget*     getWidget(const QString& groupname, const QString& caption, const QString& name="");
		/**
		    Zwraca pozycje kontrolki Widget(groupname, caption, name) w liscie RegisteredControls
		    jesli nie istnieje taka kontrolka to zwracana jest wartosc -1
		**/
		static int existControl(const QString& groupname, const QString& caption, const QString& name="");
		
		//
	protected:
		QListBox* listBox;
		QScrollView* view;
		QPushButton* okButton;
		QPushButton* cancelButton;
		static ConfigDialog *configdialog;
		static QString acttab;

	signals:
		/**
		    Sygnal jest emitowany przy otwieraniu okna
		**/
		void create();
		/**
		    Sygnal jest emitowany przy zamykaniu okna
		**/
		void destroy();

	protected slots:
		/**
		    Zapisuje wartosci kontrolek do pliku i zamyka okno
		**/

		void updateConfig();
		/**
		    Zmienia zakladke na "name"
		**/
		void changeTab(const QString& name);
};
#endif

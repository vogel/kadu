#ifndef KADU_CONFIGURATION_WINDOW_H
#define KADU_CONFIGURATION_WINDOW_H

#include <QtGui/QTabWidget>
#include <QtXml/QDomNode>

#include "color_button.h"
#include "hot_key.h"
#include "path_list_edit.h"
#include "select_font.h"
#include "syntax_editor.h"
#include "userbox.h"

class ConfigGroupBox;
class ConfigLineEdit;
class ConfigTab;
class ConfigurationWindow;
class ConfigWidget;

class QGridLayout;
class QGroupBox;
class QListWidgetItem;
class QWidget;

class ConfigSection : public QObject
{
	Q_OBJECT

	QString name;
	ConfigurationWindow *configurationWindow;
	QString pixmap;

	QListWidgetItem *listWidgetItem;
	QMap<QString, ConfigTab *> configTabs;

	QTabWidget *mainWidget;

	ConfigTab *configTab(const QString &name, bool create = true);
	bool activated;

private slots:
	void iconThemeChanged();

public:
	ConfigSection(const QString &name, ConfigurationWindow *configurationWindow, QListWidgetItem *listWidgetItem, QWidget *parentConfigGroupBoxWidget,
		const QString &pixmap);
	~ConfigSection();

	void activate();

	void show() { mainWidget->show(); }
	void hide() { mainWidget->hide(); }

	ConfigGroupBox * configGroupBox(const QString &tab, const QString &groupBox, bool create = true);

	void removedConfigTab(const QString &configTabName);

};

/**
	@class ConfigGroupBox
	@author Vogel
	@short GroupBox w oknie konfiguracji

	GroupBox w oknie konfiguracji definiowany jest przez nast�puj�cy tag:
	<code>
		&lt;group-box caption="tytu�" id="id"&gr;
			...
		&lt;/group-box&gt;
	</code>

	GroupBox mo�e znajdowa� si� tylko wewn�trz tagu tab. W jego wn�trzu
	mog� zawiera� si� dowolne tagi widget�w konfigruacyjnych.

	Dodatkowo, GroupBox'a mo�na stworzy� (lub, je�eli istnieje, uzyska�)
	wywo�uj�c funkcj� configGroupBox(section, tab, groupBox) z okna konfiguracyjnego.
	Do tak uzyskanego GroupBox'a mo�na dodawa� dowolne widgety (@see addWidget,
	@see addWidgets).
 **/
class KADUAPI ConfigGroupBox
{
	QString name;
	ConfigTab *configTab;

	QGroupBox *groupBox;
	QWidget *container;
	QGridLayout *gridLayout;

public:
	ConfigGroupBox(const QString &name, ConfigTab *configTab, QGroupBox *groupBox);
	~ConfigGroupBox();

	QWidget * widget() { return container; }

	/**
		Dodaje widget do GroupBoxa.
		@param widget dodawany widget (nie mo�e by� NULL)
		@param fullSpace kiedy true, dodany widget zajmuje ca�� szeroko�� GroupBox'a,
			w przeciwnym wypadku tylko praw� jego cz���
	 **/
	void addWidget(QWidget *widget, bool fullSpace = false);
	/**
		Dodaje 2 widget do GroupBoxa, jeden po lewej stronie, drugi po prawej.
		@param widget1 widget dodawany z lewej strony (zazwyczaj etykieta)
		@param widget2 widget dodawany z prawej strony
	 **/
	void addWidgets(QWidget *widget1, QWidget *widget2);

	/**
		Zwraca true, gdy GroupBox nie posiada w sobie �adnych widget�w.
		@return true, gdy GroupBox nie posiada w sobie �adnych widget�w
	 **/
	bool empty();

};

/**
	@class ConfigurationWindow
	@author Vogel
	@short Widget okna konfigruacyjnego.

	Okno konfiguracyjne tego typu zawiera widgety konfiuracyjne podzielone wg. 3 stopnioej hierarchii.
	Pierwszym stopniem s� sekcje reprezentowane przez ListWidget'a z lewej strony okna (zawieraj�cego
	ikony i opis tekstowy). Okno zawieraj�ce tylko jedn� sekcj� nie wy�wietla ListWidget'a.
	Drugim stopniem s� karty reprezentowane przez TabWidget'y, trzecim - grupy opisane przez GroupBox'y.

	Okno konfiguracyjne identyfikuje si� przez jego nazw� podan� w konstruktorze
	(dzi�ki tej nazwie ka�de okno mo�e osobno zapami�ta� swoj� pozycj� i ostatnio
	otwartej karty).

	Okna mog� teoretycznie zawiera� dowolne widgety. Ka�dy z nich, kt�ry dodatkowo
	dziedziczy z klasy ConfigWidget, traktowany jest w specjalny spos�b. Jego
	metody loadConfiguration i saveConfiguration s� wywo�ywane automatycznie
	przy otwieraniu okna i przy zapisywaniu konfiguracji, dzi�ki czemu nie jest
	potrzebne �adne 'r�czne' podpinanie si� do tych akcji.

	W momencie zapisania konfiguracji wszystkie obiekty w programie b�d�ce instancajmi
	klasy @see ConfigurationAwareObject zostan� o tym poinformowane i b�d�
	mog�y zaktualizowa� sw�j stan.

	Widgety w oknie mog� by� tworzone na 2 sposoby. Pierwszym z nich jest
	pobranie GroupBoxa za pomoc� funkcji @see configGroupBox i dodawanie
	do niego widget�w za pomoc� jego funkcji addWidget i addWidgets.
	Drug� jest stworzenie plik�w XML *.ui, kt�re s� wczytywane i usuwane dynamicznie
	z pomoc� metod @see appendUiFile i @see removeUiFile.

	W tym drugim przypadku stosuje si� pliki *.ui o nast�puj�cej strukturze:

	&lt;configuration-ui&gt;
		&lt;section caption="tytu�" icon="nazwa_ikony"&gt;
			&lt;tab caption="tytu�"&gt;
				&lt;group-box caption="tytu�" id="id"&gt;
					&lt;widget ... /&gt;
				&lt;/group-box&gt;
			&lt;/tab&gt;
		&lt;/section&gt;
	&lt;/configuration-ui&gt;

	Elementy zawieraj�ce atrybut id (nie wymagany) - czyli group-box i dowolny widget
	mog� zosta� pobrane przez aplikacje za pomoc� metody @see widgetById.
	Widgety z modu��w powinny posiada� id w postaci: nazwaModu�u/nazwaId.

	Atrybut catpion jest wymagany. Mo�liwe tagi widget s� opisane w dokumentacji
	klas Config* (np.: ConfigComboBox).
 **/
class KADUAPI ConfigurationWindow : public QWidget
{
	Q_OBJECT

	QString Name;

	QWidget *left;
	QMap<QString, ConfigSection *> configSections;
	ConfigSection *currentSection;

	QMap<QString, QWidget *> widgets;

	QListWidget *sectionsListWidget;
	QWidget *container;

	void loadConfiguration(QObject *object);
	void saveConfiguration(QObject *object);

	ConfigSection *configSection(const QString &name);
	ConfigSection *configSection(const QString &icon, const QString &name, bool create = true);

	QList<ConfigWidget *> processUiFile(const QString &fileName, bool append = true);
	QList<ConfigWidget *> processUiSectionFromDom(QDomNode sectionNode, bool append = true);
	QList<ConfigWidget *> processUiTabFromDom(QDomNode tabNode, const QString &iconName, const QString &sectionName, bool append = true);
	QList<ConfigWidget *> processUiGroupBoxFromDom(QDomNode groupBoxNode, const QString &sectionName, const QString &tabName, bool append = true);
	ConfigWidget * appendUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox);
	void removeUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox);

private slots:
	void updateAndCloseConfig();
	void updateConfig();

	void changeSection(const QString &newSectionName);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	/**
		Tworzy okno konfiguracyjne o danej nazwie. Nazwa wykorzystywana
		jest przy zapami�tywaniu pozycji okna oraz jego ostatnio
		otwartej karty.
	 **/
	ConfigurationWindow(const QString &name, const QString &caption);
	virtual ~ConfigurationWindow();

	QString name() { return Name; }

	/**
		Pobiera GroupBox'a zawartego w nast�puj�cej �cie�ce XPath:
		/section[@caption=SECTION]/tab[@caption=TAB]/group-box[@caption=GROUPBOX]

		Je�eli nie istnieje taki group-box to zostanie on stworzony, gdy create == true.
		Dla crate == false zostanie zwr�cony NULL.
	 **/
	ConfigGroupBox * configGroupBox(const QString &section, const QString &tab, const QString &groupBox, bool create = true);

	/**
		Je�eli okno jest ukryte wczytuje warto�ci element�w z pliku
		konfiguracyjnego i pokazuje okno.
	 **/
	virtual void show();

	/**
		Dodaje do okna konfiguracyjnego widgety wczytane z pliku fileName.
		Gdy load == true wczytuje ich warto�ci z pliku konfiguracyjnego. W przeciwnym
		wypadku wczytanie warto�ci nast�pi po wywo�aniu metody @see show.
	 **/
	QList<ConfigWidget *> appendUiFile(const QString &fileName, bool load = true);
	/**
		Usuwa kontrolki wczytane z danego pliku *.ui.
	 **/
	void removeUiFile(const QString &fileName);

	/**
		Zwraca element opisane id="id" we wczytanych plikach *.ui. Mo�liwe
		dowolne widgety konfiguracyjne i elementy group-box.
	 **/
	QWidget *widgetById(const QString &id);

	// TODO: make private or sth
	void removedConfigSection(const QString &sectionName);

signals:
	/**
		Sygna� emitowany po naci�ni�ciu Ok lub Apply ale przed zapisaniem
		warto�ci do pliku konfiguracyjnego. Nietypowe widgety konfiguracyjne
		powinny si� podpi�� pod ten sygna� i po jego uzyskaniu zapisa�
		nowe warto�ci do pliku.
	 **/
	void configurationWindowApplied();

};

#endif // KADU_CONFIGURATION_WINDOW_H

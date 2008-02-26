#ifndef KADU_CONFIGURATION_WINDOW_H
#define KADU_CONFIGURATION_WINDOW_H

#define QT3_SUPPORT
#include <qglobal.h>

#include <qdom.h>
#include <q3groupbox.h>
#include <qtabwidget.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3ValueList>
#include <QKeyEvent>

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

class Q3GridLayout;
class Q3GroupBox;
class Q3VBox;

class ConfigSection : public QObject
{
	Q_OBJECT

	QString name;
	ConfigurationWindow *configurationWindow;
	QString pixmap;

	Q3ListBoxItem *listBoxItem;
	QMap<QString, ConfigTab *> configTabs;

	QTabWidget *mainWidget;

	ConfigTab *configTab(const QString &name, bool create = true);
	bool activated;

private slots:
	void iconThemeChanged();

public:
	ConfigSection(const QString &name, ConfigurationWindow *configurationWindow, Q3ListBoxItem *listBoxItem, QWidget *parentConfigGroupBoxWidget,
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

	GroupBox w oknie konfiguracji definiowany jest przez nastêpuj±cy tag:
	<code>
		&lt;group-box caption="tytu³" id="id"&gr;
			...
		&lt;/group-box&gt;
	</code>

	GroupBox mo¿e znajdowaæ siê tylko wewn±trz tagu tab. W jego wnêtrzu
	mog± zawieraæ siê dowolne tagi widgetów konfigruacyjnych.

	Dodatkowo, GroupBox'a mo¿na stworzyæ (lub, je¿eli istnieje, uzyskaæ)
	wywo³uj±c funkcjê configGroupBox(section, tab, groupBox) z okna konfiguracyjnego.
	Do tak uzyskanego GroupBox'a mo¿na dodawaæ dowolne widgety (@see addWidget,
	@see addWidgets).
 **/
class ConfigGroupBox
{
	QString name;
	ConfigTab *configTab;

	Q3GroupBox *groupBox;
	QWidget *container;
	Q3GridLayout *gridLayout;

public:
	ConfigGroupBox(const QString &name, ConfigTab *configTab, Q3GroupBox *groupBox);
	~ConfigGroupBox();

	QWidget * widget() { return container; }

	/**
		Dodaje widget do GroupBoxa.
		@param widget dodawany widget (nie mo¿e byæ NULL)
		@param fullSpace kiedy true, dodany widget zajmuje ca³± szeroko¶æ GroupBox'a,
			w przeciwnym wypadku tylko praw± jego czê¶æ
	 **/
	void addWidget(QWidget *widget, bool fullSpace = false);
	/**
		Dodaje 2 widget do GroupBoxa, jeden po lewej stronie, drugi po prawej.
		@param widget1 widget dodawany z lewej strony (zazwyczaj etykieta)
		@param widget2 widget dodawany z prawej strony
	 **/
	void addWidgets(QWidget *widget1, QWidget *widget2);

	/**
		Zwraca true, gdy GroupBox nie posiada w sobie ¿adnych widgetów.
		@return true, gdy GroupBox nie posiada w sobie ¿adnych widgetów
	 **/
	bool empty();

};

/**
	@class ConfigurationWindow
	@author Vogel
	@short Widget okna konfigruacyjnego.

	Okno konfiguracyjne tego typu zawiera widgety konfiuracyjne podzielone wg. 3 stopnioej hierarchii.
	Pierwszym stopniem s± sekcje reprezentowane przez ListBox'a z lewej strony okna (zawieraj±cego
	ikony i opis tekstowy). Okno zawieraj±ce tylko jedn± sekcjê nie wy¶wietla ListBox'a.
	Drugim stopniem s± karty reprezentowane przez TabWidget'y, trzecim - grupy opisane przez GroupBox'y.

	Okno konfiguracyjne identyfikuje siê przez jego nazwê podan± w konstruktorze
	(dziêki tej nazwie ka¿de okno mo¿e osobno zapamiêtaæ swoj± pozycjê i ostatnio
	otwartej karty).

	Okna mog± teoretycznie zawieraæ dowolne widgety. Ka¿dy z nich, który dodatkowo
	dziedziczy z klasy ConfigWidget, traktowany jest w specjalny sposób. Jego
	metody loadConfiguration i saveConfiguration s± wywo³ywane automatycznie
	przy otwieraniu okna i przy zapisywaniu konfiguracji, dziêki czemu nie jest
	potrzebne ¿adne 'rêczne' podpinanie siê do tych akcji.

	W momencie zapisania konfiguracji wszystkie obiekty w programie bêd±ce instancajmi
	klasy @see ConfigurationAwareObject zostan± o tym poinformowane i bêd±
	mog³y zaktualizowaæ swój stan.

	Widgety w oknie mog± byæ tworzone na 2 sposoby. Pierwszym z nich jest
	pobranie GroupBoxa za pomoc± funkcji @see configGroupBox i dodawanie
	do niego widgetów za pomoc± jego funkcji addWidget i addWidgets.
	Drug± jest stworzenie plików XML *.ui, które s± wczytywane i usuwane dynamicznie
	z pomoc± metod @see appendUiFile i @see removeUiFile.

	W tym drugim przypadku stosuje siê pliki *.ui o nastêpuj±cej strukturze:

	&lt;configuration-ui&gt;
		&lt;section caption="tytu³" icon="nazwa_ikony"&gt;
			&lt;tab caption="tytu³"&gt;
				&lt;group-box caption="tytu³" id="id"&gt;
					&lt;widget ... /&gt;
				&lt;/group-box&gt;
			&lt;/tab&gt;
		&lt;/section&gt;
	&lt;/configuration-ui&gt;

	Elementy zawieraj±ce atrybut id (nie wymagany) - czyli group-box i dowolny widget
	mog± zostaæ pobrane przez aplikacje za pomoc± metody @see widgetById.
	Widgety z modu³ów powinny posiadaæ id w postaci: nazwaModu³u/nazwaId.

	Atrybut catpion jest wymagany. Mo¿liwe tagi widget s± opisane w dokumentacji
	klas Config* (np.: ConfigComboBox).
 **/
class ConfigurationWindow : public Q3VBox
{
	Q_OBJECT

	QString Name;

	Q3VBox *left;
	QMap<QString, ConfigSection *> configSections;
	ConfigSection *currentSection;

	QMap<QString, QWidget *> widgets;

	Q3ListBox *sectionsListBox;
	QWidget *container;

	void loadConfiguration(QObject *object);
	void saveConfiguration(QObject *object);

	ConfigSection *configSection(const QString &name);
	ConfigSection *configSection(const QString &icon, const QString &name, bool create = true);

	Q3ValueList<ConfigWidget *> processUiFile(const QString &fileName, bool append = true);
	Q3ValueList<ConfigWidget *> processUiSectionFromDom(QDomNode sectionNode, bool append = true);
	Q3ValueList<ConfigWidget *> processUiTabFromDom(QDomNode tabNode, const QString &iconName, const QString &sectionName, bool append = true);
	Q3ValueList<ConfigWidget *> processUiGroupBoxFromDom(QDomNode groupBoxNode, const QString &sectionName, const QString &tabName, bool append = true);
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
		jest przy zapamiêtywaniu pozycji okna oraz jego ostatnio
		otwartej karty.
	 **/
	ConfigurationWindow(const QString &name, const QString &caption);
	virtual ~ConfigurationWindow();

	QString name() { return Name; }

	/**
		Pobiera GroupBox'a zawartego w nastêpuj±cej ¶cie¿ce XPath:
		/section[@caption=SECTION]/tab[@caption=TAB]/group-box[@caption=GROUPBOX]

		Je¿eli nie istnieje taki group-box to zostanie on stworzony, gdy create == true.
		Dla crate == false zostanie zwrócony NULL.
	 **/
	ConfigGroupBox * configGroupBox(const QString &section, const QString &tab, const QString &groupBox, bool create = true);

	/**
		Je¿eli okno jest ukryte wczytuje warto¶ci elementów z pliku
		konfiguracyjnego i pokazuje okno.
	 **/
	virtual void show();

	/**
		Dodaje do okna konfiguracyjnego widgety wczytane z pliku fileName.
		Gdy load == true wczytuje ich warto¶ci z pliku konfiguracyjnego. W przeciwnym
		wypadku wczytanie warto¶ci nast±pi po wywo³aniu metody @see show.
	 **/
	Q3ValueList<ConfigWidget *> appendUiFile(const QString &fileName, bool load = true);
	/**
		Usuwa kontrolki wczytane z danego pliku *.ui.
	 **/
	void removeUiFile(const QString &fileName);

	/**
		Zwraca element opisane id="id" we wczytanych plikach *.ui. Mo¿liwe
		dowolne widgety konfiguracyjne i elementy group-box.
	 **/
	QWidget *widgetById(const QString &id);

	// TODO: make private or sth
	void removedConfigSection(const QString &sectionName);

signals:
	/**
		Sygna³ emitowany po naci¶niêciu Ok lub Apply ale przed zapisaniem
		warto¶ci do pliku konfiguracyjnego. Nietypowe widgety konfiguracyjne
		powinny siê podpi±æ pod ten sygna³ i po jego uzyskaniu zapisaæ
		nowe warto¶ci do pliku.
	 **/
	void configurationWindowApplied();

};

#endif // KADU_CONFIGURATION_WINDOW_H

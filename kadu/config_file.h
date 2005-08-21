#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <qcolor.h>
#include <qdom.h>
#include <qmap.h>
#include <qrect.h>
#include <qsize.h>
#include <qstring.h>
#include <qstringlist.h>

/**
	Klasa reprezentuj±ca plik XML-owy z zapisem konfiguracji programu
**/
class XmlConfigFile
{
	private:
		QDomDocument DomDocument;
		void write(const QString& f = QString::null);

	public:
		/**
			Otwiera plik i go wczytuje
		**/
		XmlConfigFile();
		/**
			Wczytuje plik konfiguracyjny z dysku
		**/
		void read();		
		/**
			Zapisuje na dysk zawarto¶æ konfiguracji
		**/
		void sync();
		/**
			Zapisuje do wybranego pliku kopiê konfiguracji
		**/
		void saveTo(const QString &filename);
		/**
			Zwraca glowny element konfiguracji
		**/
		QDomElement rootElement();
		/**
			Dodaje nowy element i przypisuje do rodzica.
		**/
		QDomElement createElement(QDomElement parent, const QString& tag_name);
		/**
			Zwraca pierwszy element lub element typu null jesli nie znajdzie.
		**/
		QDomElement findElement(QDomElement parent, const QString& tag_name);
		/**
			Zwraca pierwszy element ktorego dany atrybut ma dana wartosc
			lub element typu null jesli nie znajdzie.
		**/
		QDomElement findElementByProperty(QDomElement parent, const QString& tag_name,
			const QString& property_name, const QString& property_value);
		/**
			Zwraca pierwszy element lub dodaje nowy i przypisuje do rodzica.
		**/
		QDomElement accessElement(QDomElement parent, const QString& tag_name);
		/**
			Zwraca pierwszy element ktorego dany atrybut ma dana wartosc
			lub dodaje nowy i przypisuje do rodzica (ustawia rowniez zadany
			atrybut na zadana wartosc).
		**/
		QDomElement accessElementByProperty(QDomElement parent, const QString& tag_name,
			const QString& property_name, const QString& property_value);
		/**
			Usuwa wszystkie dzieci elementu.
		**/
		void removeChildren(QDomElement parent);
};

extern XmlConfigFile* xml_config_file;

/**
	Klasa reprezentuj±ca tekstowy plik konfiguracyjny (format INI)
**/
class PlainConfigFile {

	private:
		void write() const;
		bool changeEntry(const QString &group, const QString &name, const QString &value);
		QString getEntry(const QString &group, const QString &name, bool *ok = 0) const;

		QString filename;
		QMap<QString, QMap<QString, QString> > groups;
		mutable QString activeGroupName;
		mutable QMap<QString, QString> *activeGroup;

	public:
		/**
			Otwiera plik filename i go wczytuje
		**/
		PlainConfigFile(const QString &filename);
		/**
			Wczytuje plik konfiguracyjny z dysku
		**/
		void read();
		/**
			Zapisuje na dysk zawarto¶æ konfiguracji
		**/
		void sync() const;		
		/**
			Zwraca ca³± sekcjê "name"
		**/
		QMap<QString, QString>& getGroupSection(const QString& name);
		
		/**
			Zwraca listê sekcji
		**/
		QStringList getGroupList() const;
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QString do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QString &value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ char* do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const char *value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ int do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const int value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ double do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const double value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ bool do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const bool value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QRect do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QRect &value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QSize do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QSize &value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QColor do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QColor &value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QFont do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QFont &value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QPoint &value);
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QString readEntry(const QString &group,const QString &name, const QString &def = QString::null) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		int readNumEntry(const QString &group,const QString &name, int def = 0) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		unsigned int readUnsignedNumEntry(const QString &group,const QString &name, unsigned int def = 0) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		double readDoubleNumEntry(const QString &group,const QString &name, double def = 0.0) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		bool readBoolEntry(const QString &group,const QString &name, bool def = false) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QRect readRectEntry(const QString &group,const QString &name, const QRect *def = 0L) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QSize readSizeEntry(const QString &group,const QString &name, const QSize *def = 0L) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QColor readColorEntry(const QString &group,const QString &name, const QColor *def = 0L) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QFont readFontEntry(const QString &group,const QString &name, const QFont *def = 0L) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QPoint readPointEntry(const QString &group,const QString &name, const QPoint *def = 0L) const;
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QString &value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const char *value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const int value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const double value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const bool value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QRect &value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QSize &value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QColor &value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QFont &value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QPoint &value);
};

/**
	Klasa reprezentuj±ca plik z zapisem konfiguracji programu
**/
class ConfigFile {

	private:
		bool changeEntry(const QString &group, const QString &name, const QString &value);
		QString getEntry(const QString &group, const QString &name, bool *ok = 0) const;

		QString filename;
		QMap<QString, QMap<QString, QString> > groups;
		mutable QString activeGroupName;
		mutable QMap<QString, QString> *activeGroup;

	public:
		/**
			Otwiera plik filename i go wczytuje
		**/
		ConfigFile(const QString &filename);
		/**
			Zapisuje na dysk zawarto¶æ konfiguracji
		**/
		void sync() const;		
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QString do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QString &value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ char* do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const char *value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ int do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const int value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ double do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const double value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ bool do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const bool value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QRect do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QRect &value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QSize do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QSize &value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QColor do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QColor &value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QFont do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QFont &value);
		/**
			Zapisuje warto¶æ "value" do grupy "group" w pole "name"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void writeEntry(const QString &group,const QString &name, const QPoint &value);
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QString readEntry(const QString &group,const QString &name, const QString &def = QString::null) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		int readNumEntry(const QString &group,const QString &name, int def = 0) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		unsigned int readUnsignedNumEntry(const QString &group,const QString &name, unsigned int def = 0) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		double readDoubleNumEntry(const QString &group,const QString &name, double def = 0.0) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		bool readBoolEntry(const QString &group,const QString &name, bool def = false) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QRect readRectEntry(const QString &group,const QString &name, const QRect *def = 0L) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QSize readSizeEntry(const QString &group,const QString &name, const QSize *def = 0L) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QColor readColorEntry(const QString &group,const QString &name, const QColor *def = 0L) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QFont readFontEntry(const QString &group,const QString &name, const QFont *def = 0L) const;
		/**
			Zwraca warto¶æ pola "name" z grupy "group"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param def domy¶lna warto¶æ pola
			@return warto¶æ danego pola, je¶li nie istnieje zwracana
			jest warto¶æ "def"
		**/
		QPoint readPointEntry(const QString &group,const QString &name, const QPoint *def = 0L) const;
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QString &value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const char *value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const int value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const double value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const bool value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QRect &value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QSize &value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QColor &value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QFont &value);
		/**
			Tworzy pole "name" w grupie "group" je¶li ono nie istnieje
			i ustawia warto¶æ na "value"
			@param group nazwa grupy
			@param name nazwa pola w grupie
			@param value warto¶æ QPoint do wpisania
		**/
		void addVariable(const QString &group,const QString &name, const QPoint &value);
};

//TODO: po wydaniu 0.4 trzeba zmieniæ nazwê na config_file i wywaliæ define'a
extern ConfigFile *config_file_ptr;
#define config_file (*config_file_ptr)

#endif

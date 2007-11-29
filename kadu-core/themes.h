#ifndef KADU_THEMES_H
#define KADU_THEMES_H

#include <qmap.h>
#include <qobject.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>

/**
	Klasa obs³uguj±ca ró¿ne zestawy (jak ikony i emotikony)
	\class Themes
	\brief Klasa zarz±dzaj±ca zestawami
**/

class Themes : public QObject
{
	Q_OBJECT

	friend class EmoticonsManager;

	QStringList ThemesList; //!< lista z nazwami dostêpnych zestawów
	QStringList ThemesPaths; //!< lista ¶cie¿ek w których szukamy zestawów
	QStringList additional;
	QString ConfigName; //!< nazwa pliku z konfiguracj± zestawu
	QString Name; //!< typ zestawu (np. "icons", "emoticons")
	QString ActualTheme; //!< nazwa aktualnie wybranego zestawu
	QMap<QString, QString> entries;

	/**
		\fn QStringList getSubDirs(const QString& path, bool validate = true) const
		Wyszukuje podkatalogi w podanej ¶cie¿ce i zwraca listê tych zawieraj±cych
		pliki konfiguracyjne zestawu. Je¶li parametr validate ma warto¶æ false,
		zwrócona zostanie lista wszystkich znalezionych podkatalogów.
		\param path ¶cie¿ka do katalogu
		\param validate sprawdzenie zawarto¶ci
		\return lista podkatalogów
	**/
	QStringList getSubDirs(const QString& path, bool validate = true) const;

	/**
		\fn bool validateDir(const QString& path) const
		Sprawdza czy w podanym katalogu, b±d¼ jego podkatalogach znajduj±
		siê pliki z konfiguracj± zestawu.
		\param path ¶cie¿ka do katalogu
	**/
	bool validateDir(const QString& path) const;

	QString fixFileName(const QString& path, const QString& fn) const;

	public:
		/**
			\fn Themes(const QString& name, const QString& configname, const char *cname = 0)
			Konstruktor tworz±cy nowy zestaw
			\param name typ zestawu
			\param configname plik z konfiguracj± zestawu
			\param cname nazwa obiektu
		**/
		Themes(const QString& name, const QString& configname, const char *cname = 0);

		/**
			\fn QStringList defaultKaduPathsWithThemes() const
			Tworzy listê ¶cie¿ek do zestawów znajduj±cych siê w lokalizacjach
			domy¶lnych dla typu zestawu.
			\return lista ¶cie¿ek do zestawów
		**/
		QStringList defaultKaduPathsWithThemes() const;

		/**
			\fn const QStringList &themes() const
			Funkcja zwraca listê z nazwami dostêpnych zestawów
		**/
		const QStringList &themes() const;

		/**
			\fn const QString &theme() const
			Funkcja zwraca nazwê aktualnie wybranego zestawu
		**/
		const QString &theme() const;

		/**
			\fn const QStringList &paths() const
			Funkcja zwraca listê ¶cie¿ek, w których wyszukiwane s± zestawy
		**/
		const QStringList &paths() const;

		const QStringList &additionalPaths() const;

		/**
			\fn const QStringList &paths() const
			Funkcja zwraca ¶cie¿kê do wskazanego zestawu
		**/
		QString themePath(const QString& theme = QString::null) const;

		QString getThemeEntry(const QString& name) const;
		const QMap<QString, QString> & getEntries() { return entries; }

	public slots:
		/**
			\fn void setTheme(const QString& theme)
			Zmienia aktualny zestaw na wskazany o ile ten istnieje
			\param theme nazwa zestawu
		**/
		void setTheme(const QString& theme);

		/**
			\fn void setPaths(const QStringList& paths)
			Ustawia listê ¶cie¿ek, w których wyszukiwane s± zestawy
			\param paths lista ¶cie¿ek
		**/
		void setPaths(const QStringList& paths);

	signals:
		/**
			\fn void themeChanged(const QString& theme)
		 	Sygna³ jest emitowany, gdy zmieniono aktualny zestaw
			\param theme nazwa nowego zestawu
		**/
		void themeChanged(const QString& theme);

		/**
			\fn void pathsChanged(const QStringList& list)
		 	Sygna³ jest emitowany, gdy zmieniono ¶cie¿kê wyszukiwania
			\param list lista nowych ¶cie¿ek
		**/
		void pathsChanged(const QStringList& list);
};

#endif

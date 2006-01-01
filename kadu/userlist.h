#ifndef KADU_USERLIST_H
#define KADU_USERLIST_H

#include <qstring.h>
#include "usergroup.h"

/**
	\class UserList
	\brief Klasa reprezentuj±ca listê kontaktów.
**/
class UserList : public UserGroup
{
	Q_OBJECT
	public:
		/**
			\fn static void initModule()
			Inicjuje modu³ listy kontaktów.
		**/
		static void initModule();

		/**
			\fn static void closeModule()
			Sprz±ta po module listy kontaktów.
		**/
		static void closeModule();

	public slots:
		/**
			\fn bool readFromFile()
			Wczytuje listê kontaktów z plików "userlist"
			i "userattribs" z katalogu z ustawieniami.
			TODO: obsolete, do wywalenia w 0.6
			\return FALSE, gdy wyst±pi³y b³êdy, lub TRUE w przeciwnym wypadku.
		**/
		bool readFromFile();

		/**
			\fn void merge(const QValueList<UserListElement> &list)
			Scala listê kontaktów z podan± list± kontaktów \a userlist.
			\param userlist lista kontaktów z któr± nale¿y scaliæ
		**/
		void merge(const QValueList<UserListElement> &list);

		/**
			\fn void readFromConfig()
			Wczytuje listê kontaktów z drzewa xml z konfiguracj±
			programu. Poprzednie kontakty s± usuwane.
		**/
		void readFromConfig();

		/**
			\fn void writeToConfig()
			Dodaje listê kontaktów do drzewa xml z konfiguracj±
			programu. Poprzednie kontakty s± usuwane.
		**/
		void writeToConfig();

		/**
			\fn void setAllOffline(const QString &protocolName)
			\param protocolName identyfikator protoko³u
			Przestawia status wszystkich kontaktów
			dla wskazanego protoko³u na "niedostêpny".

		**/
		void setAllOffline(const QString &protocolName);

		/**
			\fn void clear()
			Usuwa wszystkie kontakty.
		**/
		void clear();

		void addPerContactNonProtocolConfigEntry(const QString &attribute_name, const QString &internal_key);
		void removePerContactNonProtocolConfigEntry(const QString &attribute_name);
		void addPerContactProtocolConfigEntry(const QString &protocolName, const QString &attribute_name, const QString &internal_key);
		void removePerContactProtocolConfigEntry(const QString &protocolName, const QString &attribute_name);


	private:
		/**
			\fn UserList()
			Konstruktor standardowy.
		**/
		UserList();

		/**
			\fn ~UserList()
		**/
		virtual ~UserList();

		QMap<QString, QString> nonProtoKeys;
		QMap<QString, QMap<QString, QString> > protoKeys;

		void initKeys();
};

extern UserList *userlist;

#endif

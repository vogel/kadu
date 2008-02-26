#ifndef KADU_HTML_DOCUMENT_H
#define KADU_HTML_DOCUMENT_H

#define QT3_SUPPORT
#include <qglobal.h>

#include <qstring.h>
#include <q3valuelist.h>

class QRegExp;

/**
	Klasa reprezentuj±ca dokument html. Przechowuje
	listê elementów. Element mo¿e byæ tagiem html
	albo kawa³kiem tekstu.
**/
class HtmlDocument
{
	private:
		struct Element
		{
			QString text;
			bool tag;
			Element() : text(), tag(false) {}
		};
		Q3ValueList<Element> Elements;
		void addElement(Element e);
		static QRegExp* url_regexp;
		static QRegExp* mail_regexp;
		static QRegExp* gg_regexp;

	public:
		static void escapeText(QString& text);
		static void unescapeText(QString& text);

		HtmlDocument();

		/**
			Dodaje podany tekst do listy elementów jako
			tag html.
			@param text tekst do dodania
		**/
		void addTag(const QString &text);
		/**
			Dodaje podany tekst do listy elementów jako
			zwyk³y tekst.
			@param text tekst do dodania
		**/
		void addText(const QString &text);
		/**
			Wstawia podany tekst do listy elementów jako
			tag html przed innym elementem.
			@param pos indeks elementu przed ktorym wstawiamy
			@param text tekst do dodania
		**/
		void insertTag(const int pos,const QString &text);
		/**
			Wstawia podany tekst do listy elementów jako
			zwykly tekst przed innym elementem.
			@param pos indeks elementu przed ktorym wstawiamy
			@param text tekst do dodania
		**/
		void insertText(const int pos,const QString &text);
		/**
			Parsuje podany napis zgodny ze struktur± html
			i otrzymane dane przypisuje do dokumentu.
			@param html napis do skonwertowania
		**/
		void parseHtml(const QString& html);
		/**
			Na podstawie zawarto¶ci dokumentu generuje
			napis zgodny ze struktur± html. Znaki specjalne
			wchodz±ce w sk³ad elementów nie bêd±cych tagami
			html s± escapowane.
		**/
		QString generateHtml() const;
		/**
			Zwraca ilo¶æ elementów wchodz±cych w sk³ad
			dokumentu.
		**/
		int countElements() const;
		/**
			Sprawdza czy element o podanym indeksie jest
			tagiem html czy zwyk³ym tekstem.
		**/
		bool isTagElement(int index) const;
		/**
			Zwraca tekst elementu o podanym indeksie.
		**/
		const QString &elementText(int index) const;
		/**
			Zwraca tekst elementu o podanym indeksie.
		**/
		QString &elementText(int index);
		/**
			Ustawia tekst i typ elementu o podanym indeksie.
		**/
		void setElementValue(int index, const QString& text, bool tag=false);
		/**
			Wydziela z elementu podany fragment tekstu.
			Element mo¿e pozostaæ jeden albo
			ulec podzieleniu na dwa lub nawet trzy elementy.
			Funkcja zmienia warto¶æ argumentu index, aby
			indeks wydzielonego elementu by³ aktualny.
		**/
		void splitElement(int& index, int start, int length);

		/**
			Skanuje tekst w poszukiwaniu adresów www,
			np. "www.kadu.net" i zmienia je w linki html.
		**/
		void convertUrlsToHtml();

		/**
			Skanuje tekst w poszukiwaniu adresów email
			i zamienia je w linki html.
		**/
		void convertMailToHtml();

		/**
			Skanuje tekst w poszukiwaniu numerów gg
			i zamienia je w linki html.
		**/
		void convertGGToHtml();

		/**
			Wyra¿enie regularne przydatne w poszukiwaniu
			adresów www (i innych).
		**/
		static const QRegExp &urlRegExp();

		/**
			Wyra¿enie regularne przydatne w poszukiwaniu
			adresów email.
		**/
		static const QRegExp &mailRegExp();

		/**
			Wyra¿enie regularne przydatne w poszukiwaniu
			numerów gg.
		**/
		static const QRegExp &ggRegExp();
};

#endif

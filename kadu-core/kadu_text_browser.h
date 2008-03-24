#ifndef KADU_TEXT_BROWSER_H
#define KADU_TEXT_BROWSER_H

#include <qglobal.h>

#include <qfont.h>
#include <qpoint.h>
#include <qstring.h>
#include <QTextBrowser>
#include <qtimer.h>
// #include <qtooltip.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QMouseEvent>
#include <Q3PopupMenu>

class QMenu;
class QPainter;
class QUrl;
class QMouseEvent;

/**
	Zmodyfikowany QTextBrowser specjalnie na potrzeby Kadu.
	Klikniêcie na linku otwiera ustawion± w konfiguracji przegl±darkê.
	W menu kontekstowym jest dodatkowa opcja "Kopiuj lokacjê odno¶nika".
	\class KaduTextBrowser
	\brief Klasa bazowa dla ChatMessagesView orazz InfoPanel.
**/
class KaduTextBrowser : public QTextBrowser //, private QToolTip
{
	Q_OBJECT

	private:
		QTimer refreshTimer; /*!< Timer od¶wie¿ania widgetu. */
		QString anchor;  /*!< Bie¿±co u¿ywany link. */
		int level;
		/**
			this value stores the current highlighted link
			for use with maybeTip(), or is null
		**/
		QString highlightedlink;  /*!< Aktualnie pod¶wietlony link (lub NULL je¶li takiego akurat brak). */
		QString image;
		bool trueTransparency;

	private slots:
		/**
			\fn void refresh()
			Slot od¶wie¿aj±cy zawarto¶æ widgetu.
		**/
 		void refresh();
		/**
			\fn void refreshLater()
			Slot uruchamiaj±cy od¶wie¿enie z opó¼nieniem.
			@see refreshTimer
		**/
 		void refreshLater();
		/**
			\fn void copyLinkLocation()
			Kopiuje wskazany odno¶nik do schowka.
		**/
 		void copyLinkLocation();
		/**
			\fn void hyperlinkClicked(const QUrl &anchor) const
			Otwiera klikniêty odno¶nik w okre¶lonej w konfiguracji przegl±darce.
			@param anchor Odno¶nik do otwarcia.
		**/
		void hyperlinkClicked(const QUrl &anchor) const;
		void linkHighlighted(const QString &);
		void saveImage();
		void verticalSliderPressedSlot();
		void verticalSliderReleasedSlot();

	protected:
		QMenu *createPopupMenu(const QPoint &point);
		virtual void drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph);
		virtual void maybeTip(const QPoint&);
		virtual void mouseReleaseEvent(QMouseEvent * e);
		virtual void wheelEvent(QWheelEvent * e);

	public:
		KaduTextBrowser(QWidget *parent = 0);
		void setSource(const QString &name);
		/**
			Nadpisane dla wyja¶nienia wieloznaczno¶ci
		**/
		void clear()
		{
			setText("");
			//dla QT<3.2 clear() powodowa³o b³±d
		}
		/**
			Nadpisane dla wyja¶nienia wieloznaczno¶ci		**/

		void setFont(const QFont& f) 	{ QTextBrowser::setFont(f); }
		void setMargin(int width);
		/**
			Returns path to image at position point, or null if there's no image.
		**/
		QString imageAt(const QPoint &point);

		/**
			default: false
		**/
		void setTrueTransparency(bool);
		bool isTrueTransparencyEnabled() const;
/* TODO: uncomment if needed
	public slots:
		virtual void copy();
*/
	signals:
		/**
			Dowolny przycisk myszy zosta³ zwolniony
			sender() - obiekt, który wygenerowa³ zdarzenie
		**/
		void mouseReleased(QMouseEvent *e);

		/**
			U¿yto rolki myszy
			sender() - obiekt, który wygenerowa³ zdarzenie
		**/
		void wheel(QWheelEvent *e);
};

#endif

#ifndef HISTORY_ADV_SEARCH_WINDOW_H
#define HISTORY_ADV_SEARCH_WINDOW_H

#include <QtCore/qdatetime.h>
#include <QtGui/qdialog.h>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>
#include <QtCore/QList>

#include "usergroup.h"

/*!
\class HistoryAdvSearchWindow
\brief Okno zaawansowanego przeszukiwania historii
*/
class HistoryAdvSearchWindow : public QDialog
{	
	Q_OBJECT

	public:
		/**
       		Konstruktor okna wyszukiwania.
		@param window w za³o¿eniu wska¼nik do okna przegl±dania historii
		*/
		HistoryAdvSearchWindow(QWidget *window);
	private:
		QComboBox* phraseEdit; /*!< pole z wyszukiwanym ci±giem znaków. */
		QWidget* parentWindow; /*!< Macierzyste okno przegl±dania historii. */
		QCheckBox* fromDateCheck; /*!< Checkbox dla daty "od". */
		QComboBox* fromDateCombo;  /*!< Pole wyboru daty "od". */
		QCheckBox* toDateCheck;  /*!< Checkbox dla daty "do". */
		QComboBox* toDateCombo; /*!< Pole wyboru daty "do". */
		QCheckBox* onlyWholeWords; /*!< Checkbox od wyszukiwania ca³ych wyrazów. */
		QCheckBox* caseSensitive;  /*!< Szukanie z uwzglêdnieniem wielko¶ci liter. */
		QCheckBox* regExp;  /*!< U¿ycie podanej frazy jako wyra¿enia regularnego. */
		QRadioButton* messagesOnly;  /*!< Tryb wy¶wietlania wyników - pojedyncze wiadomo¶ci z trafieniami. */
		QRadioButton* threadToEnd;  /*!< Tryb wy¶wietlania wyników - od znalezionej wiadomo¶ci do koñca w±tku. */
		QRadioButton* wholeThread; /*!< Tryb wy¶wletlania wyników - ca³y w±tek. */
	public slots:
		/**
			\fn void findButtonClicked();
			Slot odpalaj±cy wyszukiwanie w historii.
		**/
		void findButtonClicked();
		/**
			\fn void cancelButtonClicked();
			Slot zamykaj±cy okno wyszukiwania w historii.
		**/
		void cancelButtonClicked();
}; 

#endif

#ifndef KADU_CUSTOM_INPUT_H
#define KADU_CUSTOM_INPUT_H

#include <qglobal.h>

#include <q3multilineedit.h>
//Added by qt3to4:
#include <QKeyEvent>

class QKeyEvent;
class QWidget;

/**
	\class CustomInput
	\brief Klasa umo¿liwiaj±ca wpisywanie wiadomo¶ci
**/
class CustomInput : public Q3MultiLineEdit
{
	Q_OBJECT

	protected:

		bool autosend_enabled;/*!< zmienna informuj±ca czy w³±czone jest
					automatyczne wysy³anie */
		/**
			\fn virtual void keyPressEvent(QKeyEvent *e)
			Funkcja obs³uguj±ca naci¶niêcie przycisku
			\param e wska¼nik do obiektu obs³uguj±cego klawisze
		**/
		virtual void keyPressEvent(QKeyEvent *e);

		/**
			\fn virtual void keyReleaseEvent(QKeyEvent *e)
			Funkcja obs³uguj±ca zwolnienie przycisku
			\param e wska¼nik do obiektu obs³uguj±cego klawisze
		**/
		virtual void keyReleaseEvent(QKeyEvent* e);

	public:

		/*! Typ wyliczeniowy mowi±cy o rodzaju pisanych znaków */
		enum
		{
			KEY_BOLD,/*!< pogrubienie */
			KEY_ITALIC,/*!< kursywa */
			KEY_UNDERLINE/*!< podkre¶lenie */
		};

		/**
			\fn CustomInput(QWidget* parent = 0, const char* name = 0)
			Konstruktor tworz±cy obiekt
			i ustawiaj±cy odpowiedni styl
			\param parent rodzic okna
			\param name nazwa obiektu
		**/
		CustomInput(QWidget* parent = 0, const char* name = 0);

	public slots:

		/**
			\fn void paste()
			Slot wkleja czysty tekst
		**/
		void paste();

		/**
			\fn void setAutosend(bool on)
			Ustawia/wy³±cza automatyczne wysy³anie wiadomo¶ci
			\param on zmienna ustawiaj±ca autosend
		**/
		void setAutosend(bool on);

	signals:

		/**
			\fn void sendMessage()
			Sygna³ zostaje wys³any kiedy naci¶niêto
			klawisz wys³ania wiadomo¶ci
		**/
		void sendMessage();

		/**
			\fn void specialKeyPressed(int key)
			Sygna³ zostaje wys³any gdy naci¶niêto specjalny
			klawisz (skrót) odpowiadaj±cy np KEY_BOLD
			\param key warto¶æ z typu wyliczeniowego KEY_*
		**/
		void specialKeyPressed(int key);

		/**
			\fn void keyPressed(QKeyEvent* e, CustomInput* sender, bool &handled)
			Dowolny przycisk klawiatury zosta³ naci¶niêty.
			Przekazany zostaje tak¿e obiekt, który wywo³a³ akcjê -
			 czyli this
			\param e wska¼nik do obiektu obs³uguj±cego klawisze
			\param sender wska¼nik do obiektu, który
			wywo³a³ ten sygna³
			\param handled Slot osb³uguj±cy ten sygna³ powinien ten parametr ustawiæ na true,
			je¶li zdarzenie zosta³o ju¿ obs³u¿one i ¿adne czynno¶ci zwi±zane
			ze standardow± obs³ug± kontrolek Qt nie powinny byæ podjête.
		**/
		void keyPressed(QKeyEvent* e, CustomInput* sender, bool &handled);

		/**
			\fn void keyReleased(QKeyEvent* e, CustomInput* sender, bool &handled)
			Dowolny przycisk klawiatury zosta³ zwolniony.
			Przekazany zostaje tak¿e obiekt, który wywo³a³ akcjê -
			 czyli this.
			\param e wska¼nik do obiektu obs³uguj±cego klawisze
			\param sender wska¼nik do obiektu, który
			wywo³a³ ten sygna³
			\param handled Slot osb³uguj±cy ten sygna³ powinien ten parametr ustawiæ na true,
			je¶li zdarzenie zosta³o ju¿ obs³u¿one i ¿adne czynno¶ci zwi±zane
			ze standardow± obs³ug± kontrolek Qt nie powinny byæ podjête.
		**/
		void keyReleased(QKeyEvent* e, CustomInput* sender, bool &handled);
};

#endif

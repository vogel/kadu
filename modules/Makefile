QT_INCLUDES=-I/usr/qt/3/include

all: autoresponder.so echo.so

autoresponder.so: autoresponder.o autoresponder_moc.o
	gcc -Wall -shared -o autoresponder.so autoresponder.o autoresponder_moc.o

autoresponder.o: autoresponder.cpp autoresponder.h
	gcc -Wall -c -fPIC autoresponder.cpp -I../kadu $(QT_INCLUDES) -I../libgadu/lib

autoresponder_moc.o: autoresponder_moc.cpp
	gcc -Wall -c -fPIC autoresponder_moc.cpp -I../kadu $(QT_INCLUDES) -I../libgadu/lib

autoresponder_moc.cpp: autoresponder.h
	moc autoresponder.h > autoresponder_moc.cpp


echo.so: echo.o echo_moc.o
	gcc -Wall -shared -o echo.so echo.o echo_moc.o

echo.o: echo.cpp echo.h
	gcc -Wall -c -fPIC echo.cpp -I../kadu $(QT_INCLUDES) -I../libgadu/lib

echo_moc.o: echo_moc.cpp
	gcc -Wall -c -fPIC echo_moc.cpp -I../kadu $(QT_INCLUDES) -I../libgadu/lib

echo_moc.cpp: echo.h
	moc echo.h > echo_moc.cpp


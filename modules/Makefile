autoresponder.so: autoresponder.o autoresponder_moc.o
	gcc -Wall -shared -o autoresponder.so autoresponder.o autoresponder_moc.o

autoresponder.o: autoresponder.cpp autoresponder.h
	gcc -Wall -c -fPIC autoresponder.cpp -I ../kadu -I /usr/qt/3/include -I ../libgadu/lib

autoresponder_moc.o: autoresponder_moc.cpp
	gcc -Wall -c -fPIC autoresponder_moc.cpp -I ../kadu -I /usr/qt/3/include -I ../libgadu/lib

autoresponder_moc.cpp: autoresponder.h
	moc autoresponder.h > autoresponder_moc.cpp

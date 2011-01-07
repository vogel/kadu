/****************************************************************************
*   svg debug                                                               *
*   Copyright (C) 2011  Piotr Dąbrowski ultr@ultr.pl                        *
****************************************************************************/

#include <stdio.h>
#include <QIcon>

void showHelp( char *argv0 )
{
	fprintf( stderr, "Usage:\n" );
	fprintf( stderr, "  %s FILENAME\n", argv0 );
}

int main( int argc, char *argv[] )
{
	if( argc != 2 )
	{
		showHelp( argv[0] );
		return 1;
	}
	fprintf( stderr, "ICON: %s\n", argv[1] );
	QIcon icon;
	icon.addFile( argv[1] );
	return 0;
}

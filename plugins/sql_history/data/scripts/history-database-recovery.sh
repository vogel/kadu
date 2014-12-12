#!/usr/bin/env bash

ERROR_NO_ERROR=0
ERROR_SQLITE3_NOT_EXECUTABLE=1
ERROR_INVALID_PARAMETERS=2
ERROR_UNREADABLE_CORRUPTED_DATABASE=3
ERROR_INVALID_DIRECTORY=4
ERROR_UNABLE_TO_CREATE_BACKUP=5
ERROR_RECOVERING=6

function usage()
{
	echo "$0 corrupted_database_file"
	echo ""
	echo "  Will try to recover given database file."
	echo "  Corrupted database will be stored in corrupted_database_file.current-datetime.bak"
	echo ""
}

if [ ! $# -eq 1 ]; then
	usage
	exit $ERROR_INVALID_PARAMETERS
fi

SQLITE3=`which sqlite3`

if [ ! -e "$SQLITE3" ]; then
	exit $ERROR_SQLITE3_NOT_EXECUTABLE
fi

SQL_CORRUPTED_DATABASE=$1
SQL_CORRUPTED_DATABASE_BACKUP=$1.bak.`date +%Y.%m.%d.%H.%M.%S`

if [ ! -r "$SQL_CORRUPTED_DATABASE" ]; then
	exit $ERROR_UNREADABLE_CORRUPTED_DATABASE
fi

SQL_DATABASE_DIR=`dirname "$SQL_CORRUPTED_DATABASE"`

if [ ! -d "$SQL_DATABASE_DIR" ]; then
	exit $ERROR_INVALID_DIRECTORY
fi

pushd "$SQL_DATABASE_DIR" >/dev/null

if [ $? -gt 0 ]; then
	exit $ERROR_INVALID_DIRECTORY
fi

rm -f "$SQL_CORRUPTED_DATABASE_BACKUP"
mv "$SQL_CORRUPTED_DATABASE" "$SQL_CORRUPTED_DATABASE_BACKUP"

SQL_CORRUPTED_DATABASE=$SQL_CORRUPTED_DATABASE_BACKUP
SQL_DATABASE=$1

if [ $? -gt 0 ]; then
	popd >/dev/null
	exit $ERROR_UNABLE_TO_CREATE_BACKUP
fi

echo ".dump" | "$SQLITE3" "$SQL_CORRUPTED_DATABASE" | "$SQLITE3" "$SQL_DATABASE"

if [ $? -gt 0 ]; then
	popd >/dev/null
	exit $ERROR_RECOVERING
fi

popd >/dev/null

exit $ERROR_NO_ERROR

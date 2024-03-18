#include "DatabaseAccess.h"
#include <io.h>

#include <cstring>
bool DatabaseAccess::open()
{
	
	std::string dbFileName = "galleryDB.sqlite";
	int file_exist = _access(dbFileName.c_str(), 0);
	int res = sqlite3_open(dbFileName.c_str(), &db);
	if (res != SQLITE_OK) {
		db = nullptr;
		std::cout << "Failed to open DB" << std::endl;
		return -1;
	}


}

void DatabaseAccess::close()
{


	sqlite3_close(db);
}

void DatabaseAccess::clear()
{
	m_users.clear();
	m_albums.clear();
}

void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	const char* sqlStatement;
	char* errMessage = nullptr;
	std::string sqlQuery = "DELETE FROM ALBUMS WHERE NAME LIKE '" + albumName + "' AND USER_ID = " + std::to_string(userId) + ";";
	sqlStatement = sqlQuery.c_str();
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return ;
	}
	sqlite3_free(errMessage); // Free error message after each call
	
}

void DatabaseAccess::closeAlbum(Album& pAlbum)
{
	
}

void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	int picId = getPicIdFromAlbumAndPicName(albumName, pictureName, userId);
	const char* sqlStatement;
	char* errMessage = nullptr;
	std::string sqlQuery = "INSERT INTO TAGS VALUES (" + std::to_string(picId) + "," + std::to_string(userId) + ");";
	sqlStatement = sqlQuery.c_str();
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call
}

void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{

	int picId = getPicIdFromAlbumAndPicName(albumName, pictureName, userId);
	const char* sqlStatement;
	char* errMessage = nullptr;
	std::string sqlQuery = "REMOVE FROM TAGS WHERE " + std::to_string(picId) + "= PICTURE_ID AND" + std::to_string(userId) + "= USER_ID;";
	sqlStatement = sqlQuery.c_str();
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call


}

void DatabaseAccess::createUser(User& user)
{
	const char* sqlStatement;
	char* errMessage = nullptr;
	std::string sqlQuery = "INSERT INTO USERS VALUES ('" + user.getName() + "'," + std::to_string(user.getId()) + ");";
	sqlStatement = sqlQuery.c_str();
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call


}

void DatabaseAccess::deleteUser(const User& user)
{
	const char* sqlStatement;
	char* errMessage = nullptr;
	std::string sqlQuery = "DELETE FROM USERS WHERE NAME LIKE '" + user.getName() + "' AND ID = " + std::to_string(user.getId()) + ";";
	sqlStatement = sqlQuery.c_str();
	int res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call


}





int DatabaseAccess::getIdFromQuery(void* data, int argc, char** argv, char** azColName)
{
	int* id = static_cast<int*>(data);
	*id = std::stoi(argv[0]);
	return 0;
}

int DatabaseAccess::getAlbumIdFromName(const std::string& albumName, int userId)
{
	int id = 0;
	const char* sqlStatement;
	char* errMessage = nullptr;

	std::string sqlQuery = "SELECT * FROM ALBUMS WHERE NAME LIKE '" + albumName +"' AND USER_ID = " + std::to_string(userId) + ";";
	sqlStatement = sqlQuery.c_str();


	int res = sqlite3_exec(db, sqlStatement, getIdFromQuery, &id, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return 0;
	}
	sqlite3_free(errMessage); // Free error message after each call
	return id;
}

int DatabaseAccess::getPicIdFromAlbumAndPicName(const std::string& albumName, const std::string& pictureName, int userId)
{
	int albumId = getAlbumIdFromName(albumName, userId);
	int id = 0;
	const char* sqlStatement;
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT * FROM ALBUMS WHERE NAME LIKE '" + pictureName + "'AND ALBUM_ID =" + std::to_string(albumId) ";";

	sqlStatement = sqlQuery.c_str();
	int res = sqlite3_exec(db, sqlStatement, getIdFromQuery, &id, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return 0;
	}
	sqlite3_free(errMessage); // Free error message after each call

	return id;
}



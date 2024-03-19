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

	char* errMessage = nullptr;
	std::string sqlQuery = "DELETE FROM ALBUMS WHERE NAME LIKE '" + albumName + "' AND USER_ID = " + std::to_string(userId) + ";";
	int res = sqlite3_exec(db, sqlQuery.c_str(), nullptr, nullptr, &errMessage);
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

	char* errMessage = nullptr;
	std::string sqlQuery = "INSERT INTO TAGS VALUES (" + std::to_string(picId) + "," + std::to_string(userId) + ");";
	int res = sqlite3_exec(db, sqlQuery.c_str(), nullptr, nullptr, &errMessage);
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
	
	char* errMessage = nullptr;
	std::string sqlQuery = "REMOVE FROM TAGS WHERE " + std::to_string(picId) + "= PICTURE_ID AND" + std::to_string(userId) + "= USER_ID;";
	int res = sqlite3_exec(db, sqlQuery.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call


}

void DatabaseAccess::createUser(User& user)
{
	
	char* errMessage = nullptr;
	std::string sqlQuery = "INSERT INTO USERS VALUES ('" + user.getName() + "'," + std::to_string(user.getId()) + ");";
	int res = sqlite3_exec(db, sqlQuery.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call


}

void DatabaseAccess::deleteUser(const User& user)
{
	
	char* errMessage = nullptr;
	std::string sqlQuery = "DELETE FROM USERS WHERE NAME LIKE '" + user.getName() + "' AND ID = " + std::to_string(user.getId()) + ";";
	int res = sqlite3_exec(db, sqlQuery.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call


}




/**
* callback function to get the id, the id is the first value
* params - alot
*
* return - int id -  the id of the wanted item 
*/
int DatabaseAccess::getIdFromQuery(void* data, int argc, char** argv, char** azColName)
{
	int* id = static_cast<int*>(data);
	*id = std::stoi(argv[0]);
	return 0;
}
/**
* function to get the wanted album and then get its id 
* @param albumName - the name of the album
* @param userId - the id of the user
*
* return - int id -  the id of the wanted item
*/
int DatabaseAccess::getAlbumIdFromName(const std::string& albumName, int userId)
{
	int id = 0;
	char* errMessage = nullptr;

	std::string sqlQuery = "SELECT * FROM ALBUMS WHERE NAME LIKE '" + albumName +"' AND USER_ID = " + std::to_string(userId) + ";";


	int res = sqlite3_exec(db, sqlQuery.c_str(), getIdFromQuery, &id, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return 0;
	}
	sqlite3_free(errMessage); // Free error message after each call
	return id;
}
/**
* function to get the wanted pic and then get its id
* @param albumName - the name of the album
* @param - pictureName - the name of the picture
* @param userId - the id of the user
*
* return - int id -  the id of the wanted item
*/
int DatabaseAccess::getPicIdFromAlbumAndPicName(const std::string& albumName, const std::string& pictureName, int userId)
{
	int albumId = getAlbumIdFromName(albumName, userId);
	int id = 0;
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT * FROM ALBUMS WHERE NAME LIKE '" + pictureName + "'AND ALBUM_ID =" + std::to_string(albumId) + ";";

	int res = sqlite3_exec(db, sqlQuery.c_str(), getIdFromQuery, &id, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return 0;
	}
	sqlite3_free(errMessage); // Free error message after each call

	return id;
}



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








int DatabaseAccess::getTagsCallback(void* data, int argc, char** argv, char** azColName)
{
	std::list<Picture>* pics = static_cast<std::list<Picture>*>(data);
	for (int i = 0; i < argc ; i += 2)
	{
		for (auto& pic : *pics)
		{
			if (!strcmp(argv[i], std::to_string(pic.getId()).c_str()))
			{
				pic.tagUser(std::stoi(argv[i + 1]));
			}
		}
	}
	return 0;
}

int DatabaseAccess::getPicturesCallback(void* data, int argc, char** argv, char** azColName)
{
	std::list<Picture>* pics = static_cast<std::list<Picture>*>(data);
	for (int i = 0; i < argc - 1; i += 5)
	{
		Picture pic(std::stoi(argv[i]), argv[i + 1]);
		pics->push_back(pic);
	}
	return 0;
}

std::list<Picture> DatabaseAccess::getPictures(std::string name, int owner_id)
{
	int id = getAlbumIdFromName(name, owner_id);
	std::list<Picture> pics;


	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT * FROM PICTURES WHERE ALBUM_ID = " + std::to_string(id) + ";";
	int res = sqlite3_exec(db, sqlQuery.c_str(), getPicturesCallback, &pics, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return pics;
	}
	sqlite3_free(errMessage); // Free error message after each call

	//insert tags into the pictures

	errMessage = nullptr;
	sqlQuery = "SELECT * FROM TAGS;";
	res = sqlite3_exec(db, sqlQuery.c_str(), getTagsCallback, &pics, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return pics;
	}
	sqlite3_free(errMessage); // Free error message after each call



	return pics;
}

void DatabaseAccess::insertPictures(std::list<Album>& albums)
{
	for (auto& album : albums)
	{
		std::list<Picture> pics = getPictures(album.getName(), album.getOwnerId());
		for (auto pic : pics)
		{
			
			album.addPicture(pic);
		}
	}
}

int DatabaseAccess::getAlbumsCallback(void* data, int argc, char** argv, char** azColName)
{
	std::list<Album>* albums = static_cast<std::list<Album>*>(data);
	for (int i = 0; i < argc - 1; i += 3)
	{
		Album album(std::stoi(argv[i + 2]), argv[i + 1], argv[i + 3]);
		albums->push_back(album);
	}

	return 0;
}

const std::list<Album> DatabaseAccess::getAlbums()
{
	std::list<Album> albums;
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT * FROM ALBUMS;";
	int res = sqlite3_exec(db, sqlQuery.c_str(), getAlbumsCallback, &albums, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return albums;
	}
	sqlite3_free(errMessage); // Free error message after each call
	insertPictures(albums);




	return albums;
}

const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
	std::list<Album> albums;
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT * FROM ALBUMS WHERE USER_ID =" + std::to_string(user.getId()) + ";";
	int res = sqlite3_exec(db, sqlQuery.c_str(), getAlbumsCallback, &albums, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return albums;
	}
	sqlite3_free(errMessage); // Free error message after each call
	insertPictures(albums);
	return albums;
}

void DatabaseAccess::createAlbum(const Album& album)
{
	
	char *errMessage = nullptr;
	std::string sqlQuery = "INSERT INTO ALBUMS (NAME, USER_ID, CREATION_DATE) VALUES ('" + album.getName() + "'," + std::to_string(album.getOwnerId()) + ",'" + album.getCreationDate() + "');";
	int res = sqlite3_exec(db, sqlQuery.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
	}
	



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
	
	//delete pictures too
	 for(auto it: getPictures(albumName, userId))
	{
		 removePictureFromAlbumByName(albumName, it.getName(), userId);
	}

}

bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	std::list<Album> albums;
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT * FROM ALBUMS WHERE USER_ID = " + std::to_string(userId) + " AND NAME LIKE '" + albumName + "';";
	int res = sqlite3_exec(db, sqlQuery.c_str(), getAlbumsCallback, &albums, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return false;
	}
	sqlite3_free(errMessage); // Free error message after each call
	if (!albums.empty())
	{
		return true;
	}
	return false;
}

Album DatabaseAccess::openAlbum(const std::string& albumName)
{
	std::list<Album> albums;
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT * FROM ALBUMS WHERE NAME LIKE '" + albumName + "';";
	int res = sqlite3_exec(db, sqlQuery.c_str(), getAlbumsCallback, &albums, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return albums.front();
	}
	sqlite3_free(errMessage); // Free error message after each call
	insertPictures(albums);

	return albums.front();
}

void DatabaseAccess::closeAlbum(Album& pAlbum)
{
	
}

int DatabaseAccess::printAlbumsCallback(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc - 1; i += 3)
	{
		std::cout << "name:  " << argv[i + 1] << "  User Id:  " << argv[i + 2] << "  creation date:  " << argv[i + 3] << std::endl;
	}


	return 0;
}

void DatabaseAccess::printAlbums()
{
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT * FROM ALBUMS;";
	int res = sqlite3_exec(db, sqlQuery.c_str(), printAlbumsCallback, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call

}

void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture, int userId)
{
	
	int album_id = getAlbumIdFromName(albumName, userId);

	char* errMessage = nullptr;
	std::string sqlQuery = "INSERT INTO PICTURES (NAME, LOCATION, CREATION_DATE, ALBUM_ID) VALUES ('" + picture.getName() + "', '" + picture.getPath() + "', '" +picture.getCreationDate() + "', " + std::to_string(album_id) + ");";
	int res = sqlite3_exec(db, sqlQuery.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call


}

void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName, int userId)
{
	int album_id = getAlbumIdFromName(albumName, userId);

	char* errMessage = nullptr;
	std::string sqlQuery = "DELETE FROM PICTURES WHERE NAME LIKE '" + pictureName + "' AND ALBUM_ID = " + std::to_string(album_id) + ";";
	int res = sqlite3_exec(db, sqlQuery.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call


}

void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int taggerId, int userId)
{
	int picId = getPicIdFromAlbumAndPicName(albumName, pictureName, userId);

	char* errMessage = nullptr;
	std::string sqlQuery = "INSERT INTO TAGS VALUES (" + std::to_string(picId) + "," + std::to_string(taggerId) + ");";
	int res = sqlite3_exec(db, sqlQuery.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call
}

void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int taggerId, int userId)
{

	int picId = getPicIdFromAlbumAndPicName(albumName, pictureName, userId);
	
	char* errMessage = nullptr;
	std::string sqlQuery = "DELETE FROM TAGS WHERE PICTURE_ID = " + std::to_string(picId) + " AND USER_ID =" + std::to_string(taggerId) + ";";
	int res = sqlite3_exec(db, sqlQuery.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return;
	}
	sqlite3_free(errMessage); // Free error message after each call


}

int DatabaseAccess::printUsersCallback(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0;  i < argc - 1; i += 2)
	{
		std::cout << argv[i + 1] << std::endl;
	}
	return 0;
}

void DatabaseAccess::printUsers()
{
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT * FROM USERS;";
	int res = sqlite3_exec(db, sqlQuery.c_str(), printUsersCallback, nullptr, &errMessage);
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
	std::string sqlQuery = "INSERT INTO USERS VALUES (" + std::to_string(user.getId()) + ", '" + user.getName() + "');";
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
	
	for (auto it: getAlbumsOfUser(user))
	{
		deleteAlbum(it.getName(), user.getId());

	}
}

int DatabaseAccess::doesUserExistsCallback(void* data, int argc, char** argv, char** azColName)
{
	int* id = static_cast<int*>(data);
	if (argc == 0)
	{
		*id = 0;
	}
	else
	{
		*id = 1;
	}

	return 0;
}

bool DatabaseAccess::doesUserExists(int userId)
{
	int id = 0;
	char* errMessage = nullptr;

	std::string sqlQuery = "SELECT * FROM USERS WHERE ID = " + std::to_string(userId) + ";";


	int res = sqlite3_exec(db, sqlQuery.c_str(), doesUserExistsCallback, &id, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
	}
	sqlite3_free(errMessage); // Free error message after each call
	return id;

}

int DatabaseAccess::getUserCallback(void* data, int argc, char** argv, char** azColName)
{
	User* user = static_cast<User*>(data);
	user->setId(std::stoi(argv[0]));
	user->setName(argv[1]);
	return 0;
}

User DatabaseAccess::getUser(int userId)
{
	User user(0, ""); // random values for now
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT * FROM USERS WHERE ID = " + std::to_string(userId) + ";";
	int res = sqlite3_exec(db, sqlQuery.c_str(), getUserCallback, &user, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
	}
	sqlite3_free(errMessage); // Free error message after each call
	return user;

}
///////////

///////////

///////////

int DatabaseAccess::countCallback(void* data, int argc, char** argv, char** azColName)
{
	int* count = static_cast<int*>(data);
	*count = std::stoi(argv[0]);
	return 0;
}

int DatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	int count = 0;
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT COUNT(*) FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	int res = sqlite3_exec(db, sqlQuery.c_str(), countCallback, &count, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return 0;
	}
	sqlite3_free(errMessage); // Free error message after each call
	
	return count;
}

int DatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	int count = 0;
	
	std::list<Album> albums = getAlbums();
	for (auto it: albums )
	{
		for (auto pic : it.getPictures())
		{
			
			if (pic.isUserTagged(user))
			{
				count++;
				break;

			}
		}
	}
	return count;
}

int DatabaseAccess::countTagsOfUser(const User& user)
{
	int count = 0;
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT COUNT(*) FROM TAGS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	int res = sqlite3_exec(db, sqlQuery.c_str(), countCallback, &count, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
	}
	sqlite3_free(errMessage); // Free error message after each call

	return count;
}

float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	float count = countAlbumsOwnedOfUser(user);
	float sum = countTagsOfUser(user);
	return sum/count;
}



User DatabaseAccess::getTopTaggedUser()
{
	int id = 0;
	//
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT TAGS.PICTURE_ID, TAGS.USER_ID FROM TAGS GROUP BY TAGS.USER_ID ORDER BY COUNT(*) DESC LIMIT 1;";
	int res = sqlite3_exec(db, sqlQuery.c_str(), getIdFromQuery, &id, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
	}
	sqlite3_free(errMessage); // Free error message after each call
	User user = getUser(id);
	



	return user;
}

int DatabaseAccess::getPictureCallback(void* data, int argc, char** argv, char** azColName)
{
	Picture* pic = static_cast<Picture*>(data);
	pic->setId(std::stoi(argv[0]));
	pic->setName(argv[1]);
	pic->setCreationDate(argv[2]);
	pic->setPath(argv[3]);
	return 0;
}

Picture DatabaseAccess::getPicture(int picId)
{
	std::list<Album> albums = getAlbums();
	for (auto it : albums)
	{
		for (auto pic : it.getPictures())
		{

			if (pic.getId() == picId)
			{
				
				return pic;

			}
		}
	}


}

Picture DatabaseAccess::getTopTaggedPicture()
{
	int id = 0;
	char* errMessage = nullptr;
	std::string sqlQuery = "SELECT TAGS.PICTURE_ID, TAGS.USER_ID FROM TAGS GROUP BY TAGS.PICTURE_ID ORDER BY COUNT(*) DESC LIMIT 1;";
	int res = sqlite3_exec(db, sqlQuery.c_str(), getIdFromQuery, &id, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
	}
	sqlite3_free(errMessage); // Free error message after each call
	
	Picture pic = getPicture(id);
	return pic;
}

std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user)
{
	std::list<Album> albums = getAlbumsOfUser(user);
	std::list<Picture> pics;
	for (auto album : albums)
	{
		for (auto pic : album.getPictures())
		{
			if (pic.getTagsCount() > 0)
			{
				pics.push_back(pic);
			}
		}
	}
	return pics;
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
	if (argc == 0)
	{
		*id = 0;
		return 0;
	}
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
	std::string sqlQuery = "SELECT * FROM PICTURES WHERE NAME LIKE '" + pictureName + "' AND ALBUM_ID = " + std::to_string(albumId) + ";";

	int res = sqlite3_exec(db, sqlQuery.c_str(), getIdFromQuery, &id, &errMessage);
	if (res != SQLITE_OK) {
		std::cout << "Error: " << errMessage << std::endl;
		sqlite3_free(errMessage); // Free error message
		return 0;
	}
	sqlite3_free(errMessage); // Free error message after each call

	return id;
}











#pragma once
#pragma once
#include <list>
#include "Album.h"
#include "User.h"
#include "IDataAccess.h"
#include "sqlite3.h"
class DatabaseAccess : public IDataAccess
{

public:
	DatabaseAccess() = default;
	virtual ~DatabaseAccess() = default;

	// album related
	
	
	
	
	const std::list<Album> getAlbums() override;
	const std::list<Album> getAlbumsOfUser(const User& user) override;
	void createAlbum(const Album& album) override;
	void deleteAlbum(const std::string& albumName, int userId) override;
	bool doesAlbumExists(const std::string& albumName, int userId) override;
	Album openAlbum(const std::string& albumName) override;
	void closeAlbum(Album& pAlbum) override;
	
	void printAlbums() override;

	// picture related
	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture, int userId) override;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName, int userId) override;
	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int taggerId, int userId) override;
	void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int taggerId, int userId) override;

	// user related
	void printUsers() override;
	void createUser(User& user) override;
	void deleteUser(const User& user) override;
	bool doesUserExists(int userId) override;
	
	User getUser(int userId) override;

	// user statistics
	
	int countAlbumsOwnedOfUser(const User& user) override;
	int countAlbumsTaggedOfUser(const User& user) override;
	int countTagsOfUser(const User& user) override;
	float averageTagsPerAlbumOfUser(const User& user) override;

	// queries
	
	User getTopTaggedUser() override;
	
	Picture getTopTaggedPicture() override;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) override;

	bool open() override;
	void close() override;
	void clear() override;

private:
	std::list<Album> m_albums;
	std::list<User> m_users;

	sqlite3 *db;
	
	//albums
	int getAlbumIdFromName(const std::string& albumName, int userId);
	static int getAlbumsCallback(void* data, int argc, char** argv, char** azColName);



	//pics
	int getPicIdFromAlbumAndPicName(const std::string& albumName, const std::string& pictureName, int userId);
	static int getPicturesCallback(void* data, int argc, char** argv, char** azColName);
	static int printAlbumsCallback(void* data, int argc, char** argv, char** azColName);

	Picture getPicture(int picId);

	std::list<Picture> getPictures(std::string name, int owner_id);
	void insertPictures(std::list<Album>& album);



	//users
	static int printUsersCallback(void* data, int argc, char** argv, char** azColName);
	static int getUserCallback(void* data, int argc, char** argv, char** azColName);

	

	//tags
	
	static int getTagsCallback(void* data, int argc, char** argv, char** azColName);

	// other/all
	bool errorHandler(char* errMsg, int res);
	static int getIdFromQuery(void* data, int argc, char** argv, char** azColName);
	
	
};

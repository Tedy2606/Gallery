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
	static int getTagsCallback(void* data, int argc, char** argv, char** azColName);
	static int getPicturesCallback(void* data, int argc, char** argv, char** azColName);
	std::list<Picture> getPictures(std::string name, int owner_id);
	void insertPictures(std::list<Album>& album);
	static int getAlbumsCallback(void* data, int argc, char** argv, char** azColName);
	const std::list<Album> getAlbums() override;
	const std::list<Album> getAlbumsOfUser(const User& user) override;
	void createAlbum(const Album& album) override;
	void deleteAlbum(const std::string& albumName, int userId) override;
	bool doesAlbumExists(const std::string& albumName, int userId) override;
	Album openAlbum(const std::string& albumName) override;
	void closeAlbum(Album& pAlbum) override;
	static int printAlbumsCallback(void* data, int argc, char** argv, char** azColName);
	void printAlbums() override;

	// picture related
	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture, int userId) override;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName, int userId) override;
	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int taggerId, int userId) override;
	void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int taggerId, int userId) override;

	// user related
	static int printUsersCallback(void* data, int argc, char** argv, char** azColName);
	void printUsers() override;
	void createUser(User& user) override;
	void deleteUser(const User& user) override;
	static int doesUserExistsCallback(void* data, int argc, char** argv, char** azColName);
	bool doesUserExists(int userId) override;
	static int getUserCallback(void* data, int argc, char** argv, char** azColName);
	User getUser(int userId) override;

	// user statistics
	static int countCallback(void* data, int argc, char** argv, char** azColName);
	int countAlbumsOwnedOfUser(const User& user) override;
	int countAlbumsTaggedOfUser(const User& user) override;
	int countTagsOfUser(const User& user) override;
	float averageTagsPerAlbumOfUser(const User& user) override;

	// queries
	
	User getTopTaggedUser() override;
	static int getPictureCallback(void* data, int argc, char** argv, char** azColName);
	Picture getPicture(int picId);
	Picture getTopTaggedPicture() override;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) override;

	bool open() override;
	void close() override;
	void clear() override;

private:
	std::list<Album> m_albums;
	std::list<User> m_users;

	sqlite3 *db;
	

	

	static int getIdFromQuery(void* data, int argc, char** argv, char** azColName);
	int getAlbumIdFromName(const std::string& albumName, int userId);



	int getPicIdFromAlbumAndPicName(const std::string& albumName, const std::string& pictureName, int userId);
};

#include "DatabaseAccess.h"
#include <io.h>
#include "sqlite3.h"
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

void DatabaseAccess::closeAlbum(Album& pAlbum)
{
	
}

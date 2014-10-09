#include <vector>
#include <string>

#include <sqlite3.h>

#include <Foundation/Foundation.h>

class DataBase {
public:
  DataBase(const char* path);
  ~DataBase();

  bool IsOpen() const { return m_db != nullptr; }
  std::vector<std::vector<std::string>> Query(const std::string& query) const;

private:
  sqlite3* m_db;
};

DataBase::DataBase(const char* path) : m_db(nullptr)
{
  sqlite3_open_v2(path, &m_db,  SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_READWRITE, nullptr);
}

DataBase::~DataBase()
{
  if (!m_db) {
    return;
  }
  sqlite3_close(m_db);
  m_db = nullptr;
}

std::vector<std::vector<std::string>>
DataBase::Query(const std::string& query) const
{
  std::vector<std::vector<std::string>> response;

  if (m_db == nullptr || query.empty()) {
    return response;
  }
  sqlite3_stmt *statement = nullptr;

  if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &statement, nullptr) == SQLITE_OK) {
    while (sqlite3_step(statement) == SQLITE_ROW) {
      std::vector<std::string> row;
      const int count = sqlite3_data_count(statement);

      for (int i = 0; i < count; i++) {
        const char* data = reinterpret_cast<const char*>(sqlite3_column_text(statement, i));
        row.push_back(data ? std::string(data) : "");
      }
      response.push_back(row);
    }
    sqlite3_finalize(statement);
  }
  return response;
}

int
main(int argc, char* argv[])
{
  @autoreleasepool {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSLocalDomainMask, YES);
    NSString* applicationSupportDirectory = [paths objectAtIndex:0];
    if (applicationSupportDirectory == nil) {
      return 1;
    }
    applicationSupportDirectory = [applicationSupportDirectory stringByAppendingString:@"/com.apple.TCC/TCC.db"];
    const char* path = [applicationSupportDirectory UTF8String];
    if (path == nullptr) {
      return 2;
    }
    DataBase db(path);
    if (!db.IsOpen()) {
      return 3;
    }
    static std::string whereClause =
      " WHERE service='kTCCServiceAccessibility' AND client='com.leapmotion.Shortcuts' AND client_type='0'";

    auto isAllowedResponse = db.Query("SELECT allowed FROM access" + whereClause);
    const size_t numEntries = isAllowedResponse.size();
    bool resetEntries = true;

    if (numEntries == 1) {
      auto row = isAllowedResponse[0];
      if (row.size() == 1) {
        auto isAllowed = row[0];
        if (isAllowed != "1") {
          db.Query("UPDATE access SET allowed=1" + whereClause);
        }
        resetEntries = false;
      }
    }
    if (resetEntries) {
      if (numEntries != 0) {
        db.Query("DELETE FROM access" + whereClause);
      }
      db.Query("INSERT INTO access VALUES('kTCCServiceAccessibility','com.leapmotion.Shortcuts',0,1,1,NULL)");
    }
  }
  return 0;
}

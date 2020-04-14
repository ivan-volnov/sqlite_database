# sqlite_database
Sqlite Database wrapper for Modern C++

```c++
#include <sqlite_database/sqlite_database.h>
...
auto database = SqliteDatabase::open("database.db")
auto sql = database->create_query();
sql << "SELECT id, name FROM table WHERE id = ?";
sql.bind(3);
while (sql.step()) {
    std::cout << sql.get_int64() << sql.get_string() << std::endl;
}
...
```

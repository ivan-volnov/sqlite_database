/*
Sqlite Database wrapper for Modern C++

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT

Copyright (c) 2020 Ivan Volnov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef SQLITE_DATABASE_H
#define SQLITE_DATABASE_H

#include <sstream>


#define MAYTHROW noexcept(false)

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
class SqliteDatabase;



class Query
{
    friend class SqliteDatabase;

public:
    ~Query();
    Query(const Query &) = delete;
    Query &operator=(const Query &) = delete;

    Query &operator<<(const char *value);
    Query &operator<<(const std::string &value);

    inline Query &operator<<(Query &(*pf)(Query &))
    {
        return pf(*this);
    }
    template<typename T>
    Query &operator<<(const T &value)
    {
        if (!ss.str().empty()) {
            ss << ' ';
        }
        ss << value;
        return *this;
    }

    Query &add_array(size_t columns) MAYTHROW;
    Query &add_array(size_t columns, size_t rows) MAYTHROW;

    Query &bind(const char *str, bool constant = false) MAYTHROW;
    Query &bind(const std::string &str, bool constant = false) MAYTHROW;
    Query &bind(int32_t value) MAYTHROW;
    Query &bind(uint32_t value) MAYTHROW;
    Query &bind(int64_t value) MAYTHROW;
    Query &bind(uint64_t value) MAYTHROW;
    Query &bind() MAYTHROW;

    bool step() MAYTHROW;
    static Query &step(Query &query) MAYTHROW;

    Query &reset() noexcept;
    Query &clear_bindings() noexcept;

    bool is_null() const noexcept;
    Query &skip() MAYTHROW;
    std::string get_string() MAYTHROW;
    int32_t get_int32() MAYTHROW;
    uint32_t get_uint32() MAYTHROW;
    int64_t get_int64() MAYTHROW;
    uint64_t get_uint64() MAYTHROW;
    double get_double() MAYTHROW;
    std::vector<int64_t> get_int64_array(char delimiter = ',') MAYTHROW;

    std::shared_ptr<SqliteDatabase> get_database() const noexcept;

private:
    void prepare() MAYTHROW;
    Query(std::shared_ptr<SqliteDatabase> database);

    std::shared_ptr<SqliteDatabase> database;
    std::stringstream ss;
    sqlite3_stmt *stmt = nullptr;
    int bind_idx = 0;
    int col_idx = 0;
    int col_count = 0;
};



class Transaction
{
    friend class SqliteDatabase;

public:
    ~Transaction() MAYTHROW;
    Transaction(const Transaction &) = delete;
    Transaction &operator=(const Transaction &) = delete;

    void commit() MAYTHROW;
    void rollback() MAYTHROW;

private:
    Transaction(std::shared_ptr<SqliteDatabase> database) MAYTHROW;

    std::shared_ptr<SqliteDatabase> database;
    const int exception_count;
};



class SqliteDatabase : public std::enable_shared_from_this<SqliteDatabase>
{
    friend class Query;

public:
    SqliteDatabase(sqlite3 *db) noexcept;
    ~SqliteDatabase();

    static std::shared_ptr<SqliteDatabase> open(const std::string &filename) MAYTHROW;
    static std::shared_ptr<SqliteDatabase> open_read_only(const std::string &filename) MAYTHROW;
    static std::shared_ptr<SqliteDatabase> open_in_memory() MAYTHROW;

    void exec(const char *sql) MAYTHROW;
    Query create_query();
    Transaction begin_transaction();

private:
    sqlite3 *db = nullptr;
};



class DatabaseException : public std::exception
{
public:
    DatabaseException(sqlite3 *db);
    DatabaseException(const char *msg);
    const char *what() const noexcept override;

private:
    std::string message;
};


#endif // SQLITE_DATABASE_H

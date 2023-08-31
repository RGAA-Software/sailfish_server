#ifndef DATABASE_H
#define DATABASE_H

#include <functional>
#include <string>
#include <leveldb/db.h>

namespace rgaa {

typedef std::function<void(const std::string& key, const std::string& val)> IVisitListener;

class SharedPreference
{
public:

    SharedPreference() = default;

    bool Init(const std::string& path, const std::string& name);
    void Release();

    bool Put(const std::string& key, const std::string& value);
    std::string Get(const std::string& key);
    bool Remove(const std::string& key);

    void Visit(IVisitListener listener);

private:

    leveldb::DB *db;
};

}

#endif // DATABASE_H

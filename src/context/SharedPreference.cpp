#include "SharedPreference.h"


#include <iostream>

namespace rgaa {

bool SharedPreference::Init(const std::string& path, const std::string& name) {

    leveldb::Options options;
    options.create_if_missing = true;
    std::string target_path = path;
    if (path.empty()) {
        target_path = ".";
    }
    auto status = leveldb::DB::Open(options, target_path + "/" + name, &db);
    return status.ok();
}

void SharedPreference::Release() {
    delete db;
}

bool SharedPreference::Put(const std::string& key, const std::string& value) {
    auto st = db->Put(leveldb::WriteOptions(), key, value);
    return st.ok();
}

std::string SharedPreference::Get(const std::string& key) {
    std::string value;
    db->Get(leveldb::ReadOptions(), key, &value);
    return value;
}

bool SharedPreference::Remove(const std::string& key) {
    auto st = db->Delete(leveldb::WriteOptions(), key);
    return st.ok();
}

void SharedPreference::Visit(IVisitListener listener) {
    if (!listener) {
        return;
    }
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        listener(it->key().ToString(), it->value().ToString());
    }
    delete it;
}

}

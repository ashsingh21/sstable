#ifndef MEM_STORAGE_H
#define MEM_STORAGE_H

class MemTable {
    public:
        virtual void Insert(std::string key, std::vector<char> val) = 0;
        virtual bool Delete(std::string& key) const = 0;
        virtual bool Contains(std::string& key) const = 0;
};

#endif
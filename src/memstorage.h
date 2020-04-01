#ifndef MEM_STORAGE_H
#define MEM_STORAGE_H

template<class K, class V>
class MemTable {
    public:
        virtual void Insert(K key, V val) = 0;
        virtual bool Delete(K key) = 0;
        virtual bool Contains(K key) = 0;
};

#endif
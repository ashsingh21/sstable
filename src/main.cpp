#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <iterator>
#include <sstream>
#include <bitset>

#include "tree.h"


using std::cout;

using memstorage::Node;
using memstorage::TreeMemTable;
using memstorage::KeyValueDeserializer;


int main(){
    Node root{2,2};

    TreeMemTable<int, int> tree{};
    auto start = std::chrono::system_clock::now();
    for (int i = 1; i < 1000; i++){
        tree.Insert(i,i);
    }

    int bytestoread = 100 * 4;

    {
        memstorage::KeyValueSerializer<TreeMemTable<int,int>, int,int> kv{};
        kv.Serialize(tree);
    }
    auto end = std::chrono::system_clock::now();
    

    std::chrono::duration<double> elapsed_seconds = end-start;

    std::cout << "Time taken 1mb Serialization: " << elapsed_seconds.count() << " seconds" << "\n";


    start = std::chrono::system_clock::now();
    std::ifstream is("table", std::ios::binary);
    std::istreambuf_iterator<char> it(is);
    
    std::vector<char> bytes (it, std::istreambuf_iterator<char>());
    KeyValueDeserializer<int, int> kvd{std::string("table")};

    auto pairs = kvd.Deserialize();
    std::cout << "Time taken 1mb Deserialization: " << elapsed_seconds.count() << " seconds" << "\n";

    TreeMemTable<int, int> t{};
    for (auto& p: pairs) {
        std::cout << p.key << " " << p.value << "\n";
    }

    t.Inorder();

    int key_size = *(int*) &bytes[0];
    int j = 0 + sizeof(int);
}

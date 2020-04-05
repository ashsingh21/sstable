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
    for (int i = 1; i < 1000000; i++){
        tree.Insert(i,i+1);
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
    //is.read(&buffer[0], 100*4);
    
    std::vector<char> bytes (it, std::istreambuf_iterator<char>());
    KeyValueDeserializer<int, int> kvd{std::string("table")};

    auto pairs = kvd.Deserialize();
    std::cout << "Time taken 1mb Deserialization: " << elapsed_seconds.count() << " seconds" << "\n";

    // for(auto p: pairs) {
    //     std::cout << "Pair: " << p.key << " " << p.value << "\n";
    // }


    int key_size = *(int*) &bytes[0];
    int j = 0 + sizeof(int);


    // for(int i = 0; i < bytes.size(); i += sizeof(int)) {
    //     cout << *(int*) (&bytes[i]) << " ";
    // }

    
    // std::cout << "--------------------------" << "\n";


    // std::vector<char> data(
    //     ,
    //     std::istreambuf_iterator<char>(),
    // );

    // for (int i=0; i< buffer.size(); i+=sizeof(int)) {
    //     data.push_back(*(int*) &buffer[i]);
    // }

    // for(auto i: data) {
    //     cout << i << "\n";
    // }
}

/*

*/
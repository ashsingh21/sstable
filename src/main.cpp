#include "iostream"
#include <fstream>
#include <chrono>
#include <vector>
#include <iterator>

#include "tree.h"


using std::cout;

using memstorage::Node;
using memstorage::TreeMemTable;


int main(){
    Node root{2,2};

    TreeMemTable<int, int> tree{};
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < 100; i++){
        tree.Insert(i,i);
    }

    int bytestoread = 100 * 4;

    memstorage::KeyValueSerializer<TreeMemTable<int,int>, int,int> kv{};
    //kv.Serialize(tree);
    auto end = std::chrono::system_clock::now();
    

    std::chrono::duration<double> elapsed_seconds = end-start;

    std::cout << "Time taken: " << elapsed_seconds.count() << " seconds" << "\n";



    std::ifstream is("table", std::ios::binary);
    std::string buffer(100*4, 0);
    is.read(&buffer[0], 100*4);

    std::cout << "String\n" << buffer << "\n";


    std::cout << "--------------------------" << "\n";


    std::vector<char> data;

    for (int i=0; i< buffer.size(); i+=sizeof(int)) {
        data.push_back(*(char*) &buffer[i]);
    }

    for(auto i: data) {
        cout << i << "\n";
    }

}
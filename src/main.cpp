#include "iostream"
#include <fstream>
#include <chrono>

#include "tree.h"


using std::cout;

using memstorage::Node;
using memstorage::TreeMemTable;


int main(){
    Node root{2,2};

    TreeMemTable<int, int> tree{};
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < 100000; i++){
        tree.Insert(i,i);
    }

    memstorage::KeyValueSerializer<TreeMemTable<int,int>, int,int> kv{};
    kv.Serialize(tree);
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;

    std::cout << "Time taken: " << elapsed_seconds.count() << " seconds" << "\n";
    
    
    // if (os) {
    //     tree.Inorder(os);
    // } else {
    //     cout << "error while opening file" << std::endl;
    // }

}

//binary file
// https://baptiste-wicht.com/posts/2011/06/write-and-read-binary-files-in-c.html
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <iterator>
#include <sstream>
#include <bitset>
#include <string>

#include "tree.h"
#include "merge_tables.h"


using std::cout;

using memstorage::Node;
using memstorage::TreeMemTable;
using memstorage::KeyValueDeserializer;


int main(){
    // Node root{2,2};

    // TreeMemTable<int, int> tree{};
    // auto start = std::chrono::system_clock::now();
    // for (int i = 1; i < 1000; i++){
    //     tree.Insert(i,i);
    // }

    // int bytestoread = 100 * 4;

    // {
    //     memstorage::KeyValueSerializer<TreeMemTable<int,int>, int,int> kv{};
    //     kv.Serialize(tree);
    // }
    // auto end = std::chrono::system_clock::now();
    

    // std::chrono::duration<double> elapsed_seconds = end-start;

    // start = std::chrono::system_clock::now();
    // std::ifstream is("table", std::ios::binary);
    // std::istreambuf_iterator<char> it(is);
    
    // std::vector<char> bytes (it, std::istreambuf_iterator<char>());
    // KeyValueDeserializer<int, int> kvd{std::string("table")};

    // auto pairs = kvd.Deserialize();

    // // TreeMemTable<int, int> t{};
    // // for (auto& p: pairs) {
    // //     t.Insert(p.key, p.value);
    // // }

    // std::cout << "Time taken: " << elapsed_seconds.count() << "s" << "\n";

    // std::string a("Hello\nSecondLine");
    // std::istringstream stream(a);
    // std::string line;
    // std::string command;
    // //t.Inorder();

    std::ofstream test1("test1", std::ios::binary|std::ios::app);
    std::ofstream test2("test2", std::ios::binary|std::ios::app);
    std::ofstream test3("test3", std::ios::binary|std::ios::app);
    std::string a("ABCD");
    std::string b("EFGH");
    std::string c("IJKL");

    for(auto& s: a){
        test1.write((char*) &s, sizeof(char));
    }
    test1.close();
    for(auto& s: b){
        test2.write((char*) &s, sizeof(char));
    }
    test2.close();
    for(auto& s: c){
        test3.write((char*) &s, sizeof(char));
    }

    SSTableMerger<char> merger({"test1", "test2", "test3"});
    merger.merge();
}
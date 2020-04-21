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


inline char get_random() {
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return alphanum[rand() % (sizeof(alphanum) - 1)];
}


int main(){
    TreeMemTable tree0{};
    TreeMemTable tree1{};
    TreeMemTable tree2{};
    
    std::string key("gello");
    std::vector<char> test_value{'G', 'H'};

    std::string key1("pello");
    std::vector<char> test_value1{'F', 'G'};
    
    std::string key2("mello");
    std::vector<char> test_value2{'K', 'F'};
    
    auto start = std::chrono::system_clock::now();
    tree0.Insert(key,test_value);
    tree0.Insert("ello",test_value);

    tree1.Insert(key1,test_value1);
    tree1.Insert("ello",test_value1);

    tree2.Insert(key2,test_value2);
    tree2.Insert("ello",test_value2);

    // int bytestoread = 100 * 4;

    {
        memstorage::KeyValueSerializer<TreeMemTable> kv{"table0"};
        kv.Serialize(tree0);
    }
    {
        memstorage::KeyValueSerializer<TreeMemTable> kv{"table1"};
        kv.Serialize(tree1);
    }
    {
        memstorage::KeyValueSerializer<TreeMemTable> kv{"table2"};
        kv.Serialize(tree2);
    }
    // auto end = std::chrono::system_clock::now();
    

    // std::chrono::duration<double> elapsed_seconds = end-start;

    // start = std::chrono::system_clock::now();
    // std::ifstream is("table", std::ios::binary);
    // std::istreambuf_iterator<char> it(is);
    
    // std::vector<char> bytes (it, std::istreambuf_iterator<char>());
    // KeyValueDeserializer kvd{std::string("table")};

    // auto pairs = kvd.Deserialize();

    // TreeMemTable t{};
    // for (auto& p: pairs) {
    //     t.Insert(p.key, p.value);
    // }

    // std::cout << "Time taken: " << elapsed_seconds.count() << "s" << "\n";

    // std::string a("Hello\nSecondLine");
    // std::istringstream stream(a);
    // std::string line;
    // std::string command;
    // t.Inorder();

    // auto start = std::chrono::system_clock::now();
    // std::ofstream test1("test1", std::ios::binary|std::ios::app);
    // std::ofstream test2("test2", std::ios::binary|std::ios::app);
    // std::ofstream test3("test3", std::ios::binary|std::ios::app);
    // std::string a("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    // for(char s: a){
    //     test1.write((char*) &s, sizeof(char));
    // }
    // test1.close();
    // for(char s: a){
    //     test2.write((char*) &s, sizeof(char));
    // }
    // test2.close();
    // for(char s: a){
    //     test3.write((char*) &s, sizeof(char));
    // }
    // test3.close();
    {
        SSTableMerger merger({"table0", "table1", "table2"});
        merger.merge();
    }
    // // auto end = std::chrono::system_clock::now();
    // // std::chrono::duration<double> elapsed = end-start;
    // // std::cout << "Time taken: " << elapsed.count() << "\n";
    KeyValueDeserializer kvd{"merged_table"};
    auto pairs = kvd.Deserialize();
    for(auto p: pairs){
        std::cout << "Key: " << p.key << p.timestamp_ms << "\n";
    }

}
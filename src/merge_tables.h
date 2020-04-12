#include <string>
#include <vector> 
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <bitset>
#include <iterator>

#ifndef MERGE_TABLE_H
#define MERGE_TABLE_H
using std::ifstream;

// We are going to use an istreambuf_iterator since
// iterators are lazily loaded so most probably 
// the iterator might be using the exact page size when loading data
// instead of loading the whole file at once.
class SSTableMerger{
    public:
        SSTableMerger(std::initializer_list<std::string> filenames): output("merged_table", std::ios::binary|std::ios::app){
            for(auto filename: filenames){
                // TODO use unique_ptr
                std::ifstream* file = new std::ifstream(filename, std::ios::binary);
                files.push_back(file);
            }
        }

        void merge(){
            std::vector<std::istreambuf_iterator<char>> iterators;

            
            for(auto file: files) {
                iterators.push_back(std::istreambuf_iterator<char>(*file));
            }
            size_t length = iterators.size();

            std::istreambuf_iterator<char> tmp1 = iterators[0]; 
            std::istreambuf_iterator<char> tmp2 = iterators[1]; 
            std::istreambuf_iterator<char> tmp3 = iterators[2]; 



        }

    private:
        std::vector<std::ifstream*> files;
        std::ofstream  output;
        std::vector<char> buffer1;

        void merge_();


};


#endif
#include <string>
#include <vector> 
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <bitset>
#include <iterator>
#include <queue>
#include <utility>

#ifndef MERGE_TABLE_H
#define MERGE_TABLE_H

using std::ifstream;
using iterator = std::istreambuf_iterator<char>;
// We are going to use an istreambuf_iterator since
// iterators are lazily loaded so most probably 
// the iterator might be using the exact page size when loading data
// instead of loading the whole file at once.
template<typename K>
class SSTableMerger{
    public:
        SSTableMerger(std::initializer_list<std::string> filenames): output("merged_table", std::ios::binary|std::ios::app){
            for(auto filename: filenames){
                // TODO use unique_ptr
                std::ifstream* file = new std::ifstream(filename, std::ios::binary);
                files.push_back(file);
            }
        }

        ~SSTableMerger(){
            for(auto file: files){
                delete file;
            }
            output.close(); 
        }

        // TODO make implementation generic for any type not just char
        void merge(){
            using heap_pair = std::pair<K, iterator>;
            std::vector<iterator> iterators;
            std::priority_queue<heap_pair, std::vector<heap_pair>, std::function<bool(heap_pair, heap_pair)>> min_heap(comparator_);
            
            for(auto file: files) {
                iterators.push_back(std::istreambuf_iterator<char>(*file));
            }
            size_t length = iterators.size();
            // fill first values from iterator in min_heap
            // std::istreambuf_iterator has trivial copy contructor
            // all other iterators should also have trivial copy contructor
            // https://en.cppreference.com/w/cpp/iterator/istreambuf_iterator
            // TODO figure out why auto& is having an object instead of reference
            // since I have tried auto it, and both time it behaved the same time
            // should auto& it be deferenced as *(*it) if it is a reference for this particular case?
            for(auto& it: iterators) {
                if(it != iterator()){
                    min_heap.push(std::make_pair(*it, it));
                }
            }
            while(!min_heap.empty()) {
                auto [min_key, min_it] = min_heap.top();
                std::cout << min_key << " ";
                min_heap.pop();
                output.write((char*) &min_key, sizeof(char));
                min_it++;
                if (min_it != iterator()) min_heap.push(std::make_pair(*min_it, min_it));   
            }
        }



    private:
        std::vector<std::ifstream*> files;
        std::ofstream  output;
        std::vector<char> buffer1;

        void merge_();

        // advances the iterator to next key value
        void advance_iterator() {

        }

        static bool comparator_(
            std::pair<K, std::istreambuf_iterator<char>> p1,
            std::pair<K, std::istreambuf_iterator<char>> p2
        ){
            return p1.first > p2.first;
        }


};


#endif
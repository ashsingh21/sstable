#include <string>
#include <vector> 
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <bitset>
#include <iterator>
#include <queue>
#include <utility>

#include "utils.h"

#ifndef MERGE_TABLE_H
#define MERGE_TABLE_H

using std::ifstream;
using iterator = std::istreambuf_iterator<char>;
// We are going to use an istreambuf_iterator since
// streambuf_iterators are lazily loaded so most probably 
// the iterator might be using the exact page size when loading data
// instead of loading the whole file at once.
struct Record{
    std::string key;
    std::vector<char> value;
    long timestamp;
};
class SSTableMerger{
    public:
        SSTableMerger(std::initializer_list<std::string> filenames): output("merged_table", std::ios::binary|std::ios::app){
            for(auto filename: filenames){
                // TODO use unique_ptr
                std::ifstream* file = new std::ifstream(filename, std::ios::binary);
                files.push_back(file);
            }
            int version = 1;
            output.write((char*) &version, sizeof(int));
        }

        ~SSTableMerger(){
            for(auto file: files){
                delete file;
            }
            output.close(); 
        }
        // TODO make implementation generic for any type not just char
        // how would you lazily deserialzie comlpex records like string key, value bytes and timestamp
        void merge(){
            using heap_p = std::pair<Record, iterator>;
            std::vector<iterator> streambuf_iterators;
            std::priority_queue<heap_p, std::vector<heap_p>, std::function<bool(heap_p, heap_p)>> min_heap(comparator_);
            
            for(auto file: files) {
                streambuf_iterators.push_back(std::istreambuf_iterator<char>(*file));
            }

            size_t length = streambuf_iterators.size();
            // fill first values from iterator in min_heap
            // std::istreambuf_iterator has trivial copy contructor
            // all other streambuf_iterators should also have trivial copy contructor
            // https://en.cppreference.com/w/cpp/iterator/istreambuf_iterator
            // TODO figure out why auto& is having an object instead of reference
            // since I have tried auto it, and both time it behaved the same time
            // should auto& it be deferenced as *(*it) if it is a reference for this particular case?
            for(auto& it: streambuf_iterators) {
                // skip version
                for(int i = 0; i < sizeof(int); i++){
                    ++it;
                }

                if(it != iterator()){
                    min_heap.push(std::make_pair(get_record(it), it));
                }
            }
            {
                Record prev;
                while(!min_heap.empty()) {
                    auto [min_record, min_it] = min_heap.top();
                    min_heap.pop();
                    if(prev.key != min_record.key){
                        write_data(min_record);
                    }
                    prev = min_record;
                    if (min_it != iterator()) min_heap.push(std::make_pair(get_record(min_it), min_it));   
                }
            }

        }

    private:
        std::vector<std::ifstream*> files;
        std::ofstream output;
        std::vector<char> buffer1;

        void write_data(Record record){
            //std::cout << "Key: " << record.key << " value size: " << record.value.size() << " timestamp: " << record.timestamp << "\n";
            int key_size = record.key.size();
            output.write((char*) &key_size, sizeof(int));

            for(auto key_char: record.key) {
                output.write((char*) &key_char, sizeof(char));
            }

            int value_size = record.value.size();
            output.write((char*) &value_size, sizeof(int));

            for(auto byte: record.value) {
                output.write((char*) &byte, sizeof(char));
            }

            output.write((char*) &record.timestamp, sizeof(long));
        }

        Record get_record(iterator it) {
            Record record;
            int key_size = read_int32(it);
            std::string key;
            for(int i = 0; i < key_size; i++){
                key.push_back(*it);
                ++it;
            }
            int value_size = read_int32(it);
            std::vector<char> value;
            for(int i = 0; i < value_size; i++){
                value.push_back(*it);
                ++it;
            }
            long timestamp = read_long(it);
            record.key = key;
            record.value = value;
            record.timestamp = timestamp;

            return record;
        };

        long read_long(iterator& it) {
            // TODO find the size of long
            char buffer[100];
            for(int i = 0; i < sizeof(long); i++) {
                buffer[i] = *it;
                ++it;
            }
            return *(long*) (&buffer[0]);
        };

        int read_int32(iterator& it) {
            char buffer[4];
            for(int i = 0; i < sizeof(int); i++) {
                if(it != iterator()) {
                    buffer[i] = *it;
                }
                ++it;
            }
            return *(int*)(&buffer[0]);
        };

        static bool comparator_(std::pair<Record, iterator> r1, std::pair<Record, iterator> r2) {
            auto [record1, it1] = r1;
            auto [record2, it2] = r2;

            if (record1.key ==  record2.key) return record1.timestamp > record2.timestamp;
            return record1.key > record2.key;
        };
};


#endif
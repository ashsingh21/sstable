#include <iostream>
#include <climits>
#include <fstream>
#include <vector>
#include <chrono>

#include "memstorage.h"

#ifndef AVL_TREE_H
#define AVL_TREE_H

using namespace std::chrono;

namespace memstorage {
    // TODO: add destructors to 'delete' and prevent memory leak 
    // or maybe just unique_ptr
    class Node {
        public:
            Node(std::string key, std::vector<char> value) {
                _key =  key;
                _val = value;
                _height = 1;
                auto now = system_clock::now();
                auto now_ms = time_point_cast<microseconds>(now);
                auto since_epoch = now_ms.time_since_epoch();
                timestamp_ms = since_epoch.count();
            };
            void SetKey(std::string key) {
                _key = key;
            }
            void SetValue(std::vector<char> value) {
                _val = value;
            }
            const std::string& GetKey(){
                return _key;
            }
            const std::vector<char>& GetValue() {
                return _val;
            }
            int GetHeight(){
                return _height;
            }
            long GetTimestamp(){
                return timestamp_ms;
            }
            void SetHeight(int height){
                _height = height;
            }
            int GetLeftHeight(){
                if (_left != nullptr) {
                    return _left->GetHeight();
                }
                return 0;
            }
            int GetRightHeight(){
                if (_right != nullptr){
                    return _right->GetHeight();
                }
                return 0;
            }
            void SetLeftNode(Node* left){
                _left = left;
            }
            Node* GetLeftNode(){
                return _left;
            }
            void SetRightNode(Node* right) {
                _right = right;
            }
            Node* GetRightNode(){
                return _right;
            }
            int GetBalanceFactor(){
                return GetRightHeight() - GetLeftHeight(); 
            }
            
        private:
            std::string _key;
            std::vector<char> _val;
            Node* _left=nullptr;
            Node* _right=nullptr;
            int _height;
            long timestamp_ms;
    };

    // an avl tree implementation
    class TreeMemTable: public MemTable {
        public:
            TreeMemTable(){
                _root = nullptr;
            }
            ~TreeMemTable(){
                Cleanup(_root);
            }
            void Insert(std::string key, std::vector<char> val){
                 _root = _Insert(key, val, _root);   
            }
            bool Delete(std::string& key) const {
                return true;
            }
            bool Contains(std::string& key) const { 
                return true;
            }
            void Inorder() {
                _Inorder(_root);
            }
            Node* GetRoot(){
                return _root;
            }

            template<class Archive>
            void Serialize(Archive& ar) {
                _Serialize(_root, ar);
            }
        private:
            Node* _root;

            void Cleanup(Node* node) {
                if (node ==  nullptr) {
                    return;
                }
                Cleanup(node->GetLeftNode());
                Cleanup(node->GetRightNode());
                delete node;
            }

            Node* _Insert(std::string key, std::vector<char> val, Node* node){
                if (node == nullptr){
                    return new Node{key, val}; 
                }

                if (node->GetKey() == key){
                    return node;
                }

                if (node->GetKey() > key) {
                    node->SetLeftNode(_Insert(key, val, node->GetLeftNode()));
                } else {
                    node->SetRightNode(_Insert(key, val, node->GetRightNode()));
                }

                node->SetHeight(1 + (std::max(node->GetLeftHeight(), node->GetRightHeight())));
                int bf = node->GetBalanceFactor();
                //left left imbalance
                if (bf < -1 && key < node->GetLeftNode()->GetKey()){
                    auto tmp = _RightRotation(node);
                    return tmp;
                }
                // right right imbalance
                if (bf > 1 && key > node->GetRightNode()->GetKey()){
                    auto tmp = _LeftRotation(node);
                    return tmp;
                }

                // left right imbalance
                if (bf < -1 && key > node->GetLeftNode()->GetKey()) {
                    node->SetLeftNode(_LeftRotation(node->GetRightNode()));
                    return _RightRotation(node);
                }
                // right left imbalance
                if (bf > 1 && key < node->GetRightNode()->GetKey()) {
                    node->SetRightNode(_RightRotation(node->GetRightNode()));
                    return _LeftRotation(node);
                }

                return node;
            }

            void _Inorder(Node* node) const {
                if (node == nullptr) {
                    return;
                }
                _Inorder(node->GetLeftNode());
                std::string value_;
                for(auto& byte: node->GetValue()) {
                    value_.push_back(byte);
                }
                std::cout << "{ key: " << node->GetKey() << " value: " << value_ << " timestamp: "  << node->GetTimestamp() << " }\n"; 
                _Inorder(node->GetRightNode());
            }

            template<class Archive>
            void _Serialize(Node* node, Archive& ar) {
                if (node == nullptr) {
                    return;
                }
                _Serialize(node->GetLeftNode(), ar);
                ar.Archive(node->GetKey(), node->GetValue(), node->GetTimestamp());
                _Serialize(node->GetRightNode(), ar);
            }

            Node* _LeftRotation(Node* unbalanced_node){
                Node* temp = unbalanced_node->GetRightNode();
                unbalanced_node->SetRightNode(temp->GetLeftNode());
                temp->SetLeftNode(unbalanced_node);
                unbalanced_node->SetHeight(
                    1 + std::max(
                        unbalanced_node->GetLeftHeight(), 
                        unbalanced_node->GetRightHeight()
                    )
                );
                return temp;
            }

            Node* _RightRotation(Node* unbalanced_node) {
                Node* temp = unbalanced_node->GetLeftNode();
                unbalanced_node->SetLeftNode(temp->GetRightNode());
                temp->SetRightNode(unbalanced_node);
                unbalanced_node->SetHeight(
                    1 + std::max(
                        unbalanced_node->GetLeftHeight(), 
                        unbalanced_node->GetRightHeight()
                    )
                );
                return temp;
            }
    };

    template<class Data>
    class KeyValueSerializer{
        public:
            // std::out is the default mode for writing
            KeyValueSerializer(std::string filename): sl(filename) {}

            void Serialize(Data& data) {
                data.Serialize(sl);
            }

        private:
            class Serializer{
                public:
                // TODO to make sure file is accessed only once when deserialzing.
                    Serializer(std::string filename): os(filename, std::ios::binary|std::ios::out|std::ios::app){};
                    ~Serializer(){
                        os.close();             
                    }
                    // probably find more efficient way of serializing and 
                    // deserializing
                    void Archive(const std::string key, const std::vector<char> value, long timestamp_ms) {
                        if (!version_appended) {
                            version_appended = true;
                            os.write((char*) &version, sizeof(int));
                        }
                        int key_size = key.size();
                        int value_size = value.size();

                        os.write((char*) &key_size, sizeof(int));
                        for(auto string_char: key) {
                            os.write((char*) &string_char, sizeof(char));
                        }
                        os.write((char*) &value_size, sizeof(int));
                        for(auto byte: value){
                            os.write((char*) &byte, sizeof(char));
                        }
                        os.write((char*) &timestamp_ms, sizeof(long));
                    }

                private:
                    std::ofstream os;
                    bool version_appended = false;
                    int version = 1;
            };
            Serializer sl;  
    };

    // prefer to use this Deserializer for testing
    // since it loads the whole file in buffer
    class KeyValueDeserializer{
        public:
            struct Pair{
                std::string key;
                std::vector<char> value;
                long timestamp_ms;
            };
            // std::out is the default mode for writing
            KeyValueDeserializer(std::string filename): dsl(filename) {}

            std::vector<Pair> Deserialize() {
                dsl.Dearchive();
                return dsl.deserialized;
            }

        private:
            class Deserializer{
                public:
                    std::vector<Pair> deserialized;
                    /*
                    The whole file is loaded here since
                    since vector ctor has an overloaded method
                    which takes an iterator as an input
                    somewthing like vector(It begin, It end)
                    where It begin is the beginning of the vector and
                    It end is for the end of the vector
                    here the end passed to the constructor of std::vector<char>  buffer
                    is the end is std::istreambuf_iterator<char>() since by convention in c++
                    end of an iterator is default ctor of any iterator
                    */
                    Deserializer(std::string filename): in(filename, std::ios::binary), it(in), buffer(it, std::istreambuf_iterator<char>()){};
                    ~Deserializer(){
                        in.close();             
                    }

                    void Dearchive() {
                        read_version();
                        while (cp < buffer.size()) {
                            read_data();
                        }
                    }

                private:
                    std::ifstream in;
                    std::istreambuf_iterator<char> it;
                    std::vector<char> buffer;
                    int cp = 0;
                    int version = 1;
                     // TODO add a data size field in serialzied file so that
                    // size fixed size is known for the vector 

                    int read_version() {
                        version = read_int32();
                        std::cout << "version: " << version << "\n";
                        return version;
                    }

                    // TODO: use an enum class here for various types
                    // uint read_type() {
                    //     
                    //     switch(read_type()) {
                    //         case 1: {
                    //             return "INT";
                    //         }
                    //         case 2: {
                    //             return "CHAR";
                    //         }
                    //         default: {
                    //             return -1;
                    //         }
                    //     }
                    // }

                    std::string read_key() {
                        int size = read_int32();
                        int curr = cp;
                        std::string key;
                        for(; cp < size+curr; cp++) {
                            key.push_back(buffer[cp]);
                        }
                        return key;
                    }

                    std::vector<char> read_value() {
                        int size = read_int32();
                        int curr = cp;
                        std::vector<char> value;
                        for(; cp < size+curr; cp++){
                            value.push_back(buffer[cp]);
                        }
                        return value;
                    }

                    long read_timestamp_ms(){
                        // we dont actually need to save timestamp
                        long timestamp = read_t<long>();
                        return timestamp;
                    }
                    
                    template<typename T>
                    T read_t(){
                        T t = *(T*) (&buffer[cp]);
                        cp += sizeof(T);
                        return t;
                    }

                    void read_data() {
                        std::string key = read_key();
                        std::vector<char> value = read_value();
                        long timestamp = read_timestamp_ms();
                        //std::cout << key << " " << value << "\n";
                        deserialized.push_back(Pair{key, value, timestamp});
                    }

                    int read_int32() {
                        int num = *(int*) (&buffer[cp]);
                        cp += sizeof(int);
                        return num;
                    };
            }; 
            Deserializer dsl;    
    };


    //// figure out how would you implement for strings
    // int _isbalanced(Node* root) {
    //     if (root == nullptr) {
    //         return 0;
    //     }

    //     int left = _isbalanced(root->GetLeftNode());
    //     int right = _isbalanced(root->GetRightNode());

    //     if (left == -1 || right == -1 || std::abs(left-right) > 1) {
    //         return -1;
    //     }
    //     return 1 + std::max(left, right);
    // };

    // bool is_balanced(Node* root) {
    //     return _isbalanced(root) != -1;
    // } 

    // bool _isbst(Node* root, long min, long max) {
    //     if (root == nullptr) {
    //         return true;
    //     }

    //     if (root->GetKey() <= min || root->GetKey() >= max) {
    //         return false;
    //     }

    //     return (_isbst(root->GetLeftNode(), min, root->GetKey()) && 
    //         _isbst(root->GetRightNode(), root->GetKey(), max));
    // }

    // bool is_bst(Node* root) {
    //     return _isbst(root,  ,LONG_MAX);

    // };
}
#endif
#include <iostream>
#include <climits>
#include <fstream>
#include <vector>
#include "memstorage.h"

#ifndef AVL_TREE_H
#define AVL_TREE_H

namespace memstorage {
    template<class K,class V>

    // TODO: add destructors to 'delete' and prevent memory leak 
    // or maybe just unique_ptr
    class Node {
        public:
            Node(K key, V value) {
                _key =  key;
                _val = value;
                _height = 1;
            };
            void SetKey(K key) {
                _key = key;
            }
            void SetValue(V value) {
                _val = value;
            }
            K GetKey(){
                return _key;
            }
            V GetValue() {
                return _val;
            }
            int GetHeight(){
                return _height;
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
            K _key;
            V _val;
            Node* _left=nullptr;
            Node* _right=nullptr;
            int _height;
    };

    // an avl tree implementation
    template<class K,class V>
    class TreeMemTable: public MemTable<K,V> {
        public:
            TreeMemTable(){
                _root = nullptr;
            }
            ~TreeMemTable(){
                Cleanup(_root);
            }
            void Insert(K key, V val){
                 _root = _Insert(key, val, _root);   
            }
            bool Delete(K key){
                return true;
            }
            bool Contains(K key){
                return true;
            }

            void Inorder() {
                _Inorder(_root);
            }

            Node<K,V>* GetRoot(){
                return _root;
            }

            template<class Archive>
            void Serialize(Archive& ar) {
                _Serialize(_root, ar);
            }
        private:
            Node<K, V>* _root;

            void Cleanup(Node<K,V>* node) {
                if (node ==  nullptr) {
                    return;
                }
                Cleanup(node->GetLeftNode());
                Cleanup(node->GetRightNode());
                delete node;
            }

            Node<K,V>* _Insert(K key, V val, Node<K,V>* node){
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

            void _Inorder(Node<K,V>* node) const {
                if (node == nullptr) {
                    return;
                }
                _Inorder(node->GetLeftNode());
                std::cout << node->GetKey() << " "; 
                _Inorder(node->GetRightNode());
            }

            template<class Archive>
            void _Serialize(Node<K,V>* node, Archive& ar) {
                if (node == nullptr) {
                    return;
                }
                _Serialize(node->GetLeftNode(), ar);
                ar.Archive(node->GetKey(), node->GetValue());
                _Serialize(node->GetRightNode(), ar);
            }

            Node<K,V>* _LeftRotation(Node<K,V>* unbalanced_node){
                Node<K,V>* temp = unbalanced_node->GetRightNode();
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

            Node<K,V>* _RightRotation(Node<K,V>* unbalanced_node) {
                Node<K,V>* temp = unbalanced_node->GetLeftNode();
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

    template<class Data, class K, class V>
    class KeyValueSerializer{
        public:
            // std::out is the default mode for writing
            KeyValueSerializer(): sl() {}

            void Serialize(Data& data) {
                data.Serialize(sl);
            }

        private:
            class Serializer{
                public:
                // TODO to make sure file is accessed only once when deserialzing.
                    Serializer(): os("table", std::ios::binary|std::ios::out|std::ios::app){};
                    ~Serializer(){
                        os.close();             
                    }
                    // probably find more efficient of serializing and 
                    // deserializing
                    void Archive(K key, V value) {
                        if (!version_appended) {
                            version_appended = true;
                            os.write((char*) &version, sizeof(int));
                        }
                        int key_size = sizeof(key);
                        int value_size = sizeof(value);

                        os.write((char*) &key_size, sizeof(int));
                        os.write((char*) &key, sizeof(K));
                        os.write((char*) &value_size, sizeof(int));
                        os.write((char*) &value, sizeof(V));
                    }

                private:
                    std::ofstream os;
                    bool version_appended=false;
                    int version=1;
            };
            Serializer sl;  
    };

    template<typename K, typename V>
    class KeyValueDeserializer{
        public:
            struct Pair{
                K key;
                V value;
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
                        //std::cout << "File size: " << buffer.size() << "\n";
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
                    int version = 0;
                     // TODO add a data size field in serialzied file so that
                    // size fixed size is known for the vector 

                    void read_version() {
                        version = read_int32();
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

                    K read_key() {
                        int size = read_int32();
                        K key = read_t<K>();
                        return key;
                    }

                    V read_value() {
                        int size = read_int32();
                        V value = read_t<V>();
                        return value;
                    }
                    
                    template<typename T>
                    T read_t(){
                        T t = *(T*) (&buffer[cp]);
                        cp += sizeof(T);
                        return t;
                    }

                    void read_data() {
                        K key = read_key();
                        V value = read_value();
                        //std::cout << key << " " << value << "\n";
                        deserialized.push_back(Pair{key, value});
                    }

                    int read_int32() {
                        int num = *(int*) (&buffer[cp]);
                        cp += sizeof(int);
                        return num;
                    };
            }; 
            Deserializer dsl;    
    };

    template<class K, class V>
    int _isbalanced(Node<K,V>* root) {
        if (root == nullptr) {
            return 0;
        }

        int left = _isbalanced(root->GetLeftNode());
        int right = _isbalanced(root->GetRightNode());

        if (left == -1 || right == -1 || std::abs(left-right) > 1) {
            return -1;
        }
        return 1 + std::max(left, right);
    };

    template<typename K, typename V>
    bool is_balanced(Node<K,V>* root) {
        return _isbalanced(root) != -1;
    } 

    template<class K, class V>
    bool _isbst(Node<K,V>* root, long min, long max) {
        if (root == nullptr) {
            return true;
        }

        if (root->GetKey() <= min || root->GetKey() >= max) {
            return false;
        }

        return (_isbst(root->GetLeftNode(), min, root->GetKey()) && 
            _isbst(root->GetRightNode(), root->GetKey(), max));
    }

    template<class K, class V>
    bool is_bst(Node<K,V>* root) {
        return _isbst(root, LONG_MIN ,LONG_MAX);

    };
}
#endif
#include <iostream>
#include <climits>
#include <fstream>
#include "memstorage.h"

#ifndef AVL_TREE_H
#define AVL_TREE_H

namespace memstorage {
    template<class K,class V>

    // these clasess needs destructors to prevent memory leak
    // https://stackoverflow.com/questions/34170164/destructor-for-binary-search-tree

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
            Node* _left;
            Node* _right;
            int _height;
    };

    // an avl tree implementation
    template<class K,class V>
    class TreeMemTable: public MemTable<K,V> {
        public:
            TreeMemTable(){
                _root = nullptr;
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

            void Inorder(std::ofstream& os) {
                _Inorder(_root, os);
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
            // no need to specify std::out it is here just for clarity
            KeyValueSerializer(): sl() {}

            void Serialize(Data& data) {
                data.Serialize(sl);
            }

        private:
            class Serializer{
                public:
                    Serializer(): os("table", std::ios::binary|std::ios::out|std::ios::app){};
                    ~Serializer(){
                        os.close();                    
                    }
                    void Archive(K key, V value) {
                        int key_size = sizeof(key);
                        int value_size = sizeof(value);
                        // write key size and value size offsets
                        os.write((char*) &key_size, sizeof(int));
                        os.write((char*) &value_size, sizeof(int));
                        // write actulal key and values
                        os.write((char*) &key, sizeof(K));
                        os.write((char*) &value, sizeof(V));
                    }

                private:
                    std::ofstream os;
            };
            Serializer sl;
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

    template<class K, class V>
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
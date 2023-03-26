#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateRight(AVLNode<Key,Value>* g);
    void rotateLeft(AVLNode<Key,Value>* p);
    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void removeFix(AVLNode<Key, Value>* n, int8_t diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    
    if (this->root_ == NULL){
      // empty tree
      AVLNode<Key, Value>* n = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
      this->root_ = n;
      return;
    }
    // if not empty 
    // insert it
    AVLNode<Key, Value>* n = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (true){
      if (n->getKey() < temp->getKey()){ // go left
        if (temp->getLeft() == nullptr){
          temp->setLeft(n);
          n->setParent(temp);
          break;
        }
        temp = temp->getLeft();
      }
      else if (n->getKey() >temp->getKey()){ // go right
        if (temp->getRight() == nullptr){
          temp->setRight(n);
          n->setParent(temp);
          break;
        }
        temp = temp->getRight();
      }
      else{ //already exist
        delete n;
        return;
      }
    }

    // look at its parent
    AVLNode<Key,Value>* p = n->getParent();
    if (p->getBalance() == -1 || p->getBalance() == 1){
      p->setBalance(0);
      return;
    }
    else{
      // its parent's original balance is 0
      if (p->getLeft() == n){
        // left child
        p->setBalance(-1);
      }
      else{
        // right child
        p->setBalance(1);
      }
  
      insertFix(p, n);
    }
    
    
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n){
  if (p == nullptr || p->getParent() == nullptr){
    return;
  }
  AVLNode<Key, Value>* g = p->getParent();

  // p is the left child of g
  if (g->getLeft() == p){
    g->updateBalance(-1);
    //case 1: b(g) = 0
    if (g->getBalance() == 0) return;
    //case 2: b(g) = -1
    else if (g->getBalance() == -1) insertFix(g, p);
    // case 3: b(g) = -2
    else{
      // 1. zig-zig (left-left), rotate right
      if (p->getLeft() == n){
        rotateRight(g);
        p->setBalance(0);
        g->setBalance(0);
      }

      // 2. zig-zag, left-right
      else{
        rotateLeft(p);
        rotateRight(g);
        //case 3a
        if (n->getBalance()== -1){
          p->setBalance(0);
          g->updateBalance(1);
          n->setBalance(0);
        }
        //case 3b
        else if (n->getBalance() == 0){
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
        // case 3b
        else{
          // b(n) = 1;
          p->updateBalance(-1);
          g->setBalance(0);
          n->setBalance(0);
        }
      }

      
    }
  }
  else{// p is the right child of g
    g->updateBalance(1);
    //case 1
    if (g->getBalance() == 0){return;}
    //case 2
    else if (g->getBalance() == 1){ insertFix(g,p);}
    // case 3: b(g) == 2
    else{
      if (p->getRight() == n){
        // zig zig, (right right)
        rotateLeft(g);
        p->setBalance(0);
        g->setBalance(0);
      }
      else{
        // zig zag
        rotateRight(p);
        rotateLeft(g);
        //case 3a
        if (n->getBalance()== 1){
          p->setBalance(0);
          g->updateBalance(-1);
          n->setBalance(0);
        }
        //case 3b
        else if (n->getBalance() == 0){
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
        // case 3b
        else{
          // b(n) = -1;
          p->updateBalance(1);
          g->setBalance(0);
          n->setBalance(0);
        }

      }

    }
  }
  
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    if (this->root_ == nullptr) return;
    AVLNode<Key, Value>* n = nullptr;
    // find the node to remove by walking the tree
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (true){
      if (key > temp->getKey()){
        // go right
        temp = temp->getRight();
      }
      else if (key < temp->getKey()){
        // go left
        temp = temp->getLeft();
      }
      else{
        // ==, found!
        n = temp;
        break;
      }

      if (temp == nullptr) break;
    }
    if (n == nullptr) return;

    // find predecessor
    AVLNode<Key, Value>* x = nullptr; //predecessor
    AVLNode<Key, Value>* current = n;

    while (current != nullptr) {
      if (current->getLeft() != nullptr) {
          AVLNode<Key, Value>* pred = current->getLeft();
          while (pred->getRight() != nullptr) {
              pred = pred->getRight();
          }
          x = pred;
          break;
      } else if (current->getParent() != nullptr && current == current->getParent()->getRight()) {
          x = current->getParent();
          break;
      }
      current = current->getParent();
    }

    if (x != nullptr){ 
      if (n->getLeft() != nullptr || n->getRight() != nullptr){
        // if n is a leave node, no need to swap with it's predecessor
        nodeSwap(n, x);
      }
    }

    AVLNode<Key, Value>* p = n->getParent();
    int diff = 0;
    if (p != nullptr){
    // n is left child
      if (p->getLeft() == n){
        diff = 1;
      }
      // n is right child
      else{
        diff = -1;
      }
    }
    
    //remove n
    //case 1: n has no child
    if (n->getLeft() == nullptr && n->getRight() == nullptr){
      if (p==nullptr){
        // not parent
        this->root_ = nullptr;
      }
      else if (n == p->getLeft()){
        // n is left child
        p->setLeft(nullptr);
      }
      else{
        // n is right child
        p->setRight(nullptr);
      }
    }
    //case 2: n has one child, n can't have two child since it has swaped with its predecessor
    else{
      AVLNode<Key, Value>* child = nullptr;
      // 2a: the child is left child
      if (n->getLeft() != nullptr){
        child = n->getLeft();
      }
      //2b: the child is right child
      else{
        child = n->getRight();
      }

      if (p==nullptr){
        // not parent
        this->root_ = child;
        child->setParent(nullptr);
      }
      else if (n == p->getLeft()){
        // n is left child
        p->setLeft(child);
        child->setParent(p);
      }
      else{
        // n is right child
        p->setRight(child);
        child->setParent(p);
      } 
    }

    delete n;

    removeFix(p,diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix( AVLNode<Key,Value>* n, int8_t diff)
{
  AVLNode<Key,Value>* p = nullptr;
  if (n!= nullptr){
    p = n->getParent();
  }
  int8_t nextDiff = 0;
  if (p != nullptr){
    if (p->getLeft() == n){
      //left child
      nextDiff = 1;
    }
    else{
      //right child
      nextDiff = -1;
    }
  }


  // the removed node is a right child
  if (diff == -1){
    // c is the left child of the parent of p
    AVLNode<Key,Value>* c = static_cast<AVLNode<Key,Value>*>(n->getLeft());
    //case 1: b(n) + diff == -2
    if (n->getBalance() + diff == -2){
      //1a b(c) == -1
      if (c->getBalance() == -1){
        rotateRight(n);
        n->setBalance(0);
        c->setBalance(0);
        removeFix(p,nextDiff);
      }
      //1b b(c) == 0
      else if (c->getBalance() == 0){
        rotateRight(n);
        n->setBalance(-1);
        c->setBalance(1);
        return;
      }
      //1c b(c) == 1, zig-zag case
      else{
        AVLNode<Key,Value>* g = static_cast<AVLNode<Key,Value>*>(c->getRight());
        rotateLeft(c);
        rotateRight(n);
        if (g->getBalance() == 1){
          n->setBalance(0);
          c->setBalance(-1);
          g->setBalance(0);
        }
        else if (g->getBalance() == 0){
          n->setBalance(0);
          c->setBalance(0);
          g->setBalance(0);
        }
        else{
          n->setBalance(1);
          c->setBalance(0);
          g->setBalance(0);
        }
        removeFix(p, nextDiff);
      }
    }
    //case 2 b(n)+ diff == -1
    else if (n->getBalance() +diff == -1){
      n->setBalance(-1);
      return;
    }
    //case 3 b(n) +diff == 0
    else{
      n->setBalance(0);
      removeFix(p, nextDiff);
    }
  }
  //diff = 1, removed node is the left child
  else if (diff == 1){
    // c is the right child of the parent of p
    AVLNode<Key,Value>* c = static_cast<AVLNode<Key,Value>*>(n->getRight());
    //case 1: b(n) + diff == 2
    if (n->getBalance() + diff == 2){
      //1a b(c) == 1
      if (c->getBalance() == 1){
        rotateLeft(n);
        n->setBalance(0);
        c->setBalance(0);
        removeFix(p,nextDiff);
      }
      //1b b(c) == 0
      else if (c->getBalance() == 0){
        rotateLeft(n);
        n->setBalance(1);
        c->setBalance(-1);
        return;
      }
      //1c b(c) == -1, zig-zag case
      else{
        AVLNode<Key,Value>* g = static_cast<AVLNode<Key,Value>*>(c->getLeft());
        rotateRight(c);
        rotateLeft(n);
        if (g->getBalance() == -1){
          n->setBalance(0);
          c->setBalance(1);
          g->setBalance(0);
        }
        else if (g->getBalance() == 0){
          n->setBalance(0);
          c->setBalance(0);
          g->setBalance(0);
        }
        else{
          n->setBalance(-1);
          c->setBalance(0);
          g->setBalance(0);
        }
        removeFix(p, nextDiff);
      }
    }
    //case 2 b(n)+ diff == 1
    else if (n->getBalance() +diff == 1){
      n->setBalance(1);
      return;
    }
    //case 3 b(n) +diff == 0
    else{
      n->setBalance(0);
      removeFix(p, nextDiff);
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* g)
{
  AVLNode<Key,Value>* p = g->getLeft();
  if (g->getParent() == nullptr){
    // g is root node
    this->root_ = p;
  }
  else{
    // g has a parent
    if (g->getParent()->getLeft()==g){
      // g is a left child
      g->getParent()->setLeft(p);
    }
    else{
      // g is a right child
      g->getParent()->setRight(p);
    }
  }
  p->setParent(g->getParent());
  g->setParent(p);
  g->setLeft(p->getRight());
  if (p->getRight() != nullptr){
    p->getRight()->setParent(g);
  }
  p->setRight(g);
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* p)
{
  AVLNode<Key,Value>* n = p->getRight();
  if (p->getParent() == nullptr){
    // p is root node
    this->root_ = n;
  }
  else{
    // p has a parent
    if (p->getParent()->getLeft()==p){
      // p is a left child
      p->getParent()->setLeft(n);
    }
    else{
      // p is a right child
      p->getParent()->setRight(n);
    }
  }
  n->setParent(p->getParent());
  p->setParent(n);
  p->setRight(n->getLeft());
  if (n->getLeft() != nullptr){
    n->getLeft()->setParent(p);
  }
  n->setLeft(p);
}
  


#endif

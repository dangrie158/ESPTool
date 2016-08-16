#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#ifndef NULL
#include "cstddef"
#endif

template <typename T> class Node {
public:
  T value;
  Node<T> *next;

  Node(T data) : value(data), next(NULL) {}
};

template <typename T> class LinkedList {
private:
  Node<T> *mRoot;

public:
  LinkedList<T>() : mRoot(NULL) {}
  void push_back(T data) {
    if (this->mRoot == NULL) {
      this->mRoot = new Node<T>(data);
      return;
    }

    Node<T> *temp = this->mRoot;

    while (temp->next != NULL) {
      temp = temp->next;
    }

    temp->next = new Node<T>(data);
  }

  T pop() {
    if (this->mRoot == NULL) {
      return NULL;
    }

    Node<T> *temp = this->mRoot;
    Node<T> *prev = NULL;

    while (temp->next != NULL) {
      prev = temp;
      temp = temp->next;
    }

    T data = temp->value;
    delete temp;
    if (prev != NULL) {
      prev->next = NULL;
    } else {
      mRoot = NULL;
    }
    return data;
  }

  int size() {
    int counter = 0;
    Node<T> *temp = mRoot;
    while (temp != 0) {
      temp = temp->next;
      counter++;
    }
    return counter;
  }

  T at(int index) {
    Node<T> *temp = this->mRoot;
    if (temp == NULL) {
      return NULL;
    }
    while (index-- > 0) {
      if (temp->next != NULL) {
        temp = temp->next;
      } else {
        return NULL;
      }
    }
    return temp->value;
  }

  void clear() {
    Node<T> *temp = mRoot;
    while (temp != NULL) {
      Node<T> *next = temp->next;
      delete temp;
      temp = next;
    }
  }

  void remove(T data) {
    Node<T> *temp = this->mRoot;
    if (temp == NULL) {
      return;
    }

    Node<T> *prev = NULL;

    while (temp != NULL) {
      if (temp->value == data) {
        if (prev == NULL) {
          Node<T> *next = temp->next;
          delete mRoot;
          mRoot = next;
        } else {
          prev->next = temp->next;
          delete temp;
        }
        return;
      }
      prev = temp;
      temp = temp->next;
    }
  }
};
#endif // _LINKEDLIST_H_

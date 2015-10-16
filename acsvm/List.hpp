//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Linked list handling.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__List_H__
#define ACSVM__List_H__


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // ListLink
   //
   template<typename T>
   class ListLink
   {
   public:
      ListLink() : obj{nullptr}, prev{this}, next{this} {}
      ListLink(T *obj_) : obj{obj_}, prev{this}, next{this} {}
      ~ListLink() {unlink();}

      //
      // insert
      //
      void insert(ListLink<T> *head)
      {
         (prev = head->prev)->next = this;
         (next = head      )->prev = this;
      }

      void relink(ListLink<T> *head) {unlink(); insert(head);}

      //
      // unlink
      //
      void unlink()
      {
         prev->next = next;
         next->prev = prev;

         prev = next = this;
      }

      T *const obj;
      ListLink<T> *prev, *next;
   };
}

#endif//ACSVM__List_H__


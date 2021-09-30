/*
 * Developed by R. E. Bryant, 2017
 * Extended to store strings, 2018
 */

/*
 * This program implements a queue supporting both FIFO and LIFO
 * operations.
 *
 * It uses a singly-linked list to represent the set of queue elements
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
  Create empty queue.
  Return NULL if could not allocate space.
*/
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    /* What if malloc returned NULL? */
    if (!q) {
      return NULL; // if malloc couldn't allocate space return null
    }
    else { // otherwise
      q->head = NULL; // set head to null
      q->tail = NULL; // set tail to null
      q->size = 0; // and set size to 0
      return q;
    }
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q) { // if null do nothing
      return;
    }

    /* How about freeing the list elements and the strings? */
    list_ele_t* curr = q->head;
    list_ele_t* helper = NULL;

    while (curr != NULL) { // interate through the linked list
      helper = curr;
      curr = curr->next; // grab the next node
      free(helper->value); // free the string
      free(helper); // free the node
    }
    /* Free queue structure */
    free(q);
}

/*
  Attempt to insert element at head of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    /* What should you do if the q is NULL? */
    if (!q) {
      return false; // return false if q is null
    }

    newh = malloc(sizeof(list_ele_t));
    if (!newh) {
      return false; // if malloc failed return false
    }

    /* Don't forget to allocate space for the string and copy it */
    newh->value = malloc(strlen(s) + 1);
    if (newh->value == NULL) {
      free(newh); // free the node
      return false; // if malloc failed return false
    }
    strcpy(newh->value, s); // if it worked copy the string

    newh->next = q->head;
    q->head = newh;

    if (q->size == 0) {
      q->tail = newh; // if the q was empty newh is also the tail
    }

    q->size++; // increment the size
    return true;
}


/*
  Attempt to insert element at tail of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    /* You need to write the complete code for this function */
    list_ele_t* new;

    if (!q) {
      return false; // if q is null return false
    }

    new = malloc(sizeof(list_ele_t));
    if (!new) {
      return false; // if malloc failed return false
    }

    new->value = malloc(strlen(s) + 1);
    if (new->value == NULL) {
      free(new); // free the node
      return false; // if malloc failed return false
    }
    strcpy(new->value, s); // if it worked copy the string

    if (q->size == 0) {
      q->head = q->tail = new; // if q is empty both the head and tail are the new node
    }
    else {
      q->tail->next = new; // otherwise make current tail point to new node
      q->tail = new; // then make the new node the tail
    }

    /* Remember: It should operate in O(1) time */
    new->next = NULL; // make sure tail/new node points to null
    q->size++; // increment size
    return true;
}

/*
  Attempt to remove element from head of queue.
  Return true if successful.
  Return false if queue is NULL or empty.
  If sp is non-NULL and an element is removed, copy the removed string to *sp
  (up to a maximum of bufsize-1 characters, plus a null terminator.)
  The space used by the list element and the string should be freed.
*/
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    /* You need to fix up this code. */
    if (!q || !q->head) {
      return false; // if q is null or empty return false
    }

    list_ele_t* remove = q->head;

    /* If the string of the removed node was too long to fit into sp
       then the null terminator is not copied into the last spot of sp.
       Thus we have to do it ourself to make sure we don't run into problems
    */
    if (sp != NULL) { // if sp is NON-NULL
      strncpy(sp, remove->value, bufsize - 1);
      sp[bufsize - 1] = '\0'; // comment above explains process
    }

    q->head = remove->next; // make the q head point to the next node
    free(remove->value); // free the string
    free(remove); // free the node
    q->size--; // decrement size

    return true;
}

/*
  Return number of elements in queue.
  Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* You need to write the code for this function */
    /* Remember: It should operate in O(1) time */
    if (!q) {
      return 0; // if null return 0
    }
    return q->size;
}

/*
  Reverse elements in queue
  No effect if q is NULL or empty
  This function should not allocate or free any list elements
  (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
  It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    /* You need to write the code for this function */
    if (!q || !q->head) {
      return; // if null or empty do nothing
    }

    list_ele_t* prev = NULL;
    list_ele_t* next = NULL;
    list_ele_t* temp_h = q->head;
    list_ele_t* curr = q->head;

    while (curr != NULL) {
      next = curr->next; // grab next node
      curr->next = prev; // change direction of next pointer
      prev = curr; // move prev to curr
      curr = next; // move curr to next
    }

    q->tail = temp_h; // set tail to the old head of the q
    q->head = prev; // curr holds null so set head to prev, the last node of q
}

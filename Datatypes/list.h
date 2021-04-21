#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

typedef struct list list;
typedef struct node **list_pos;
typedef void(*removeFunc)(void*);

/*
* creates an empty list
*
* returns a empty list
*/
list *list_empty();

/*
* Checks if list is empty
*
* \param [in] l - list to check
*
* returns a bool (1 if empty)
*/
bool list_isEmpty(list *l);

/*
* Checks if pos is at the end of the list
*
* \param [in] l - list to check
* \param [in] pos - position in list
*
* returns a bool (1 if at the end)
*/
bool list_isEnd(list *l, list_pos pos);

/*
* Checks if pos is at the beginning of the list
*
* \param [in] l - list to check
* \param [in] pos - position in list
*
* returns a bool (1 if at the beginning)
*/
bool list_isFirst(list *l, list_pos pos);

/*
* Returns the first position in the list
*
* \param [in] l - list to get pos from
*
* returns the first position in the list
*/
list_pos list_first(list *l);

/*
* Returns the last position in the list
*
* \param [in] l - list to get pos from
*
* returns the last position in the list
*/
list_pos list_end(list *l);

/*
* Returns the next position in the list
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
*
* returns the next position in the list
*/
list_pos list_next(list *l, list_pos pos);

/*
* Returns the prev position in the list
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
*
* returns the prev position in the list
*/
list_pos list_prev(list *l, list_pos pos);

/*
* Returns the pointer to the value
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
*
* returns the pointer to the value
*/
void *list_inspect(list *l, list_pos pos);

/*
* Inserts a element before current position
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
* \param [in] value - a pointer to value that will be added in the list
*
* returns position of the newly added element
*/
list_pos list_insert_before(list *l, list_pos pos, void *value);

/*
* Inserts a element after current position
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
* \param [in] value - a pointer to value that will be added in the list
*
* returns position of the newly added element
*/
list_pos list_insert_after(list *l, list_pos pos, void *value);

/*
* Removes the element at current position
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
*
* returns position of the next element
*/
list_pos list_remove(list *l, list_pos pos, removeFunc);

/*
* Frees all element in the list and their values
*
* \param [in] l - list to get pos from
*
*/
void list_kill(list *l, removeFunc);

#endif
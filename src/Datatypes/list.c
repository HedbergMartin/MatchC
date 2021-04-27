/*
* File:         list.c
* Description:  A double-linked list
* Author:		Simon Bonér
* CS username:  dv17sbr
* Date:			2018-09-14
*/
#include <stdlib.h>
#include <assert.h>
#include "list.h"

typedef struct node {
	void *value;
	struct node *next;
	struct node *prev;
} node;

static node *make_node(void *value) {
	node *n = malloc(sizeof(node));

	if (n == NULL) {
		return NULL;
	}
	n->value = value;
	n->next = NULL;
	n->prev = NULL;
	return n;
}

struct list {
	struct node *first;
	struct node *end;
};


/*
* creates an empty list
*
* returns a empty list
*/
list *list_empty() {
	list *l = calloc(1, sizeof(list));

	if (l == NULL) {
		return NULL;
	}
	return l;
}


/*
* Checks if list is empty
*
* \param [in] l - list to check
*
* returns a bool (1 if empty)
*/
bool list_isEmpty(list *l) {
	assert(l != NULL);

	if (l->first == NULL) {
		return true;
	}

	return false;
}


/*
* Checks if pos is at the end of the list
*
* \param [in] l - list to check
* \param [in] pos - position in list
*
* returns a bool (1 if at the end)
*/
bool list_isEnd(list *l, list_pos pos) {
	assert(l != NULL);
	assert(pos != NULL);

	if (list_isEmpty(l)) {
		return true;
	}
	else if ((*pos)->next == NULL) {
		return true;
	}

	return false;
}


/*
* Checks if pos is at the beginning of the list
*
* \param [in] l - list to check
* \param [in] pos - position in list
*
* returns a bool (1 if at the beginning)
*/
bool list_isFirst(list *l, list_pos pos) {
	assert(l != NULL);
	assert(pos != NULL);

	if (list_isEmpty(l)) {
		return true;
	}
	else if ((*pos)->prev == NULL) {
		return true;
	}

	return false;
}


/*
* Returns the first position in the list
*
* \param [in] l - list to get pos from
*
* returns the first position in the list
*/
list_pos list_first(list *l) {
	assert(l != NULL);
	list_pos pos = &(l->first);

	return pos;
}


/*
* Returns the last position in the list
*
* \param [in] l - list to get pos from
*
* returns the last position in the list
*/
list_pos list_end(list *l) {
	assert(l != NULL);
	list_pos pos = &(l->end);

	return pos;
}


/*
* Returns the next position in the list
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
*
* returns the next position in the list
*/
list_pos list_next(list *l, list_pos pos) {
	assert(l != NULL);
	assert(pos != NULL);

	if (!list_isEnd(l, pos)) {
		pos = &((*pos)->next);
	}

	return pos;
}


/*
* Returns the prev position in the list
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
*
* returns the prev position in the list
*/
list_pos list_prev(list *l, list_pos pos) {
	assert(l != NULL);
	assert(pos != NULL);

	if (!list_isFirst(l, pos)) {
		pos = &((*pos)->prev);
	}

	return pos;
}


/*
* Returns the pointer to the value
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
*
* returns the pointer to the value
*/
void *list_inspect(list *l, list_pos pos) {
	assert(l != NULL);
	assert(pos != NULL);

	return (*pos)->value;
}


/*
* Inserts a element before current position
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
* \param [in] value - a pointer to value that will be added in the list
*
* returns position of the newly added element
*/
list_pos list_insert_before(list *l, list_pos pos, void *value) {
	assert(value != NULL);
	assert(pos != NULL);
	assert(l != NULL);

	node *n = make_node(value);

	if (n == NULL) {
		return NULL;
	}

	if (list_isFirst(l, pos)) {
		node *tempNode = l->first;

		if (!list_isEmpty(l)) {
			tempNode->prev = n;
		} else {
			l->end = n;
		}
		l->first = n;
		n->next = tempNode;
		n->prev = NULL;
		pos = &(l->first);
	} else {
		node *tempNode = (*pos);
		n->prev = tempNode->prev;
		n->next = tempNode;
		n->prev->next = n;
		tempNode->prev = n;
		pos = &(tempNode->prev);
	}


	return pos;
}


/*
* Inserts a element after current position
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
* \param [in] value - a pointer to value that will be added in the list
*
* returns position of the newly added element
*/
list_pos list_insert_after(list *l, list_pos pos, void *value) {
	assert(value != NULL);
	assert(pos != NULL);
	assert(l != NULL);

	node *n = make_node(value);

	if (n == NULL) {
		return NULL;
	}

	if (list_isEnd(l, pos)) {
		node *tempNode = l->end;

		if (!list_isEmpty(l)) {
			tempNode->next = n;
		} else {
			l->first = n;
		}
		l->end = n;
		n->prev = tempNode;
		n->next = NULL;
		pos = &(l->end);
	} else {
		node *tempNode = (*pos);
		n->next = tempNode->next;
		n->prev = tempNode;
		n->next->prev = n;
		tempNode->next = n;
		pos = &(tempNode->next);
	}

	return pos;
}



/*
* Removes the element at current position
*
* \param [in] l - list to get pos from
* \param [in] pos - current pos in list
* \param [in] func - function for removing allocated memory in list elements. Use NULL if unsure
*
* returns position of the next element
*/
list_pos list_remove(list *l, list_pos pos, removeFunc func) {
	assert(pos != NULL);
	assert(l != NULL);

	node *tempNode = (*pos);

	if (list_isEnd(l, pos) && list_isFirst(l, pos)) {
		l->first = NULL;
		l->end = NULL;
		pos = &(l->first);
	} else if (list_isEnd(l, pos)) {
		l->end = tempNode->prev;
		tempNode->prev->next = NULL;
		pos = &(l->end);
	} else if (list_isFirst(l, pos)) {
		l->first = tempNode->next;
		tempNode->next->prev = NULL;
		pos = &(l->first);
	} else {
		tempNode->prev->next = tempNode->next;
		tempNode->next->prev = tempNode->prev;
		pos = &(tempNode->prev->next);
	}

	if (func != NULL) {
		(*func)(tempNode->value);
	}
	free(tempNode->value);
	free(tempNode);

	return pos;
}


/*
* Frees all element in the list and their values
*
* \param [in] l - list to get pos from
* \param [in] func - function for removing allocated memory in list elements. Use NULL if unsure
*
*/
void list_kill(list *l, removeFunc func) {
	assert(l != NULL);
	list_pos pos = list_first(l);

	while (!list_isEmpty(l)) {
		pos = list_remove(l, pos, func);
	}
	free(l);
	return;

}

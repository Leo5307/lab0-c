#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q_head = malloc(sizeof(struct list_head));

    if (q_head) {
        INIT_LIST_HEAD(q_head);
        return q_head;
    }

    else
        return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    element_t *entry;
    element_t *safe;
    if (head) {
        list_for_each_entry_safe (entry, safe, head, list) {
            list_del(&(entry->list));
            free(entry->value);
            free(entry);
        }

        free(head);
    }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)

{
    if (!head) {
        return false;
    }
    element_t *node = malloc(sizeof(element_t));
    if (!node || !s) {
        free(node);
        return false;
    }
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *node = malloc(sizeof(element_t));
    if (!node || !s) {
        free(node);
        return false;
    }

    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }

    else {
        list_add_tail(&node->list, head);
        return true;
    }
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)

{
    if (!head) {
        return NULL;
    }
    element_t *remove_node = list_first_entry(head, element_t, list);
    if (sp && remove_node) {
        strncpy(sp, remove_node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
        list_del(&remove_node->list);
        // printf("%s", sp);
    }

    return remove_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head) {
        return NULL;
    }
    element_t *remove_node = list_last_entry(head, element_t, list);
    if (sp && remove_node) {
        strncpy(sp, remove_node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
        list_del(&remove_node->list);
        // printf("%s", sp);
    }

    return remove_node;
    // return NULL;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *cursor;

    list_for_each (cursor, head)
        len++;
    return len;
    // return -1;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head) {
        return false;
    }
    int size = q_size(head);
    int mid = 0;
    if (size % 2 == 0) {
        mid = size / 2 - 1;
    } else {
        mid = size / 2;
    }
    struct list_head *cursor;
    int len = 0;
    list_for_each (cursor, head) {
        if (len == mid) {
            break;
        } else {
            len++;
        }
    }
    element_t *del_node = container_of(cursor, element_t, list);
    list_del(cursor);
    free(del_node->value);
    free(del_node);
    return true;
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{  // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head) {
        return false;
    }
    element_t *entry;
    element_t *safe;
    char *key = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (key && strcmp(entry->value, key) ==
                       0) {  // condition of key equal to element's value
            // printf("%s",key);
            list_del(&(entry->list));
            free(entry->value);
            free(entry);
            continue;
        }

        if (safe->value && strcmp(entry->value, safe->value) ==
                               0) {  // condition of finding same element,store
                                     // the value to key
            list_del(&(entry->list));
            key = strdup(entry->value);
            free(entry->value);
            free(entry);
        }
    }

    free(key);
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (head && !(list_is_singular(head))) {
        struct list_head *prev_list, *next_list;
        struct list_head *first_list = head->next;
        struct list_head *second_list = first_list->next;

        while (first_list != head && second_list != head) {
            prev_list = first_list->prev;
            next_list = second_list->next;

            // in the box
            first_list->prev = second_list;
            second_list->next = first_list;

            // outside the box
            first_list->next = next_list;
            second_list->prev = prev_list;
            next_list->prev = first_list;
            prev_list->next = second_list;

            // next iteration
            first_list = first_list->next;
            second_list = first_list->next;
        }
    }


    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (list_empty(head))
        return;
    if (!(list_is_singular(head))) {
        struct list_head *curr = head;
        struct list_head *tmp;
        do {
            tmp = curr->next;
            curr->next = curr->prev;
            curr->prev = tmp;
            curr = tmp;
        } while ((curr != head));  // terminate
    }
}


/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (list_empty(head) || k <= 1 || list_is_singular(head)) {
        return;
    }

    struct list_head *cursor;
    struct list_head *safe;
    struct list_head new_queue;
    struct list_head *new_queue_head = &new_queue;
    struct list_head **start_node = &head;
    // struct list_head tmp_queue, *tmp_head = head, *safe, *node;
    INIT_LIST_HEAD(new_queue_head);
    int count = 1;
    list_for_each_safe (cursor, safe, head) {
        if (count == k) {
            list_cut_position(new_queue_head, *start_node, cursor);
            q_reverse(new_queue_head);
            list_splice_init(new_queue_head, head);
            start_node = &(safe->prev);
            count = 0;
        }
        count++;
        // printf("done");
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    struct list_head list_less, list_greater;
    element_t *item = NULL, *is = NULL;

    if (list_empty(head) || list_is_singular(head))
        return;

    INIT_LIST_HEAD(&list_less);
    INIT_LIST_HEAD(&list_greater);

    element_t *pivot = list_last_entry(head, element_t, list);
    list_del(&pivot->list);

    list_for_each_entry_safe (item, is, head, list) {
        if (strcmp(item->value, pivot->value) < 0)
            list_move_tail(&item->list, &list_less);
        else
            list_move_tail(&item->list, &list_greater);
    }

    q_sort(&list_less, false);
    q_sort(&list_greater, false);

    list_add(&pivot->list, head);
    list_splice(&list_less, head);
    list_splice_tail(&list_greater, head);

    if (descend) {
        q_reverse(head);
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}

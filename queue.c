#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

typedef int (*list_cmp_func_t)(void *,
                               const struct list_head *,
                               const struct list_head *);

void timsort(void *priv, struct list_head *head, list_cmp_func_t cmp);
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
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *remove_node = list_first_entry(head, element_t, list);
    list_del(&(remove_node->list));
    if (sp && remove_node) {
        size_t size;
        if (strlen(remove_node->value) < (bufsize - 1)) {
            size = strlen(remove_node->value);
        } else {
            size = bufsize - 1;
        }
        strncpy(sp, remove_node->value, size);
        sp[size] = '\0';
    }

    return remove_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *remove_node = list_last_entry(head, element_t, list);
    list_del(&(remove_node->list));
    if (sp && remove_node) {
        size_t size;
        if (strlen(remove_node->value) < (bufsize - 1)) {
            size = strlen(remove_node->value);
        } else {
            size = bufsize - 1;
        }
        strncpy(sp, remove_node->value, size);
        sp[size] = '\0';
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
// bool q_delete_dup(struct list_head *head)
// {  // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

//     if (!head) {
//         return false;
//     }

//     if (list_is_singular(head)){
//         return true;
//     }
//         element_t *entry;
//     element_t *safe;
//     char *key = NULL;
//     list_for_each_entry_safe (entry, safe, head, list) {
//         if (key && strcmp(entry->value, key) ==
//                        0) {  // condition of key equal to element's value
//             // printf("%s",key);
//             list_del(&(entry->list));
//             free(entry->value);
//             free(entry);
//             continue;
//         }

//         if (safe->value && strcmp(entry->value, safe->value) ==
//                                0) {  // condition of finding same
//                                element,store
//                                      // the value to key
//             list_del(&(entry->list));
//             key = strdup(entry->value);
//             free(entry->value);
//             free(entry);
//         }
//     }

//     free(key);
//     return true;
// }
bool q_delete_dup(struct list_head *head)
{  // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head) {
        return false;
    }

    if (list_is_singular(head)) {
        return true;
    }
    struct list_head *cursor;
    struct list_head *del_cursor;
    struct list_head *cursor_moving;
    struct list_head *del_cursor_moving;
    int count = 0;
    cursor = head->next;

    while (cursor != head) {
        // safe = cursor->next;
        cursor_moving = cursor->next;
        element_t *curr_element = container_of(cursor, element_t, list);

        while (cursor_moving != head) {
            element_t *curr_moving_element =
                container_of(cursor_moving, element_t, list);
            // printf("round");
            if (strcmp(curr_element->value, curr_moving_element->value) == 0) {
                printf("%s\n", curr_moving_element->value);
                del_cursor_moving = cursor_moving;
                cursor_moving = cursor_moving->next;
                cursor_moving->prev = del_cursor_moving->prev;
                cursor_moving->prev->next =
                    del_cursor_moving->next;  // important
                element_t *del_node =
                    container_of(del_cursor_moving, element_t, list);
                free(del_node->value);
                free(del_node);
                count++;
            } else {
                break;
            }
            // printf("check");
        }
        if (count > 0) {
            del_cursor = cursor;
            cursor = cursor->next;
            cursor->prev = del_cursor->prev;
            cursor->prev->next = del_cursor->next;  // important
            element_t *del_node = container_of(del_cursor, element_t, list);
            free(del_node->value);
            free(del_node);
            count = 0;
        } else {
            cursor = cursor->next;
        }
    }
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
    if (!head || list_empty(head) || k <= 1 || list_is_singular(head)) {
        return;
    }

    struct list_head *cursor;
    struct list_head *safe;
    struct list_head new_queue;
    struct list_head tmp_queue;
    struct list_head *new_queue_head = &new_queue;
    struct list_head *tmp_queue_head = &tmp_queue;
    struct list_head **start_node = &head;
    // struct list_head tmp_queue, *tmp_head = head, *safe, *node;
    INIT_LIST_HEAD(new_queue_head);
    INIT_LIST_HEAD(tmp_queue_head);
    int count = 1;
    list_for_each_safe (cursor, safe, head) {
        if (count == k) {
            list_cut_position(tmp_queue_head, *start_node, cursor);
            q_reverse(tmp_queue_head);
            list_splice_tail_init(tmp_queue_head, new_queue_head);
            start_node = &(safe->prev);
            count = 0;
        }
        count++;
    }
    if (!list_empty(head)) {
        list_splice_tail_init(head, new_queue_head);
    }
    list_cut_position(head, new_queue_head, new_queue_head->prev);
}

void merge_two_list(struct list_head *l1,
                    struct list_head *l2,
                    struct list_head *head)
{
    while (!list_empty(l1) && !list_empty(l2)) {
        element_t *l1_node = container_of(l1->next, element_t, list);
        element_t *l2_node = container_of(l2->next, element_t, list);
        if (strcmp(l1_node->value, l2_node->value) <= 0) {
            list_move_tail(l1->next, head);
        } else {
            list_move_tail(l2->next, head);
        }
    }
    if (!list_empty(l1)) {
        list_splice_tail_init(l1, head);
    }
    if (!list_empty(l2)) {
        list_splice_tail_init(l2, head);
    }
}
void mergeSortList(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head, *fast = head;
    do {
        fast = fast->next->next;
        slow = slow->next;
    } while (fast != head && fast->next != head);

    LIST_HEAD(l_head);
    LIST_HEAD(r_head);
    list_splice_tail_init(head, &r_head);
    list_cut_position(&l_head, &r_head, slow);

    mergeSortList(&l_head);
    mergeSortList(&r_head);
    merge_two_list(&l_head, &r_head, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    mergeSortList(head);
    if (descend) {
        q_reverse(head);
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_is_singular(head)) {
        return q_size(head);
    }
    struct list_head *cursor;
    struct list_head *safe;
    struct list_head *cursor_moving;

    cursor = head->next;
    int size = q_size(head);
    // printf("%d",size);
    for (int i = 1; i <= size; i++) {
        safe = cursor->next;
        cursor_moving = cursor->next;
        // printf("%d",strcmp(container_of(cursor,element_t,list)->value,container_of(cursor_moving,element_t,list)->value));
        // list_del(cursor);
        // q_release_element(container_of(cursor,element_t,list));
        // printf("%d",i);
        for (int j = 1; j <= size - i; j++) {
            // printf("%d,%d",i,j);
            element_t *curr_element = container_of(cursor, element_t, list);
            element_t *curr_moving_element =
                container_of(cursor_moving, element_t, list);
            // printf("%s",curr_element->value);
            // printf("%s",curr_moving_element->value);
            char *cur_value = curr_element->value;
            char *cur_mov_value = curr_moving_element->value;

            if (strcmp(cur_value, cur_mov_value) > 0) {
                // printf("\nstart debug");
                // printf("%s",curr_element->value);
                list_del(cursor);
                q_release_element(curr_element);
                break;
            } else {
                printf("\n move to next");
                cursor_moving = cursor_moving->next;
            }
        }
        cursor = safe;
    }
    return (q_size(head));


    // return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_is_singular(head)) {
        return q_size(head);
    }
    struct list_head *cursor;
    struct list_head *safe;
    struct list_head *cursor_moving;

    cursor = head->next;
    int size = q_size(head);
    // printf("%d",size);
    for (int i = 1; i <= size; i++) {
        safe = cursor->next;
        cursor_moving = cursor->next;
        // printf("%d",strcmp(container_of(cursor,element_t,list)->value,container_of(cursor_moving,element_t,list)->value));
        // list_del(cursor);
        // q_release_element(container_of(cursor,element_t,list));
        // printf("%d",i);
        for (int j = 1; j <= size - i; j++) {
            printf("%d,%d", i, j);
            element_t *curr_element = container_of(cursor, element_t, list);
            element_t *curr_moving_element =
                container_of(cursor_moving, element_t, list);
            // printf("%s",curr_element->value);
            // printf("%s",curr_moving_element->value);
            char *cur_value = curr_element->value;
            char *cur_mov_value = curr_moving_element->value;

            if (strcmp(cur_value, cur_mov_value) < 0) {
                // printf("\nstart debug");
                // printf("%s",curr_element->value);
                list_del(cursor);
                q_release_element(curr_element);
                break;
            } else {
                // printf("\n move to next");
                cursor_moving = cursor_moving->next;
            }
        }
        cursor = safe;
    }
    return (q_size(head));
    // return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    queue_contex_t *queue_first =
        container_of(head->next, queue_contex_t, chain);
    struct list_head *cursor, *safe;
    bool first = true;
    list_for_each_safe (cursor, safe, head) {
        if (first) {
            first = false;
            continue;
        } else {
            queue_contex_t *queue_tmp =
                container_of(cursor, queue_contex_t, chain);
            list_splice_tail_init(queue_tmp->q, queue_first->q);
        }
    }
    q_sort(queue_first->q, descend);
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return q_size(queue_first->q);
}

static inline size_t run_size(struct list_head *head)
{
    if (!head)
        return 0;
    if (!head->next)
        return 1;
    return (size_t)(head->next->prev);
}

struct pair {
    struct list_head *head, *next;
};

static size_t stk_size;

static struct list_head *merge(void *priv,
                               list_cmp_func_t cmp,
                               struct list_head *a,
                               struct list_head *b)
{
    struct list_head *head = NULL;
    struct list_head **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b) <= 0) {
            *tail = a;
            tail = &(*tail)->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &(*tail)->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void build_prev_link(struct list_head *head,
                            struct list_head *tail,
                            struct list_head *list)
{
    tail->next = list;
    do {
        list->prev = tail;
        tail = list;
        list = list->next;
    } while (list);

    /* The final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

static void merge_final(void *priv,
                        list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    build_prev_link(head, tail, b);
}

int find_minrun(int n)
{
    int count = 0;
    int min_merge = 32;
    bool mod = false;
    if (n == 0) {
        return 0;
    }
    while (n > min_merge) {
        if (n % 2 != 0) {
            mod = true;
        }
        n /= 2;
        // printf("n = %d",count);
        count++;
    }
    int minrun = 1;
    for (int i = 0; i < count; i++) {
        minrun *= 2;
    }
    if (mod) {
        return (minrun + 1);
    } else {
        return minrun;
    }
}


static struct pair find_run(void *priv,
                            struct list_head *list,
                            list_cmp_func_t cmp,
                            int minrun)
{
    size_t len = 1;
    struct list_head *next = list->next, *head = list;
    struct pair result;

    if (!next) {
        result.head = head, result.next = next;
        return result;
    }

    if (cmp(priv, list, next) > 0) {
        /* decending run, also reverse the list */
        struct list_head *prev = NULL;
        do {
            len++;
            list->next = prev;
            prev = list;
            list = next;
            next = list->next;
            head = list;
        } while (next && (cmp(priv, list, next) > 0 || len < minrun));
        list->next = prev;
    } else {
        do {
            len++;
            list = next;
            next = list->next;
        } while (next && (cmp(priv, list, next) <= 0 || len < minrun));
        list->next = NULL;
    }
    head->prev = NULL;
    head->next->prev = (struct list_head *) len;
    result.head = head, result.next = next;
    return result;
}

static struct list_head *merge_at(void *priv,
                                  list_cmp_func_t cmp,
                                  struct list_head *at)
{
    size_t len = run_size(at) + run_size(at->prev);
    struct list_head *prev = at->prev->prev;
    struct list_head *list = merge(priv, cmp, at->prev, at);
    list->prev = prev;
    list->next->prev = (struct list_head *) len;
    --stk_size;
    return list;
}

static struct list_head *merge_force_collapse(void *priv,
                                              list_cmp_func_t cmp,
                                              struct list_head *tp)
{
    while (stk_size >= 3) {
        if (run_size(tp->prev->prev) < run_size(tp)) {
            tp->prev = merge_at(priv, cmp, tp->prev);
        } else {
            tp = merge_at(priv, cmp, tp);
        }
    }
    return tp;
}

static struct list_head *merge_collapse(void *priv,
                                        list_cmp_func_t cmp,
                                        struct list_head *tp)
{
    int n;
    while ((n = stk_size) >= 2) {
        if ((n >= 3 &&
             run_size(tp->prev->prev) <= run_size(tp->prev) + run_size(tp)) ||
            (n >= 4 && run_size(tp->prev->prev->prev) <=
                           run_size(tp->prev->prev) + run_size(tp->prev))) {
            if (run_size(tp->prev->prev) < run_size(tp)) {
                tp->prev = merge_at(priv, cmp, tp->prev);
            } else {
                tp = merge_at(priv, cmp, tp);
            }
        } else if (run_size(tp->prev) <= run_size(tp)) {
            tp = merge_at(priv, cmp, tp);
        } else {
            break;
        }
    }

    return tp;
}


void timsort(void *priv, struct list_head *head, list_cmp_func_t cmp)
{
    stk_size = 0;

    struct list_head *list = head->next, *tp = NULL;
    if (head == head->prev)
        return;
    int size = q_size(head);
    printf("size = %d", size);
    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;
    int minrun = find_minrun(size);
    printf("%d\n", minrun);
    do {
        /* Find next run */
        struct pair result = find_run(priv, list, cmp, minrun);
        result.head->prev = tp;
        tp = result.head;
        list = result.next;
        stk_size++;
        tp = merge_collapse(priv, cmp, tp);
    } while (list);

    /* End of input; merge together all the runs. */
    tp = merge_force_collapse(priv, cmp, tp);

    /* The final merge; rebuild prev links */
    struct list_head *stk0 = tp, *stk1 = stk0->prev;
    while (stk1 && stk1->prev)
        stk0 = stk0->prev, stk1 = stk1->prev;
    if (stk_size <= 1) {
        build_prev_link(head, head, stk0);
        return;
    }
    merge_final(priv, cmp, head, stk1, stk0);
}
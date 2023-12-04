#include <stddef.h>
#include "ll_cycle.h"

int ll_has_cycle(node *head) {
    /* your code here */
    if (!head) return 0;
    node *slow = head, *fast = head->next;
    while (fast != slow && fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow == fast;
}
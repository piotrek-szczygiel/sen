#include <stdio.h>

#include "ps.h"

int main() {
    auto hm = dict_create<int>(1);
    defer(dict_free(hm));

    dict_put(hm, "siemano", 666);
    dict_put(hm, "kolano", 2137);

    dict_put(hm, "siemano", 123);
    dict_put(hm, "essa", 20);

    printf("%d\n", *dict_get(hm, "siemano"));
    printf("%d\n", *dict_get(hm, "kolano"));
    printf("%d\n", *dict_get(hm, "essa"));

    return 0;
}

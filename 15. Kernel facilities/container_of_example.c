#include <stdio.h>
#include <stddef.h> // for offsetof

#define container_of(ptr, type, member) ({                      \
    const typeof(((type *)0)->member) *__mptr = (ptr);          \
    (type *)((char *)__mptr - offsetof(type, member)); })

struct car {
    int id;
    char *name;
};

int main() {
    struct car c = {1, "Toyota"};
    int *id_ptr = &c.id;

    struct car *pcar = container_of(id_ptr, struct car, id);
    printf("Name = %s\n", pcar->name);
    return 0;
}

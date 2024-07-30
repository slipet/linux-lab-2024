#include "proc_data.h"

// order
// reverse
// partially order

int main(void)
{
    char *root = "./test_cases/test_case_";
    char *out;
    int maxBits = 24;
    for (int i = 0; i < maxBits; ++i) {
        int size = (1 << i);
        uint32_t *data;

        out = getPath(root, size, 1);
        printf("generate sorted test case: %s\n", out);
        data = gen_test_case(size, 1);
        outputData(out, data, size);
        free(data);
        free(out);
        out = NULL;
        data = NULL;


        out = getPath(root, size, 0);
        printf("generate reverse test case: %s\n", out);
        data = gen_test_case(size, 0);
        outputData(out, data, size);
        free(data);
        free(out);
        out = NULL;
        data = NULL;


        out = getPath(root, size, 2);
        printf("generate random test case: %s\n", out);
        data = gen_test_case(size, 2);
        outputData(out, data, size);
        free(data);
        free(out);
        out = NULL;
        data = NULL;
    }

    return 0;
}
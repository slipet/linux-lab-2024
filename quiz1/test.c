#include "test.h"

void init_time(double *timep)
{
    (void) delta_time(timep);
}

double delta_time(double *timep)
{
    struct timespec tv;
    // gettimeofday(&tv, NULL);
    clock_gettime(CLOCK_MONOTONIC, &tv);
    double current_time = 1.0E3 * tv.tv_sec + 1.0E-6 * tv.tv_nsec;
    double delta = current_time - *timep;
    *timep = current_time;
    return delta;
}

double test_list(expConfig *config, testCases *tests)
{
    double time_start, time_end, delta;
    node_t *list = NULL;
    size_t count = tests->size;
    // initilize list
    while (count--) {
        list = config->list_construct(list, &tests->data[count]);
    }

    init_time(&time_end);
    time_start = time_end;
    // sort list
    config->algorithm(&list);

    delta = delta_time(&time_end);
    config->list_free(&list);
    return delta;
}
double test_linux_list(expConfig *config, testCases *tests)
{
    double time_start, time_end, delta;
    LIST_HEAD(linux_list);
    size_t count = tests->size;
    // initilize list
    while (count--) {
        if (!config->list_construct(&linux_list, &tests->data[count])) {
            fprintf(stderr, "Failed to add element to list\n");
            return;
        }
    }

    init_time(&time_end);
    time_start = time_end;
    // sort list
    config->algorithm(&linux_list);
    delta = delta_time(&time_end);
    double elapsed = time_end - time_start;
    config->list_free(&linux_list);
    return delta;
}

testCases *init_testcases(uint32_t *data, size_t size)
{
    testCases *testcases = malloc(sizeof(testCases));

    if (data == NULL) {
        // default generate random 100 numbers
        testcases->size = 1000;
        testcases->data = malloc(sizeof(uint32_t) * testcases->size);
        for (int i = 0; i < testcases->size; ++i)
            testcases->data[i] = i;
        return testcases;
    }

    testcases->size = size;
    testcases->data = data;
    return testcases;
}
void free_testcases(testCases *testcases)
{
    free(testcases->data);
    free(testcases);
}

uint32_t *read_testcases(char *path, uint32_t *size)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", path);
        return 0;
    }
    fscanf(file, "%u ", size);

    if (*size <= 0) {
        printf("Error reading test case size1:%u\n", *size);
        return 0;
    }

    uint32_t *data = malloc(sizeof(uint32_t) * (*size));
    if (data == NULL) {
        printf("Error allocating memory for testcases\n");
        fclose(file);
        return 0;
    }

    for (int i = 0; i < *size; i++) {
        if (fscanf(file, "%u ", &data[i]) != 1) {
            printf("Error reading test case data:%u\n", i);
            free(data);
            fclose(file);
            return 0;
        }
    }
    fclose(file);

    return data;
}

char *getFileName(Path_t *path, size_t size)
{
    sprintf(path->data_size, "%u\0", size);
    if (size == 0)
        path->data_size[0] = '\0';

    size_t root_len = strlen(path->root);
    size_t prefix_len = strlen(path->prefix);
    size_t suffix_len = strlen(path->suffix);
    size_t tag_len = strlen(path->tag);
    size_t data_size_len = strlen(path->data_size);

    char *filename = malloc(root_len + prefix_len + tag_len + data_size_len +
                            suffix_len + 1);
    if (filename == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    strcpy(filename, path->root);
    strcat(filename, path->prefix);
    strcat(filename, path->tag);
    strcat(filename, path->data_size);
    strcat(filename, path->suffix);
    return filename;
}

void fixed_quantity_test(expConfig *config,
                       Path_t *path,
                       const size_t maxBits,
                       const size_t algo_num)
{
    char *tagSet[] = {"sorted_", "reverse_", "random_"};

    for (size_t i = 0; i < 3; i++) {
        path->tag = tagSet[i];
        for (size_t i = 0; i < maxBits; i++) {
            double retTime[algo_num];
            char result[algo_num][512];
            size_t size = 1 << i;

            path->root = &"./test_cases/";
            path->prefix = &"test_case_";
            path->suffix = &".data";
            char *dataname = getFileName(path, size);

            // init test case and execute experiments
            printf("test case: %s\n", dataname);
            uint32_t *data = read_testcases(dataname, &size);
            testCases *Data = init_testcases(data, size);
            retTime[0] = test_list(&config[0], Data);
            retTime[1] = test_linux_list(&config[1], Data);

            // Output the result
            path->suffix = &".txt";
            path->root = &"./result/";
            for (size_t k = 0; k < algo_num; k++) {
                path->prefix = config[k].algoName;
                char *resultname = getFileName(path, 0);
                FILE *fp = fopen(resultname, "a");
                sprintf(result[k], "%u %.6f\n", size, retTime[k]);

                fprintf(fp, "%s", result[k]);
                fclose(fp);
                free(resultname);
            }

            free_testcases(Data);
            free(dataname);

            printf(
                "=============================================================="
                "==\n");
        }
    }
}

void random_quantity_test(expConfig *config,
                        Path_t *path,
                        const size_t times,
                        const size_t algo_num,
                        int ordered)
{
    for (size_t i = 0; i < times; i++) {
        double retTime[algo_num];
        char result[algo_num][512];
        size_t data_size = get_random(0, 1, (1 << 17));
        uint32_t *data = gen_test_case(data_size, ordered);

        // Output the result
        path->suffix = &".txt";
        path->root = &"./result/";
        path->prefix = config->algoName;
        switch (ordered) {
        case 0:
            path->tag = &"reverse";
            //data = proc_data_reverse(data, data_size);
            break;
        case 1:
            path->tag = &"sorted";
            //data = proc_sorted_data(data, data_size, 1, 1);
            break;
        case 2:
            path->tag = &"random";
            //data = proc_sorted_data(data, data_size, 1, 0);
            break;
        default:
            break;
        }

        testCases *Data = init_testcases(data, data_size);
        retTime[0] = test_list(&config[0], Data);
        retTime[1] = test_linux_list(&config[1], Data);
        for (size_t i = 0; i < algo_num; i++) {
            path->prefix = config[i].algoName;
            char *resultname = getFileName(path, 0);
            FILE *fp = fopen(resultname, "a");
            sprintf(result[i], "%u %.6f\n", data_size, retTime[i]);

            fprintf(fp, "%s", result[i]);
            fclose(fp);
            free(resultname);
        }

        free_testcases(Data);


        printf(
            "=============================================================="
            "==\n");
    }
}
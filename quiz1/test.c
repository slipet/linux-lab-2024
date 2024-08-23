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
        for (size_t j = 0; j < maxBits; j++) {
            double retTime[algo_num];
            char result[algo_num][512];
            size_t size = 1 << j;

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
                          const size_t maxBits,
                          const size_t times,
                          const size_t algo_num,
                          int ordered)
{
    for (size_t j = 0; j < times; j++) {
        double retTime[algo_num];
        char result[algo_num][512];
        size_t data_size = get_random(0, 1, (1 << maxBits));
        uint32_t *data = gen_test_case(data_size, ordered);

        // Output the result
        path->suffix = &".txt";
        path->root = &"./result/";
        path->prefix = config->algoName;
        switch (ordered) {
        case 0:
            path->tag = &"reverse";
            // data = proc_data_reverse(data, data_size);
            break;
        case 1:
            path->tag = &"sorted";
            // data = proc_sorted_data(data, data_size, 1, 1);
            break;
        case 2:
            path->tag = &"random";
            // data = proc_sorted_data(data, data_size, 1, 0);
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
TEST_RESULT_T *INIT_TEST_RESULT_T(size_t methods_size,
                                  size_t groups_size,
                                  size_t sample_size)
{
    TEST_RESULT_T *tmp = malloc(sizeof(TEST_RESULT_T));
    tmp->methods = malloc(sizeof(TEST_METHOD_T) * methods_size);
    tmp->size = methods_size;
    for (size_t i = 0; i < tmp->size; i++) {
        tmp->methods[i].size = groups_size;
        tmp->methods[i].groups = malloc(sizeof(result_t) * groups_size);
    }
    return tmp;
}
void FREE_TEST_RESULT_T(TEST_RESULT_T *obj)
{
    int cnt = 0;
    for (size_t i = 0; i < obj->size; i++) {
        for (size_t j = 0; j < obj->methods[i].size; j++) {
            free(obj->methods[i].groups[j].execTime);
            free(obj->methods[i].groups[j].weight);
        }
        free(obj->methods[i].groups);
    }

    free(obj->methods);
    free(obj);
    obj = NULL;
}
void repeated_fixed_quantity_test(expConfig *config,
                                  Path_t *path,
                                  const size_t maxBits,
                                  const size_t repeat_times,
                                  const size_t algo_num,
                                  int ordered)
{
    TEST_RESULT_T *result = INIT_TEST_RESULT_T(algo_num, maxBits, repeat_times);

    for (size_t i = 0; i < maxBits; i++) {
        size_t data_size = 1 << i;

        double retTime[algo_num][repeat_times];

        // Output the result
        path->suffix = &"repeated.txt";
        path->root = &"./result/percenttiles_";
        for (size_t algo = 0; algo < algo_num; algo++) {
            result->methods[algo].groups[i].execTime =
                malloc(sizeof(double) * repeat_times);
            result->methods[algo].groups[i].size = repeat_times;
        }
        printf("Data size: %lu\n", data_size);
        for (size_t t = 0; t < repeat_times; ++t) {
            printf("        epoch: %lu\n", t);
            struct timeval time_start, time_end;
            double delta;
            init_time(&time_end);
            time_start = time_end;

            uint32_t *data = gen_test_case(data_size, ordered);
            delta = delta_time(&time_end);
            printf("        data genration time: %lf\n", delta);
            time_start = time_end;
            testCases *Data = init_testcases(data, data_size);
            delta = delta_time(&time_end);
            printf("        data initialization time: %lf\n", delta);
            time_start = time_end;
            // exec sorting

            result->methods[LIST_SORT].groups[i].execTime[t] =
                test_list(&config[0], Data);
            delta = delta_time(&time_end);
            printf("        list time: %f\n", delta);
            time_start = time_end;
            result->methods[LINUX_LIST_SORT].groups[i].execTime[t] =
                test_linux_list(&config[1], Data);
            delta = delta_time(&time_end);
            printf("        linux list time: %f\n", delta);
            free_testcases(Data);
        }
        struct timeval time_start, time_end;
        double delta;
        init_time(&time_end);
        time_start = time_end;
        for (size_t j = 0; j < algo_num; ++j) {
            double *percentiles =
                malloc(sizeof(double) * DUDECT_NUMBER_PERCENTILES);
            prepare_percentiles(result->methods[j].groups[i].execTime,
                                percentiles, repeat_times);
            update_data(&(result->methods[j].groups[i]), percentiles,
                        repeat_times);
            double avg = calculate_mean(result->methods[j].groups[i].execTime,
                                        result->methods[j].groups[i].weight,
                                        result->methods[j].groups[i].size);

            char str[algo_num][512];
            path->prefix = config[j].algoName;
            char *resultname = getFileName(path, 0);
            FILE *fp = fopen(resultname, "a");
            sprintf(str[j], "%u %.6f\n", data_size, avg);
            fprintf(fp, "%s", str[j]);
            fclose(fp);
            free(resultname);
        }
        delta = delta_time(&time_end);
        printf("output: %f\n\n", delta);
    }
    calculate_Fvalue(result);
    FREE_TEST_RESULT_T(result);
}


double calculate_mean(double *time, double *weight, size_t size)
{
    double sum = 0.0f;
    double sz = 0.0f;
    for (size_t i = 0; i < size; ++i) {
        sum += time[i] * weight[i];
        sz += weight[i];
    }
    return sum / sz;
}
double calculate_totalmean(TEST_RESULT_T *test)
{
    double sum = 0.0f;
    double sz = 0.0f;
    for (size_t method = 0; method < test->size; ++method) {
        for (size_t group = 0; group < test->methods[method].size; ++group) {
            for (size_t i = 0; i < test->methods[method].groups[group].size;
                 ++i) {
                sum += test->methods[method].groups[group].execTime[i] *
                       test->methods[method].groups[group].weight[i];
                sz += test->methods[method].groups[group].weight[i];
            }
        }
    }
    return sum / sz;
}
size_t get_groups_totalsz(TEST_METHOD_T *method)
{
    size_t sz = 0;
    for (size_t group = 0; group < method->size; ++group) {
        sz += method->groups[group].processed_size;
    }
    return sz;
}
double calculate_groups_mean(TEST_METHOD_T *method)
{
    double sum = 0.0f;
    double sz = 0.0f;

    for (size_t group = 0; group < method->size; ++group) {
        for (size_t i = 0; i < method->groups[group].size; ++i) {
            sum += method->groups[group].execTime[i] *
                   method->groups[group].weight[i];
            sz += method->groups[group].weight[i];
        }
    }

    return sum / sz;
}
double calculate_groups_stddev(TEST_METHOD_T *method, double mean)
{
    double sum_of_squares = 0.0;
    double total_size = 0.0f;
    for (size_t group = 0; group < method->size; ++group) {
        for (size_t i = 0; i < method->groups[group].size; ++i) {
            sum_of_squares +=
                pow(method->groups[group].execTime[i] - mean, 2.0) *
                method->groups[group].weight[i];
            total_size += 1.0f * method->groups[group].weight[i];
        }
    }
    return sqrt(sum_of_squares / (total_size - 1.0f));
}
long double calculate_sample_size(TEST_METHOD_T *method)
{
    long double sz = 0;
    for (size_t group = 0; group < method->size; ++group) {
        for (size_t i = 0; i < method->groups[group].size; ++i) {
            sz += method->groups[group].weight[i];
        }
    }
    return sz;
}


double calculate_Fvalue(TEST_RESULT_T *test)
{
    double totalmean = calculate_totalmean(test);
    double totalsz = 0.0f;
    double MSTR = 0.0f, MSE = 0.0f;
    double f_value = 0.0;
    for (size_t method = 0; method < test->size; ++method) {
        totalsz += 1.0f * get_groups_totalsz(&(test->methods[method]));
    }

    double df1 = (1.0f * test->size) - 1.0f;
    double df2 = (1.0f * totalsz) - (1.0f * test->size);
    for (size_t method = 0; method < test->size; ++method) {
        long double size = calculate_sample_size(&(test->methods[method]));
        double mean = calculate_groups_mean(&(test->methods[method]));
        MSTR += (mean - totalmean) * (mean - totalmean) * (1.0f * size);
        double stddev = calculate_groups_stddev(&(test->methods[method]), mean);
        MSE += ((size - 1.0f) * stddev * stddev);
        // printf("size:%lf mean = %f stddev:%f\n",size, mean, stddev);
    }
    MSTR /= (1.0f * (df1));
    MSE /= 1.0f * (df2);
    f_value = MSTR / MSE;
    double p_value = 1 - gsl_cdf_fdist_P(f_value, df1, df2);
    printf("df1 = %f\n", df1);
    printf("df2 = %f\n", df2);
    printf("MSTR = %f\n", MSTR);
    printf("MSE = %f\n", MSE);

    printf("p_value = %f\n", p_value);
    printf("f_value = %f\n", f_value);
}



void validate_cal_Fvalue()
{
    // df1: 6
    // df2: 2
    // MSB: 19.163878
    // MSE: 0.045100
    // F-value: 424.919685
    // P-value: 0.000000
    TEST_RESULT_T *test = malloc(sizeof(TEST_RESULT_T));
    test->size = 3;
    test->methods = malloc(sizeof(TEST_METHOD_T) * test->size);
    test->methods->size = 3;

    test->methods[0].size = 1;
    test->methods[0].groups = malloc(sizeof(result_t) * 1);
    test->methods[0].groups[0].processed_size = 3;
    test->methods[0].groups[0].size = 3;
    test->methods[0].groups[0].execTime = malloc(sizeof(double) * 3);
    test->methods[0].groups[0].weight = malloc(sizeof(double) * 3);
    test->methods[0].groups[0].execTime[0] = 10.1;
    test->methods[0].groups[0].execTime[1] = 10.23;
    test->methods[0].groups[0].execTime[2] = 9.8;
    test->methods[0].groups[0].weight[0] = 1.0;
    test->methods[0].groups[0].weight[1] = 1.0;
    test->methods[0].groups[0].weight[2] = 1.0;

    test->methods[1].size = 1;
    test->methods[1].groups = malloc(sizeof(result_t) * 1);
    test->methods[1].groups[0].processed_size = 3;
    test->methods[1].groups[0].size = 3;
    test->methods[1].groups[0].execTime = malloc(sizeof(double) * 3);
    test->methods[1].groups[0].weight = malloc(sizeof(double) * 3);
    test->methods[1].groups[0].execTime[0] = 12.1;
    test->methods[1].groups[0].execTime[1] = 12.3;
    test->methods[1].groups[0].execTime[2] = 11.8;
    test->methods[1].groups[0].weight[0] = 1.0;
    test->methods[1].groups[0].weight[1] = 1.0;
    test->methods[1].groups[0].weight[2] = 1.0;

    test->methods[2].size = 1;
    test->methods[2].groups = malloc(sizeof(result_t) * 1);
    test->methods[2].groups[0].processed_size = 3;
    test->methods[2].groups[0].size = 3;
    test->methods[2].groups[0].execTime = malloc(sizeof(double) * 3);
    test->methods[2].groups[0].weight = malloc(sizeof(double) * 3);
    test->methods[2].groups[0].execTime[0] = 15.1;
    test->methods[2].groups[0].execTime[1] = 15.2;
    test->methods[2].groups[0].execTime[2] = 14.9;
    test->methods[2].groups[0].weight[0] = 1.0;
    test->methods[2].groups[0].weight[1] = 1.0;
    test->methods[2].groups[0].weight[2] = 1.0;

    calculate_Fvalue(test);
}
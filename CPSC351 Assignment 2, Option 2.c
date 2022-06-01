/*Remember to create a ReadMe.md that identifies the source file, output file, and instructions file.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE(x) (sizeof(x)/sizeof(*(x))) // (array size /array element size) = number of elements in the array for a type of array
                                        // replace x with list and remove (x) from SIZE(x) for the same effect
                                        // i.e. #define SIZE (sizeof(list)/sizeof(*list))
int list[] = {7, 12, 19, 3, 18, 4, 2, -5, 6, 15, 8};
int result[(SIZE(list))] = { 0 };            // remove (list) if SIZE is being used instead of SIZE(x)
FILE* fout;

typedef struct
{
    int* subArray;
    unsigned int size;
} SortingThreadParameters;

typedef struct
{
    SortingThreadParameters left;
    SortingThreadParameters right;
} MergingThreadParameters;

void SortingThread(SortingThreadParameters* thread) { // sample array {7, 12, 19, 3, 18}. size 5
    for (int i = 0; i < (thread->size) - 1; ++i) { // i < (5-1) == i < 4
        if (*((thread->subArray) + i) > *((thread->subArray) + i + 1)) { /* 0 and 1. 1 and 2. 2 and 3. 3 and 4. First event at 2 and 3
                                                                         changes array to {3, 7, 12, 19, 18} after resolution */
            int j = i; // j = 2
            while (*((thread->subArray) + j) >= *((thread->subArray) + i + 1) || j > -1) { // j = 2. At index 2 and 3. 19 > 3
                if (*((thread->subArray) + j) < *((thread->subArray) + i + 1)) // manually breaks if value at index j is less than value at index j+1
                    break;
                int temp = *((thread->subArray) + i); // temp = 19
				*((thread->subArray) + i) = *((thread->subArray) + j + 1); // 7, 12, 3, 3, 18
                    *((thread->subArray) + (i + 1)) = temp; // 7, 12, 3, 19, 18
                --j; // j = 1. next iteration: at index 1 and 2.
            }
        }
    }
}

void MergingThread(MergingThreadParameters* thread) {
    int i = 0;
    int o = 0;

    for (int m = 0; m < (thread->left.size) + (thread->right.size); ++m) // {2, 5} {1, 3, 4}. m < 2+3
    {
        if (i >(thread->left.size) - 1) { // i > 2
            result[m] = *((thread->right.subArray) + o);
            ++o;
        }
        else if (o > (thread->right.size) - 1) { // o > 3
            result[m] = *((thread->left.subArray) + i);
            ++i;
        }
        else {
            if (*((thread->left.subArray) + i) < *((thread->right.subArray) + o)) { // 2 < 1 -> False. 2 < 3 -> True. 5 < 3 - > False 5 < 4 -> False
                result[m] = *((thread->left.subArray) + i); // result[1] = 2.
                ++i; // i = 1. valid indexes are 0, 1.
            }
            else if (*((thread->left.subArray) + i) > *((thread->right.subArray) + o)) { // 2 > 1 -> True. 5 > 3 -> True. 5 > 4 -> True
                result[m] = *((thread->right.subArray) + o); // result[0] = 1. result[2] = 3. result[3] = 4
                ++o; // o = 1. o = 2. o = 3. valid indexes are 0, 1, 2.
            }
            else if (*((thread->left.subArray) + i) == *((thread->right.subArray) + o)) { // value inside indexes of the two arrays are equal
                result[m] = *((thread->left.subArray) + i);
                result[m] = *((thread->right.subArray) + o);
                ++i;
                ++o;
            }
        }
    }
}

void pthread_create(SortingThreadParameters* leftarray,
                    SortingThreadParameters* rightarray,
                    MergingThreadParameters* mergearray, FILE* fileout) {
    SortingThread(leftarray);
    SortingThread(rightarray);
    MergingThread(mergearray);
    fprintf(fileout, "{");
    for (int i = 0; i < SIZE(list); ++i) {
        if (i == SIZE(list) - 1)
            fprintf(fileout, "\%d ", list[i]);
        else
            fprintf(fileout, " \%d, ", list[i]);
    }
    fprintf(fileout, "}\n");
}

int main(int argc, const char* argv[]) {
    fout = fopen(argv[2], "w");

    SortingThreadParameters* paramsLeft = malloc(sizeof(SortingThreadParameters));
    paramsLeft->subArray = list;
    paramsLeft->size = (SIZE(list)); // 10 / 2 = 5

    SortingThreadParameters* paramsRight = malloc(sizeof(SortingThreadParameters));
    paramsRight->subArray = list + paramsLeft->size; // list + 5
    paramsRight->size = (SIZE(list)) - paramsLeft->size;  // 10 - 5 = 5

    MergingThreadParameters * paramsMerge = malloc(sizeof(MergingThreadParameters));
    paramsMerge->left = *paramsLeft;
    paramsMerge->right = *paramsRight;

    pthread_create(paramsLeft, paramsRight, paramsMerge, fout);

    fclose(fout);
    return 0;
}


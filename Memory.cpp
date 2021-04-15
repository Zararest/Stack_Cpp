#include <stdio.h>
#include <stdlib.h>
#include "headers/Memory.h"

Memory::Memory(){

    array_of_canary = NULL;
    canary_count = 0;
}

Memory::~Memory(){

    canary_count = 0;
}

void* Memory::calloc_class(int num_of_elem, int size_of_elem){

    if (num_of_elem <= 0)
    {
        return NULL;
    }

    if (size_of_elem <= 0)
    {
        return NULL;
    }

    char* tmp = (char*)calloc(num_of_elem * size_of_elem + 2 * sizeof(int), 1);
    ((int*) tmp)[0] = (int) tmp;
    *( (int*) (tmp + sizeof(int) + num_of_elem * size_of_elem ) )= (int) ( tmp + sizeof(int) + num_of_elem * size_of_elem ) ;
    void* tmp_void = (void*) ( tmp + sizeof(int) );

    array_of_canary = (int**) realloc(array_of_canary, (canary_count + 2) * sizeof(int*));
    array_of_canary[canary_count] = (int*) tmp;
    array_of_canary[canary_count + 1] = (int*) ( tmp + num_of_elem * size_of_elem + sizeof(int) );

    canary_count++;
    canary_count++;

    return tmp_void;

}

void* Memory::realloc_class(void* prev_point, int num_of_byte){

    if (prev_point == NULL)
    {            
        return calloc_class(num_of_byte, 1);
    }
    int num_of_prev_canary = 0;
    char* prev_pre_canary = 0;
    char* prev_post_canary = 0;
    char* new_line = NULL;
    void* tmp_void = NULL;


    while ( ((((int*) prev_point) - 1)  != array_of_canary[num_of_prev_canary]) && (num_of_prev_canary < (canary_count - 1) ) )
    {
        num_of_prev_canary++;
    }

    if (num_of_prev_canary == canary_count - 1)
    {
        return NULL;
    }

    new_line = (char*) realloc( ((int*) prev_point) - 1, num_of_byte + 2 * sizeof(int));

    if (new_line == NULL)
    {
        return NULL;
    }

    ((int*) new_line)[0] = (int) new_line;
    *( (int*) ( new_line + sizeof(int) + num_of_byte ) )= (int) ( new_line + sizeof(int) + num_of_byte ) ;


    array_of_canary[num_of_prev_canary] = (int*) new_line;
    array_of_canary[num_of_prev_canary + 1] = (int*) ( new_line + sizeof(int) + num_of_byte );
    tmp_void = (void*) (new_line + sizeof(int));

    return tmp_void;
}

void Memory::free_class(void* point){

    int num_of_prev_canary = 0;

    if (point == NULL)
    {
        return ;
    }
    
    if  (  *( ((int*) point) - 1) == (int) ( ((int*) point) - 1) )
    {
        while ( ( (((int*) point) - 1)  != array_of_canary[num_of_prev_canary] ) && ( num_of_prev_canary < (canary_count - 1) ) )
        {
            num_of_prev_canary++;
        }

        if (num_of_prev_canary == canary_count - 1)
        {
            exit(101);
        }

        array_of_canary[num_of_prev_canary] = NULL;
        array_of_canary[num_of_prev_canary + 1] = NULL;

        free( ((int*) point) - 1 );
    } else
    {
        exit(010);
    }

}


int Memory::light_check(){

    int i = 0;
    int mas_of_err[canary_count] = {0};
    int num_of_err = 0;

    for (i = 0; i < canary_count; i++)
    {
        if (array_of_canary[i] != NULL)
        {
            if (*array_of_canary[i] != (int) array_of_canary[i])
            {
                num_of_err++;
                mas_of_err[i] = 1;
            }
        }
    }

    if (num_of_err != 0)
    {
        FILE* file_of_err = fopen("errors.txt", "w");
        fprintf(file_of_err, "Pre_canary - canary, which stands before memory\n");
        fprintf(file_of_err, "Post_canary - canary, which stands after memory\n");

        fprintf(file_of_err, "The total number of deaths of Canaries is:|%i|\n", num_of_err);
        for (i = 0; i <= canary_count; i++)
        {

            if ((mas_of_err[i] = 1) && ( (i % 2) == 0) )
            {
                fprintf(file_of_err, "The pre_canary number[%i] didn't survive your shit-code\n\n", i/2 + 1);
            }

            if ((mas_of_err[i] = 1) && ( (i % 2) == 1) )
            {
                fprintf(file_of_err, "The post_canary number[%i] died in agoni\n\n", i/2 + 1);
            }
        }

        fclose(file_of_err);
        return 1;
    } else
    {
        return 0;
    }
}

int Memory::memory_check (void* point){

    int num_of_prev_canary = 0;

    if (point == NULL)
    {
        return 1;
    }

    if  (  *(((int*) point) - 1) != (int) (((int*) point) - 1) )
    {
        while ( ( (((int*) point) - 1)  != array_of_canary[num_of_prev_canary] ) && ( num_of_prev_canary < (canary_count - 1) ) )
        {
            num_of_prev_canary++;
        }

        if (num_of_prev_canary == canary_count - 1)
        {
            return 2;
        } else
        {
            return 3;
        }
    } else
    {
        return 0;
    }
}

void Memory::memory_dump(){

    FILE* dump_file = fopen("Dynamic memory.txt", "w");
    int i = 0, j = 0, k = 0;

    for (i = 0; i < (canary_count / 2); i++)
    {

        if ( array_of_canary[i * 2] != NULL)
        {
            fprintf(dump_file, "%i) point |%x|:\n", k + 1, (int) (array_of_canary[i * 2] + 1));

            for (j = 0 ; j < ( (int) array_of_canary[i * 2 + 1] - (int) (array_of_canary[i * 2] + 1) ); j++)
            {
                fprintf(dump_file, "[%i] ", *(( (unsigned char*) (array_of_canary[i * 2] + 1) ) + j) );
            }

            fprintf(dump_file, "end\n");
            k++;
        }
    }
    fclose(dump_file);
}



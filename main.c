#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const int STR_TABLE_SIZE = 4096;


unsigned short find_str_in_table(char * s, char * table[]) {
//return an unsigned short
//two bytes, in theory represents 65536 possible indices
    for (int i = 0; i < STR_TABLE_SIZE; i++ ) {
        if (strcmp(s, table[i]) == 0) {
            return i;
        }
    }
    return -1;
}


int lz78_encode(char * input_file_path) {
    char * string_table[STR_TABLE_SIZE];
    for (int i = 0; i < 256; i++ ) {
        string_table[i] = malloc(sizeof(char) * 2);
        if (string_table[i] == NULL) {
            return -1;
        }
        string_table[i][0] = (char) i;
        string_table[i][1] = '\0';
    }
    int string_table_idx = 256;
    FILE *input = fopen(input_file_path, "r");
    if (input == NULL) {
        perror("Error opening file");
        return 1;
    }
    FILE *output = fopen("./output.txt", "w");
    // Check if the file was successfully opened
    if (output == NULL) {
        perror("Error creating file");
        return 1; // Exit with an error code
    }

    // Read the file character by character
    int file_char; // Note: fgetc() returns an int, not char

    size_t curr_word_buff_size = 1;
    char * curr_word = malloc(sizeof(char)*curr_word_buff_size);

    curr_word[0] = '\0';
    size_t curr_word_size = 1;
    unsigned short prev_idx = -1;
    while ((file_char = fgetc(input)) != EOF) {
        char tmp[2];
        tmp[0] = file_char;
        tmp[1] = '\0';
        curr_word_size++;
        if (curr_word_size > curr_word_buff_size) {
            curr_word = realloc(curr_word, curr_word_size);
            curr_word_buff_size = curr_word_size;
        }
        size_t cat_length = strlcat(curr_word, tmp, curr_word_buff_size);
        int str_table_search_idx = find_str_in_table(curr_word, string_table);
        if (str_table_search_idx != -1) {
            printf("Found match in table for string: %s\n", curr_word);
            //set the prev index into the table for use later if needed
            prev_idx = str_table_search_idx;
            //nothing else to do
        } else {
            printf("String not found in table: %s\n", curr_word);
            //add curr word (excluding new char) to the table
            curr_word[curr_word_size-2] = '\0'; // reset the character we just added
            char * copied_word = malloc(sizeof(char)* curr_word_buff_size);
            size_t copy_len = strlcpy(copied_word, curr_word, curr_word_buff_size);
            string_table[string_table_idx++] = copied_word;
            //emit the code of the last word used into the output file stream
            fwrite(&prev_idx, sizeof(prev_idx), 1, output);
            //set current_word to the latest character found
            curr_word[0] = tmp[0];
            curr_word[1] = '\0';
            //reset buffer size
            curr_word_size = 2;
        }
    }

    // Close the file
    fclose(input);
    return 0;
    fprintf(output, "Hello, World!\n");
    fprintf(output, "This is a test file.\n");
    fclose(output);



    //TODO: free memory
    return 0;
}



int main() {
    lz78_encode("oh snap");

}

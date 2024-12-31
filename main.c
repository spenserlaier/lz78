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

int initialize_table(char * table[]) {
    for (int i = 0; i < 256; i++ ) {
        table[i] = malloc(sizeof(char) * 2);
        if (table[i] == NULL) {
            return -1;
        }
        table[i][0] = (char) i;
        table[i][1] = '\0';
    }
    return 0;
}


int lz78_encode(char * input_file_path, char * output_file_path) {
    char * string_table[STR_TABLE_SIZE];
    int initialize_result = initialize_table(string_table);
    if (initialize_result == -1) {
        printf("Error initializing string table\n");
        return -1;
    }
    int string_table_idx = 256;
    FILE *input = fopen(input_file_path, "r");
    if (input == NULL) {
        perror("Error opening file");
        return 1;
    }
    FILE *output = fopen(output_file_path, "w");
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
        printf("current character: %c\n", file_char);
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

    fclose(input);
    fprintf(output, "Hello, World!\n");
    fprintf(output, "This is a test file.\n");
    fclose(output);
    //TODO: free memory
    return 0;
}

int lz78_decode(char * input, char* output) {
    FILE *encoded_file = fopen(input, "r");
    if (encoded_file == NULL) {
        perror("Error opening file");
        return 1;
    }
    FILE *decoded_file = fopen(output, "w");
    char * string_table[STR_TABLE_SIZE];
    int string_table_idx = 256;
    initialize_table(string_table);


    uint16_t code;  // To store the two-byte value (assuming little-endian or matching byte order)
    size_t bytesRead;

    size_t curr_word_buff_size = 1;
    char * curr_word = malloc(sizeof(char)*curr_word_buff_size);
    curr_word[0] = '\0';
    size_t curr_word_size = 1;
    while ((bytesRead = fread(&code, sizeof(code), 1, encoded_file)) == 1) {
        //first, write the stored value of the code to the output stream
        fputs(string_table[code], decoded_file);
        int idx = 0;
        while (string_table[code][idx] != '\0') {
            curr_word_size ++;
            if (curr_word_size > curr_word_buff_size) {
                curr_word = realloc(curr_word, curr_word_size);
            }
            int str_table_search_idx = find_str_in_table(curr_word, string_table);
            if (str_table_search_idx != -1) {
                //nothing to do
            } else {
                //the entry doesn't exist; update the code for the word, excluding the current
                //character, and then set the current word equal to the current character
                curr_word[curr_word_size-2] = '\0';
                char * copied_word = malloc(sizeof(char)* curr_word_size);
                size_t copy_len = strlcpy(copied_word, curr_word, curr_word_size);
                string_table[string_table_idx++] = copied_word;
                curr_word[0] = string_table[code][idx];
                curr_word[1] = '\0';
                curr_word_size = 2;
            }
            idx ++;
        }
    }
    if (bytesRead == 0 && feof(encoded_file)) {
        printf("End of file reached.\n");
    } else if (bytesRead == 0) {
        perror("Error reading file");
    }



    fclose(encoded_file);
    fclose(decoded_file);
    return 0;
}



int main() {
    lz78_encode("./input.txt", "./output.txt");


}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int count_words(char *, int, int);
//add additional prototypes here
int reverse_string(char *, int);
int count_word_lengths(char *, int);
int replace_string(char*, int, int, int, char**);
int doesWordMatch(char*, int, char*, int*, int);
void insert_to_replace(char*, char*, int, int, int, int);



int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    int buff_index = 0; //keep track of buff index
    int i = 0;
    while(*(user_str + i) != '\0') { //loop until the end of the user string
        if(buff_index == len) {
            return -1; //looped past max buff chars (50)
        }

        char current = *(user_str + i);
        if(current != ' ' && current != '\t') { //non-white space char, so add to buff
            *(buff + buff_index) = current;
            buff_index++;
        } else {

            if( *(buff + buff_index - 1) != ' ' && buff_index != 0) { // if current spot in the buffer is not a space, then add it to buff
                *(buff + buff_index) = ' ';
                buff_index++;
            }
        }
        i++;
    }


    if ( *(buff + buff_index - 1) == ' ') { //if last spot in buff is space reduce the pointer to the end of the buffer by 1
        buff_index--;
    }

    memset(buff + buff_index, '.', len-buff_index);

    
    return buff_index;
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar(']');
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len) {
    int wordCount = 0;

    if(len < str_len) {
        return -1; //if buff size is less than str_len, then there is an error
    }

    if(str_len == 0) return 0;

    for(int i = 0; i < str_len; i++) {
        char current = *(buff + i);

        if(current == ' ') { //if space then there is a word, so update word count
            wordCount++;
        }
    }

    return wordCount + 1; // +1 to account for the last word that does not have a space after
}

int count_word_lengths(char *buff, int str_len) {
    int current_word_len = 0;
    int num_counter = 1;
    printf("Word Print\n");
    printf("----------\n");
    printf("%d. ", num_counter);
    for(int i = 0; i < str_len; i++) {
        char current = *(buff + i);
        if(current == ' ') { //if space then at the end of the word, print the current length and reset to 0 for next word
            printf("(%d)\n", current_word_len);
            current_word_len = 0;
            printf("%d. ", ++num_counter);
            continue;
        }
        putchar(current);
        current_word_len++;
    }
    printf("(%d)\n", current_word_len); //print for last word that does not have a space after
    printf("\n");
    return num_counter;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
int reverse_string(char* buff, int str_len) {
    int left = 0; //left most index
    int right = str_len - 1; //right most index
    
    while(left < right) {
        char temp = *(buff + left); //current char at left index
        *(buff + left) = *(buff + right); //swap left char for right char
        *(buff + right) = temp;//swap right with previous left pointer

        //update pointers
        left++;
        right--;
    }
    return 0;
}


//function to handle swap words
int replace_string(char* buff, int buff_size, int str_len, int argc, char** argv) {
    
    if (argc < 5 || argc > 5) {
        return -1; //to many or to little args
    }

    int found = 0; //0 = false
    char* to_replace = *(argv + 3);
    char* replacement = *(argv + 4);
    int to_replace_start_index = 0;
    int to_replace_end_index = 0;

    for(int i = 0; i < str_len; i++) {
        if(*(buff + i) == *(to_replace)) { //if first char matches with the word we want to replace
            found = doesWordMatch(buff, i, to_replace, &to_replace_end_index, buff_size); //check if word matched
        }

        if(found == 1) {
            to_replace_start_index = i;
            break;
        }
    }
    
    if(found == 0) {
        return -2; //word not found in original string
    }

    insert_to_replace(buff, replacement, to_replace_start_index, to_replace_end_index, str_len, buff_size); //handle the insertion of the 5th token
    return 0;
}


//checks if word is the same as to_replace in the buffer at the index passed in the start_index parameter
int doesWordMatch(char* buff, int start_index, char* to_replace, int* end_index, int buff_size) {
    int i = 0;
    char to_replace_char;
    while( (to_replace_char = *(to_replace + i)) != '\0' && (start_index + i) < buff_size) {
        char buff_char = *(buff + start_index + i);
        
        if (to_replace_char != buff_char) {
            return 0; //return false early if a char does not match
        }

        i++;
    }

    *(end_index) = start_index + i - 1; //set the end index from the mem address passed in the end_index param
    return 1; //return true
}

//handles the changing of the buffer and printing of the buffer after the word to replace has been identified
void insert_to_replace(char* buff, char* replacement, int to_replace_start_index, int to_replace_end_index, int str_len, int buff_size) {
    int replacement_len = 0;
    int copylength;
    int to_replace_len = to_replace_end_index - to_replace_start_index + 1;

    for (int i = 0; *(replacement + i) != '\0'; i++) { //get the length of the replacement string
        replacement_len++;
    }
    
    //copy length not taking into account the buffer
    copylength = str_len - (to_replace_end_index + 1);
    if((to_replace_start_index + replacement_len) + copylength > buff_size) { //handle copy length when it will  be out of buffer size
        copylength = buff_size - (to_replace_start_index + replacement_len);
    }

    //copy the existing characters from the buffer forward to the end of the replacement string
    memcpy(buff + to_replace_start_index + replacement_len, buff + to_replace_end_index + 1, copylength);
    //copy the replacement string into the memory where it should be
    memcpy(buff + to_replace_start_index, replacement, replacement_len);

    // printf("Modified String:  ");
    // for(int i = 0; i < str_len + (replacement_len - to_replace_len); i++) {
    //     putchar(*(buff + i));
    // }
    // printf("\n");

    if (to_replace_start_index + replacement_len + copylength < buff_size) {
        memset(buff + (str_len + (replacement_len - to_replace_len)), '.', buff_size - (str_len + (replacement_len - to_replace_len)));
    }
}


int main(int argc, char *argv[]){

    char *buff;          //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    /*
     *     If the the argument count (argc) is less than 2, then that means that it is one. Which means that no arguments past the
     *     executable name was provided. The first part of the conditional checks that which covers that case, making it safe. 
     * 
     *     If the first character of the second arg (*argv[1]) is not a "-", then that means that the command was not executed correctly. The second part of the
     *     check below covers this, which makes it safe.  
     * 
     */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    /*      
     *  this checks if the argument count is less than 3, specifically 2. Since above we already check if the argument count is one and if the option is h, we can assume that the
     *  user entered an argument that is not h. Meaning that there should be another argument after the second argument. So if there is not (less than 3), then
     *  show the usage and exit with error code 1
     * 
     *  Checks if there are args after the non -h flag, and if not exit with code 1 and print usage.
     */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

    buff = malloc(BUFFER_SZ);

    if(buff == NULL) {
        printf("Memory Allocation Failure!\n");
        exit(99); //the directions in the readme.md say to use err code 2, but above it says 99, so I went with 99
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d\n", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        case 'r':
            rc = reverse_string(buff, user_str_len);
            if (rc < 0){
                printf("Error reversing string, rc = %d\n", rc);
                exit(2);
            }

            break;
        case 'w':
            rc = count_word_lengths(buff, user_str_len);
            if (rc < 0){
                printf("Error counting chars, rc = %d\n", rc);
                exit(2);
            }
            printf("Number of words returned: %d\n", rc);
            break;
        case 'x':
            rc = replace_string(buff, BUFFER_SZ, user_str_len, argc, argv);
            if (rc < 0){
                printf("Error replacing string, rc = %d\n", rc);
                exit(2);
            }
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          It is good practice so that we can change BUFFER_SZ without having to update the value in every spot we need it. If we
//          hard code the 50 value in all spots we need the buffer size, then we will also have to manually update it in every spot.
//          It is good practice since it allows us to only change the variable in one spot and the rest of the progra, does not have
//          to change.
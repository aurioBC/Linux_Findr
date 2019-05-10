/*==============================================================================
|   SOURCE:     findr.c
|
|   DATE:       Sept 4, 2018
|
|   AUTHOR:     Alex Zielinski
|
|   DESC:       This program replicates the CTRL + F feature most applications
|               provide (such as PDF or Word). It allows users to search for
|               a particular string within a text file. The program takes two
|               CMD ARGS. the first one being the text files location. The
|               second being the string to search for. The program will output
|               the following:
|                   - line number within the text file that the string
|                     shows up in
|                   - line of text from that line number
|                   - total number occurances of the string within the text file
|=============================================================================*/
#include <stdio.h>
#include <string.h>

/* MACROS */
#define FILENAME 1
#define STRING 2
#define ARGS 3
#define ARRSIZE 1024
#define FG_BOLD_GREEN  "\033[1;32m"
#define BG_BLUE "\e[44m"
#define COLOR_FLUSH "\033[0m"

/* FUNCTION PROTOTYPES */
int valid_args(int c);
int open_file(char *fn, FILE **fp);
int find_string(char *line, char *word, int *arr);
void search(FILE *fp, char *word);
void zero_array(int *arr);
void add_marker(int *arr, int mark);
void output_result(char *line, int *arr, int size);


/*==============================================================================
|   FUNCTION:   int main(int argc, char *argv[])
|
|   DATE:       Sept 4, 2018
|
|   AUTHOR:     Alex Zielinski
|
|   DESC:       Main entry point of program
|=============================================================================*/
int main(int argc, char *argv[])
{
      FILE *file;

      if(!valid_args(argc))
          return 1;

      if(open_file(argv[FILENAME], &file) == -1)
          return 1;

      printf("\n");
      search(file, argv[STRING]);
      printf("\n");

      fclose(file);
      return 0;
}


/*------------------------------------------------------------------------------
|   FUNCTION:   int find_string(char *line, char *word, int *arr)
|                   *line : line of text to check for find_string
|                   *word : string to look for
|                   *arr : array of markers
|
|   DATE:       Sept 5, 2018
|
|   AUTHOR:     Alex Zielinski
|
|   DESC:       Checks if the string in '*word' exists in the '*line' of text.
|               returns the number of occurances of the string
|-----------------------------------------------------------------------------*/
int find_string(char *line, char *word, int *arr)
{
    int line_size = strlen(line);
    int word_size = strlen(word);
    int i = 0; // line pos
    int j = 0; // word pos
    int l = 0; // marker pos
    int match = 0;
    int sig = 0;

    // search for string in line
    while(1)
    {
        // check for end of line
        if(i == line_size - 1)
            return match;

        if(word_size == 1)          // if string is one character
        {
            if(line[i] == word[j])  // check if string match
            {
                add_marker(arr, i);
                match++;
            }
            i++;
        }
        else                        // else not one character
        {
            if(line[i] == word[j])  // check if string match
            {
                sig = 1;            // sig on
                if(j == word_size - 1) // check for full match
                {
                    add_marker(arr, i - word_size + 1);
                    match++;
                    j = 0;          // reset word pos
                }
                else
                    j++;            // increment word pos
                i++;                // increment line pos
            }
            else
            {
                if(sig != 1) // check sig
                    i++;

                sig = 0;    // sig off
                j = 0;
            }
        }
    }

    return match;
}


/*------------------------------------------------------------------------------
|   FUNCTION:   void search(FILE *fp, char *word)
|                   *fp : file pointer
|                   *word : string to search for
|
|   DATE:       Sept 5, 2018
|
|   AUTHOR:     Alex Zielinski
|
|   DESC:       Iterates through the file '*fp' line by line and calls
|               find_string() to check if string exists in
|               the line of text
|-----------------------------------------------------------------------------*/
void search(FILE *fp, char *word)
{
    char *line = NULL;      // initial line buffer
    ssize_t nread;          // characters read from getline()
    size_t len = 0;
    int line_num = 0;
    int match = 0;
    int tmp = 0;
    int markers[ARRSIZE];

    // iterate line by line
    while ((nread = getline(&line, &len, fp)) != -1)
    {
        zero_array(markers);
        line_num++;                 // get current line number

        if((tmp = find_string(line, word, markers)) != 0)
        {
            printf("\033[1;32m|Line #%d|\n\033[0m", line_num);
            output_result(line, markers, strlen(word));
            match += tmp;
        }
    }

    // print match results
    printf("\n\033[1;32m|Total Matches: %d|\n\033[0m", match);
    free(line);
}


/*------------------------------------------------------------------------------
|   FUNCTION:   int valid_args(int c)
|                   c : number of args
|
|   DATE:       Sept 4, 2018
|
|   AUTHOR:     Alex Zielinski
|
|   DESC:       Returns true if number of args provided is 3. Otherwise returns
|               false
|-----------------------------------------------------------------------------*/
int valid_args(int c)
{
    if(c != ARGS)
    {
        printf("Invalid command line arguments\n\nUsage: ./ctrlf <file> <word to search for>\n\n");
        return 0;
    }

    return 1;
}


/*------------------------------------------------------------------------------
|   FUNCTION:   int open_file(char *fn, FILE **fp)
|                   *fn : file directory
|                   **fp : file to open
|
|   DATE:       Sept 4, 2018
|
|   AUTHOR:     Alex Zielinski
|
|   DESC:       Opens file
|-----------------------------------------------------------------------------*/
int open_file(char *fn, FILE **fp)
{
    // validate operation
    if((*fp = fopen(fn, "r")) == NULL)
    {
        printf("Failure to open\n\n");
        return -1;
    }

    return 0;
}


/*------------------------------------------------------------------------------
|   FUNCTION:   void zero_array(int *arr)
|                   *arr : array of markers
|
|   DATE:       Sept 6, 2018
|
|   AUTHOR:     Alex Zielinski
|
|   DESC:       Sets all elements of marker array to -1
|-----------------------------------------------------------------------------*/
void zero_array(int *arr)
{
    for(int i = 0; i < ARRSIZE; i++)
        arr[i] = -1;
}


/*------------------------------------------------------------------------------
|   FUNCTION:   void add_marker(int *arr, int mark)
|                   *arr : array of markers
|                   mark : marker
|
|   DATE:       Sept 6, 2018
|
|   AUTHOR:     Alex Zielinski
|
|   DESC:       Adds a marker pos to an available marker array element
|-----------------------------------------------------------------------------*/
void add_marker(int *arr, int mark)
{
    for(int i = 0; i < ARRSIZE; i++)
        if(arr[i] == -1)
        {
            arr[i] = mark;
            break;
        }
}


/*------------------------------------------------------------------------------
|   FUNCTION:   void output_result(char *line, int *arr, int size)
|                   *line : line of text from file
|                   *arr : marker array
|                   size : word size
|
|   DATE:       Sept 6, 2018
|
|   AUTHOR:     Alex Zielinski
|
|   DESC:       Outputs the match results to terminal. Outputs the line number
|               where a match occured and prints out the line with the matched
|               string highlighted
|-----------------------------------------------------------------------------*/
void output_result(char *line, int *arr, int size)
{
    int j = 0;

    for(int i = 0; i < strlen(line); i++)
    {
        if(size == 1)  // if word size is 1
        {
            if(i == arr[j]) // check for marker
            {
                printf("%s%c%s", BG_BLUE, line[i], COLOR_FLUSH);
                j++;
            }
            else
                printf("%c", line[i]);
        }
        else
        {
            if(i == arr[j]) // check for marker
            {
                printf("%s%c", BG_BLUE, line[i]);
                j++;
            }
            else if(i == (arr[j - 1] + (size - 1)))
                printf("%c%s", line[i], COLOR_FLUSH);
            else
                printf("%c", line[i]);
        }
    }
}

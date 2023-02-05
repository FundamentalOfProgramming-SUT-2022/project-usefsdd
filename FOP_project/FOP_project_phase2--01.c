// Email: usefsadidi@gmail.com
// Telegram: @usefsdd
// january 2023
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <curses.h>
#include <stdbool.h>
#include <dirent.h>
// defining errors
#define FILEEXIST mvprintw((row_2-1),0,"%50s","ERROR: THIS FILE IS ALREADY EXIST...");
#define NOPOS mvprintw((row_2-1),0,"%50s","ERROR: INCORRECT POSITION...");
#define NOFILE mvprintw((row_2-1),0,"%50s","ERROR: FILE NOT EXIST...");
#define NODIR mvprintw((row_2-1),0,"%50s","ERROR: WRONG PATH...");
#define NOSIZE mvprintw((row_2-1),0,"%50s","ERROR: WRONG SIZE...");
#define NOACC mvprintw((row_2)-1,0,"%50s","ERROR: NOT ALLOWED TO TOUCH THIS PATH...");
// a function for concating strings
char *concat(int count, ...)
{
    va_list ap;
    int i;
    int len = 1;
    va_start(ap, count);
    for (i = 0; i < count; i++)
        len += strlen(va_arg(ap, char *));
    va_end(ap);
    char *merged = calloc(sizeof(char), len);
    int null_pos = 0;
    va_start(ap, count);
    for (i = 0; i < count; i++)
    {
        char *s = va_arg(ap, char *);
        strcpy(merged + null_pos, s);
        null_pos += strlen(s);
    }
    va_end(ap);
    return merged;
}
// file path & string extraction
char *extraction(char *command, char *control, int find)
{
    int lenght = strlen(control);
    char *out = (char *)malloc(900 * sizeof(char));
    char *ptr = strstr(command, control);
    int type = 0;
    char *string_index;
    int index = 0;
    if (*(ptr + lenght) == '"')
    {
        type = 2;
        string_index = ptr + lenght + 1;
        
    }
    else
    {
        type = 1;
        string_index = ptr + lenght;
    }
    while (1)
    {
        if (*(string_index) == '\"' && *(string_index - 1) != '\\' && type == 2)
        {
            out[index] = '\0';
            break;
        }
        if ((*(string_index) == ' ' || *(string_index) == '\0') && type == 1)
        {
            out[index] = '\0';
            break;
        }
        if (*(string_index) == '\\')
        {
            if (*(string_index + 1) == '\\' && *(string_index + 2) == 'n')
            {
                out[index] = '\\';
                out[index + 1] = 'n';
                index += 2;
                string_index += 3;
            }
            else if (*(string_index + 1) == 'n')
            {
                out[index] = '\n';
                index++;
                string_index += 2;
            }
            else if (*(string_index + 1) == '\\' || *(string_index + 1) == '\"')
            {
                out[index] = *(string_index + 1);
                index++;
                string_index += 2;
            }
            else if (*(string_index + 1) == '*' && find == 0)
            {
                out[index] = *(string_index + 1);
                index++;
                string_index += 2;
            }
        }
        else
        {
            out[index] = *string_index;
            index++;
            string_index++;
        }
    }
    return out;
}
// convert the path to windows standard form
char *full_path(char *command)
{
    char *path = (char *)malloc(200 * sizeof(char));
    strcpy(path, command);
    while (1)
    {
        char *place = strstr(path, "/");
        if (place == NULL)
            break;
        *place = '\\';
    }
    return path+1;
}
// string position extraction
int *position_extraction(char *command)
{
    char *s = (char *)malloc(1000 * sizeof(char));
    strcpy(s, command);
    char *position = (char *)malloc(50 * sizeof(char));
    char *pos = strstr(s, "--pos ");
    if (pos == NULL)
        return NULL;
    position = strtok(pos + 6, " ");
    char *lineNum = strtok(position, ":");
    char *startPos = strtok(NULL, "");
    int *pospos = (int *)malloc(2 * sizeof(int));
    pospos[0] = atoi(lineNum);
    pospos[1] = atoi(startPos);
    return pospos;
}
// string size extraction
int size_extraction(char *command)
{
    char *s = (char *)malloc(1000 * sizeof(char));
    strcpy(s, command);
    char *size = (char *)malloc(10 * sizeof(char));
    char *siz = strstr(s, "-size ");
    if (siz == NULL)
        return -1;
    size = strtok(siz + 6, " ");
    return atoi(size);
}
// get options
int options(char *command, char *optionName)
{

    char *s = (char *)malloc(1000 * sizeof(char));
    strcpy(s, command);
    char *option = strstr(s, optionName);
    if (option == NULL)
        return 0;
    return 1;
}
// at option
int at_option(char *command)
{
    char *s = (char *)malloc(1000 * sizeof(char));
    strcpy(s, command);
    char *at = strstr(s, "-at ");
    if (at == NULL)
        return 1;
    char *num = strtok(at + 4, " ");
    return atoi(num);
}
// check existance of a file or directory
int check_existance(char *path)
{
    if (strstr(path,"..setting") != NULL || strstr(path,"..undoroot") != NULL)
        return -3;
    char dir[200];
    int end = strlen(path) - 1;
    for (int i = end; i >= 0; i--)
    {
        if (path[i] == '\\')
        {
            end = i;
            break;
        }
    }
    for (int j = 0; j < end; j++)
    {
        dir[j] = path[j];
        dir[j + 1] = '\0';
    }
    if (access(dir, F_OK) != 0)
        return -1;

    if (access(path, F_OK) != 0)
        return -2;
    return 0;
}
// command's function
int create(char *path)
{
    char *address = (char *)malloc(200 * sizeof(char));
    address = strtok(path, "\\");
    char *dir = (char *)malloc(20 * sizeof(char));
    dir = strtok(NULL, "\\");
    char *dir2 = (char *)malloc(20 * sizeof(char));
    dir2 = strtok(NULL, "\\");
    while (dir2 != NULL)
    {
        address = concat(3, address, "\\", dir);
        _mkdir(address);
        strcpy(dir, dir2);
        dir2 = strtok(NULL, "\\");
    }
    char *file = concat(3, address, "\\", dir);
    if (access(file, F_OK) == 0)
        return -1;
    else
    {
        FILE *fptr;
        fptr = fopen(file, "w");
        fclose(fptr);
        if (dir[0] == '.')
            system(concat(2, "attrib +s +h ", file));
    }
}
int insert(char *path, char *string, int *position)
{
    int line = 1;
    int posInLine = 0;
    FILE *file = fopen(path, "r");
    FILE *insert = fopen("root\\..setting\\insert.txt", "w");
    while (1)
    {
        if (line == position[0] && posInLine == position[1])
            break;
        char b = fgetc(file);
        if (b == EOF)
        {
            fclose(insert);
            fclose(file);
            remove("root\\..setting\\insert.txt");
            return -5;
        }
        fputc(b, insert);
        if (b == '\n')
        {
            if (line == position[0])
            {
                fclose(insert);
                fclose(file);
                remove("root\\..setting\\insert.txt");
                return -5;
            }
            line++;
            posInLine = 0;
        }
        else
        {
            posInLine++;
        }
    }
    fprintf(insert, "%s", string);
    while (1)
    {
        char b = fgetc(file);
        if (b == EOF)
            break;
        fputc(b, insert);
    }
    fclose(insert);
    fclose(file);
    file = fopen(path, "w");
    insert = fopen("root\\..setting\\insert.txt", "r");
    while (1)
    {
        char b = fgetc(insert);
        if (b == EOF)
            break;
        fputc(b, file);
    }
    fclose(insert);
    fclose(file);
    remove("root\\..setting\\insert.txt");
}
int cat(char *path)
{
    FILE *file = fopen(path, "r");
    while (1)
    {
        char c = fgetc(file);
        if (c == EOF)
            break;
        printf("%c", c);
    }
    printf("\n");
    fclose(file);
}
int removestr(char *path, int *position, int size, int backfor)
{
    int removepos = 0;
    int posall = 0;
    int line = 1;
    int posInLine = 0;
    FILE *file = fopen(path, "r");
    FILE *removee;
    if (backfor == 1)
        removee = fopen("root\\..setting\\removee.txt", "w");
    while (1)
    {
        if ((line == position[0] && posInLine == position[1]) || posall == position[2])
            break;
        char b = fgetc(file);
        if (backfor == 1)
            fputc(b, removee);
        if (b == EOF)
        {
            if (backfor == 1)
            {
                fclose(removee);
                remove("root\\..setting\\removee.txt");
            }
            fclose(file);
            return -5;
        }
        if (b == '\n')
        {
            if (line == position[0])
            {
                if (backfor == 1)
                {
                    fclose(removee);
                    remove("root\\..setting\\removee.txt");
                }
                fclose(file);
                return -5;
            }
            line++;
            posInLine = 0;
        }
        else
        {
            posInLine++;
        }
        posall++;
    }
    if (backfor == 1)
    {
        for (int i = 0; i < size; i++)
        {
            char b = fgetc(file);
            if (b == EOF)
            {
                if (backfor == 1)
                {
                    fclose(removee);
                    remove("root\\..setting\\removee.txt");
                }
                fclose(file);
                return -6;
            }
        }
    }
    else
    {
        if (size > posall)
            return -6;
        removepos = posall;
    }
    while (1)
    {
        char b = fgetc(file);
        if (b == EOF)
            break;
        if (backfor == 1)
            fputc(b, removee);
    }
    if (backfor == 1)
        fclose(removee);
    fclose(file);
    if (backfor == 1)
    {
        file = fopen(path, "w");
        removee = fopen("root\\..setting\\removee.txt", "r");
        while (1)
        {
            char b = fgetc(removee);
            if (b == EOF)
                break;
            fputc(b, file);
        }
        fclose(removee);
        fclose(file);
        remove("root\\..setting\\removee.txt");
    }
    if (backfor == -1)
    {
        int pos[3] = {-1, -1, removepos - size};
        removestr(path, pos, size, 1);
    }
}
int copy(char *path, int *position, int size, int backfor)
{
    int copypos = 0;
    int posall = 0;
    int line = 1;
    int posInLine = 0;
    FILE *file = fopen(path, "r");
    while (1)
    {
        if ((line == position[0] && posInLine == position[1]) || posall == position[2])
            break;
        char b = fgetc(file);
        if (b == EOF)
        {
            fclose(file);
            return -5;
        }
        if (b == '\n')
        {
            if (line == position[0])
            {
                fclose(file);
                return -5;
            }
            line++;
            posInLine = 0;
        }
        else
        {
            posInLine++;
        }
        posall++;
    }
    if (backfor == 1)
    {
        FILE *clipboard = fopen("root\\..setting\\clipboard.txt", "w");
        for (int i = 0; i < size; i++)
        {
            char b = fgetc(file);
            if (b == EOF)
            {
                fclose(clipboard);
                fclose(file);
                return -6;
            }
            fputc(b, clipboard);
        }
        fclose(file);
        fclose(clipboard);
    }
    else
    {
        if (size > posall)
            return -6;
        copypos = posall;
    }
    if (backfor == -1)
    {
        int pos[3] = {-1, -1, copypos - size};
        copy(path, pos, size, 1);
    }
}
int cut(char *path, int *position, int size, int backfor)
{
    int cutpos = 0;
    int posall = 0;
    int line = 1;
    int posInLine = 0;
    FILE *file = fopen(path, "r");
    FILE *removee;
    if (backfor == 1)
        removee = fopen("root\\..setting\\removee.txt", "w");
    while (1)
    {
        if ((line == position[0] && posInLine == position[1]) || posall == position[2])
            break;
        char b = fgetc(file);
        if (backfor == 1)
            fputc(b, removee);
        if (b == EOF)
        {
            if (backfor == 1)
            {
                fclose(removee);
                remove("root\\..setting\\removee.txt");
            }
            fclose(file);
            return -5;
        }
        if (b == '\n')
        {
            if (line == position[0])
            {
                if (backfor == 1)
                {
                    fclose(removee);
                    remove("root\\..setting\\removee.txt");
                }
                fclose(file);
                return -5;
            }
            line++;
            posInLine = 0;
        }
        else
        {
            posInLine++;
        }
        posall++;
    }
    if (backfor == 1)
    {
        FILE *clipboard = fopen("root\\..setting\\clipboard.txt", "w");
        for (int i = 0; i < size; i++)
        {
            char b = fgetc(file);
            if (b == EOF)
            {
                fclose(removee);
                remove("root\\..setting\\removee.txt");
                fclose(file);
                return -6;
            }
            fputc(b, clipboard);
        }
        fclose(clipboard);
    }
    else
    {
        if (size > posall)
            return -6;
        cutpos = posall;
    }
    while (1)
    {
        char b = fgetc(file);
        if (b == EOF)
            break;
        if (backfor == 1)
            fputc(b, removee);
    }
    if (backfor == 1)
        fclose(removee);
    fclose(file);
    if (backfor == 1)
    {
        file = fopen(path, "w");
        removee = fopen("root\\..setting\\removee.txt", "r");
        while (1)
        {
            char b = fgetc(removee);
            if (b == EOF)
                break;
            fputc(b, file);
        }
        fclose(removee);
        fclose(file);
        remove("root\\..setting\\removee.txt");
    }
    if (backfor == -1)
    {
        int pos[3] = {-1, -1, cutpos - size};
        cut(path, pos, size, 1);
    }
}
int paste(char *path, int *position)
{
    int line = 1;
    int posInLine = 0;
    FILE *file = fopen(path, "r");
    FILE *insert = fopen("root\\..setting\\insert.txt", "w");
    while (1)
    {
        if (line == position[0] && posInLine == position[1])
            break;
        char b = fgetc(file);
        if (b == EOF)
        {
            fclose(insert);
            fclose(file);
            remove("root\\..setting\\insert.txt");
            return -5;
        }
        fputc(b, insert);
        if (b == '\n')
        {
            if (line == position[0])
            {
                fclose(insert);
                fclose(file);
                remove("root\\..setting\\insert.txt");
                return -5;
            }
            line++;
            posInLine = 0;
        }
        else
        {
            posInLine++;
        }
    }
    FILE *clipboard = fopen("root\\..setting\\clipboard.txt", "r");
    while (1)
    {
        char b = fgetc(clipboard);
        if (b == EOF)
            break;
        fputc(b, insert);
    }
    fclose(clipboard);
    while (1)
    {
        char b = fgetc(file);
        if (b == EOF)
            break;
        fputc(b, insert);
    }
    fclose(insert);
    fclose(file);
    file = fopen(path, "w");
    insert = fopen("root\\..setting\\insert.txt", "r");
    while (1)
    {
        char b = fgetc(insert);
        if (b == EOF)
            break;
        fputc(b, file);
    }
    fclose(insert);
    fclose(file);
    remove("root\\..setting\\insert.txt");
}
int find(char *path, char *position, int at, int count, int byword, int all) {}
int replace(char *command) {}
int grep(char *path, char *string)
{
    char *line = malloc(1000000);
    FILE *file = fopen(path, "r");
    FILE *grep = fopen("root\\..setting\\grep.txt", "a");
    FILE *grepl = fopen("root\\..setting\\grepl.txt", "a");
    int exist = 0;
    while (1)
    {
        line[0] = '\0';
        fgets(line, 1000000, file);
        if (line[0] == '\0')
            break;
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';
        if (strstr(line, string) != NULL)
        {
            fprintf(grep, "%s : %s\n", path, line);
            exist++;
        }
    }
    if (exist > 0)
        fprintf(grepl, "%s\n", path);
    fclose(grepl);
    fclose(grep);
    fclose(file);
}
void undo_backup(char *path)
{
    create(concat(2, "root\\..undo", path));
    FILE *file = fopen(path, "r");
    FILE *undo = fopen("root\\..setting\\undo.txt", "w");
    while (1)
    {
        char c = fgetc(file);
        if (c == EOF)
            break;
        fputc(c, undo);
    }
    fclose(file);
    fprintf(undo, "\n$$END OF THE VERSION$$\n");
    FILE *undolist = fopen(concat(2, "root\\..undo", path), "r");
    while (1)
    {
        char c = fgetc(undolist);
        if (c == EOF)
            break;
        fputc(c, undo);
    }
    fclose(undolist);
    fclose(undo);
    undolist = fopen(concat(2, "root\\..undo", path), "w");
    undo = fopen("root\\..setting\\undo.txt", "r");
    while (1)
    {
        char c = fgetc(undo);
        if (c == EOF)
            break;
        fputc(c, undolist);
    }
    fclose(undo);
    fclose(undolist);
    remove("root\\..setting\\undo.txt");
}
void undo_delete_last_record(char *path)
{
    FILE *undolist = fopen(concat(2, "root\\..undo", path), "r");
    FILE *undo = fopen("root\\..setting\\undo.txt", "w");
    char *line = (char *)malloc(100000 * sizeof(char));
    while (1)
    {
        bool end = false;
        char *check = "$$END OF THE VERSION$$";
        line[0] = '\0';
        fgets(line, 100000, undolist);
        if (line[0] == '\0')
            return;
        for (int i = 0; i < 22; i++)
        {
            if (line[i] != check[i])
                break;
            if (i == 21)
                end = true;
        }
        if (end == true)
            break;
    }
    while (1)
    {
        char c = getc(undolist);
        if (c == EOF)
            break;
        fputc(c, undo);
    }
    fclose(undo);
    fclose(undolist);
    undolist = fopen(concat(2, "root\\..undo", path), "w");
    undo = fopen("root\\..setting\\undo.txt", "r");
    while (1)
    {
        char x = fgetc(undo);
        if (x == EOF)
            break;
        fputc(x, undolist);
    }
    fclose(undo);
    fclose(undolist);
    remove("root\\..setting\\undo.txt");
}
void undo(char *path)
{
    FILE *undolist = fopen(concat(2, "root\\..undo", path), "r");
    FILE *file = fopen(path, "w");
    FILE *undo = fopen("root\\..setting\\undo.txt", "w");
    char *line = (char *)malloc(100000 * sizeof(char));
    bool firstLine = true;
    while (1)
    {
        bool end = false;
        char *check = "$$END OF THE VERSION$$";
        line[0] = '\0';
        fgets(line, 100000, undolist);
        if (line[0] == '\0' && firstLine == true)
            return;
        for (int i = 0; i < 22; i++)
        {
            if (line[i] != check[i])
                break;
            if (i == 21)
                end = true;
        }
        if (end == true)
            break;
        if (firstLine == false)
            fputc('\n', file);
        // fprintf(file, "%s", line);
        for (int i = 0; i < strlen(line) - 1; i++)
        {
            fputc(line[i], file);
        }

        firstLine = false;
    }
    while (1)
    {
        char c = fgetc(undolist);
        if (c == EOF)
            break;
        fputc(c, undo);
    }
    fclose(file);
    fclose(undo);
    fclose(undolist);
    undolist = fopen(concat(2, "root\\..undo", path), "w");
    undo = fopen("root\\..setting\\undo.txt", "r");
    while (1)
    {
        char c = fgetc(undo);
        if (c == EOF)
            break;
        fputc(c, undolist);
    }
    fclose(undo);
    fclose(undolist);
    remove("root\\..setting\\undo.txt");
}
void indentation(char *path)
{
    FILE * file = fopen(path,"r");
    FILE * indentFile = fopen("root\\..setting\\indentfile.txt","w");
    int indent = 0;         //indent of each line
    int cursorPos = 0;      //last writed pos
    int nowPos = 0;   
    int oneSpace = 0;       //(bool)space between line & {
    int lineStart = 0;  //(bool)check if the line started
    char c;
    while (1)
    {
        c = fgetc(file);
        nowPos++;
        //find the position of last char before { or }
        if (c == '{' || c =='}')
        {
            char x;
            while (1)
            {
                fseek(file,-2,SEEK_CUR);
                x = fgetc(file);
                nowPos--;
                if (x == '\n')
                    fseek(file,-1,SEEK_CUR);
                if (x != ' ' && x != '\n')
                {
                    if (x == '{' || x == '}' || x == EOF)
                        oneSpace = 0;
                    else
                        oneSpace = 1;
                    break;
                }
            }
            //get to the last writed char
            fseek(file,0,SEEK_SET);
            for (int i = 0; i < cursorPos; i++)
                fgetc(file);
            //write up to the nowpos
            for (int j = cursorPos; j < nowPos; j++)
            {
                char print;
                print = fgetc(file);
                if (!((print == ' ' || print == '\n') && lineStart == 0))
                {
                    lineStart = 1;
                    fputc(print, indentFile);
                    if (print == '\n')
                    {
                        lineStart = 0;
                        for (int i = 0; i < 4 * indent; i++)
                            fputc(' ', indentFile);
                    }
                }
            }
            // print {}
            if (c == '{')
            {
                indent++;
                if (oneSpace == 1)
                    fprintf(indentFile," {\n");
                else if (oneSpace == 0)
                    fprintf(indentFile,"{\n");
                for (int i = 0; i < 4 * indent; i++)
                        fputc(' ', indentFile);
            }
            if (c == '}')
            {
                indent--;
                fputc('\n',indentFile);
                for (int i = 0; i < 4 * indent; i++)
                        fputc(' ', indentFile);
                fprintf(indentFile,"}\n");
                for (int i = 0; i < 4 * indent; i++)
                        fputc(' ', indentFile);
            }
            //get back to {} (ahead)
            while (1)
            {
                char b = fgetc(file);
                nowPos++;
                if (b == '{' || b == '}')
                {
                    cursorPos = nowPos;
                    break;
                }
            }
        }
        //end of the program
        if (c == EOF)
        {
            break;
        }
        
    }
    fclose(file);
    fclose(indentFile);
    //print corrected text in the file
    file = fopen(path,"w");
    indentFile = fopen("root\\..setting\\indentfile.txt","r");
    char * line = malloc(1000000);
    while (1)
    {
        line[0] = '\0';
        fgets(line,1000000,indentFile);
        if (line[0] == '\0')
            break;
        int flagPrintLine = 0;      //check lines and not print free lines
        for (int i = 0; i < strlen(line); i++)
        {
            if (line[i] != ' ' && line[i] != '\n' && line[i] != '\0')
            {
                flagPrintLine = 1;
                break;
            }
        }
        if (flagPrintLine == 1)
            fprintf(file,"%s",line);
    }
    free(line);
    fclose(file);
    fclose(indentFile);
    remove("root\\..setting\\indentfile.txt");
}
int compare(char *path1, char *path2)
{
    FILE *file1 = fopen(path1, "r");
    FILE *file2 = fopen(path2, "r");
    char *linef1 = (char *)malloc(100000);
    char *linef2 = (char *)malloc(100000);
    int lineNum = 0;
    int longerFile = 0;
    int longerStartLine = 0;
    int longerEndLine = 0;
    while (1)
    {
        lineNum++;
        linef1[0] = '\0';
        linef2[0] = '\0';
        fgets(linef1, 100000, file1);
        fgets(linef2, 100000, file2);
        if (linef1[0] == '\0' && linef2[0] == '\0')
        {
            longerEndLine = lineNum - 1;
            break;
        }
        if (linef1[0] != '\0' && linef2[0] == '\0')
        {
            if (longerFile == 0)
                longerStartLine = lineNum;
            longerFile = 1;
            continue;
        }
        if (linef1[0] == '\0' && linef2[0] != '\0')
        {
            if (longerFile == 0)
                longerStartLine = lineNum;
            longerFile = 2;
            continue;
        }
        if (linef1[strlen(linef1) - 1] == '\n')
            linef1[strlen(linef1) - 1] = '\0';
        if (linef2[strlen(linef2) - 1] == '\n')
            linef2[strlen(linef2) - 1] = '\0';
        if (strcmp(linef1, linef2))
            printf("============ #%d ============\n%s\n%s\n", lineNum, linef1, linef2);
    }
    if (longerFile == 1)
    {
        printf("=====11===== #%d - #%d =====11=====\n", longerStartLine, lineNum - 1);
        fseek(file1, 0, SEEK_SET);
        for (int i = 1; i < longerStartLine; i++)
            fgets(linef1, 100000, file1);
        for (int i = longerStartLine; i <= longerEndLine; i++)
        {
            fgets(linef1, 100000, file1);
            if (linef1[strlen(linef1) - 1] == '\n')
                linef1[strlen(linef1) - 1] = '\0';
            printf("%s\n", linef1);
        }
    }
    if (longerFile == 2)
    {
        printf("=====22===== #%d - #%d =====22=====\n", longerStartLine, lineNum - 1);
        fseek(file2, 0, SEEK_SET);
        for (int i = 1; i < longerStartLine; i++)
            fgets(linef2, 100000, file2);
        for (int i = longerStartLine; i <= longerEndLine; i++)
        {
            fgets(linef2, 100000, file2);
            if (linef2[strlen(linef2) - 1] == '\n')
                linef2[strlen(linef2) - 1] = '\0';
            printf("%s\n", linef2);
        }
    }
    fclose(file1);
    fclose(file2);
}
int directory_tree(int depth)
{
    if (depth == 1)
    {
        DIR *d;
        struct dirent *dir;
        int dirCount = 0;
        d = opendir("root");
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_name[0] != '.')
                dirCount++;
        }
        closedir(d);
        d = opendir("root");
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_name[0] != '.')
            {
                if (dirCount == 0)
                    break;
                if (dirCount > 1)
                    printf("|-----%s\n", dir->d_name);
                if (dirCount == 1)
                    printf("|_____%s\n", dir->d_name);
                dirCount--;
            }
        }
        closedir(d);
    }
    if (depth == 2)
    {
        struct dirent *dir;
        int dirCount = 0;
        DIR *d = opendir("root");
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_name[0] != '.')
                dirCount++;
        }
        closedir(d);
        d = opendir("root");
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_name[0] != '.')
            {
                if (dirCount == 0)
                    break;
                if (dirCount > 1)
                    printf("|-----%s\n", dir->d_name);
                if (dirCount == 1)
                    printf("|_____%s\n", dir->d_name);
                char *addr = concat(2, "root\\", dir->d_name);
                struct dirent *ddir;
                int ddirCount = 0;
                DIR *dd = opendir(addr);
                while ((ddir = readdir(dd)) != NULL)
                {
                    if (ddir->d_name[0] != '.')
                        ddirCount++;
                }
                closedir(dd);
                dd = opendir(addr);
                while ((ddir = readdir(dd)) != NULL)
                {
                    if (ddir->d_name[0] != '.')
                    {
                        if (dirCount > 1)
                        {
                            if (ddirCount == 0)
                                break;
                            if (ddirCount > 1)
                                printf("|  |-----%s\n", ddir->d_name);
                            if (ddirCount == 1)
                                printf("|  |_____%s\n", ddir->d_name);
                        }
                        if (dirCount == 1)
                        {
                            if (ddirCount == 0)
                                break;
                            if (ddirCount > 1)
                                printf("   |-----%s\n", ddir->d_name);
                            if (ddirCount == 1)
                                printf("   |_____%s\n", ddir->d_name);
                        }
                        ddirCount--;
                    }
                }
                dirCount--;
                closedir(dd);
            }
        }
        closedir(d);
    }
}
int pipe() {}
/// phase 2
char * path_2;
char * fileName_2;
int temp_2;
int save_state_2;
int row_2, col_2;
int curLine_2;
int curPos_2;
int Vline_2;
int Vpos_2;
int firstLineNum_2;
void open_file()
{
    FILE *file = fopen(path_2, "r");
    FILE *workingFile = fopen("root\\..setting\\workingFile.txt", "w");
    char c;
    while ((c = fgetc(file)) != EOF)
    {
        fputc(c, workingFile);
    }
    fclose(workingFile);
    fclose(file);
    char dir[1000];
    strcpy(dir, path_2);
    char *dir1 = strtok(dir, "\\");
    char *dir2 = strtok(NULL, "\\");
    while (1)
    {
        if (dir2 == NULL)
        {
            strcpy(fileName_2, dir1);
            break;
        }
        strcpy(dir1, dir2);
        dir2 = strtok(NULL, "\\");
    }
    save_state_2 = 43;
    curLine_2 = -1;
    curPos_2 = -1;
    firstLineNum_2 = 1;
}
void save_file()
{
    FILE *file = fopen(path_2, "w");
    FILE *workingFile = fopen("root\\..setting\\workingFile.txt", "r");
    char c;
    while ((c = fgetc(workingFile)) != EOF)
    {
        fputc(c, file);
    }
    fclose(workingFile);
    fclose(file);
    save_state_2 = 32;
}
void saveAs_file(char * save_path)
{
    char * pathh = malloc(200);
    strcpy(pathh, save_path);
    printw("%s",pathh);
    create(save_path);
    FILE *file = fopen(pathh, "w");
    FILE *workingFile = fopen("root\\..setting\\workingFile.txt", "r");
    char c;
    while ((c = fgetc(workingFile)) != EOF)
    {
        fputc(c, file);
    }
    fclose(file);
    fclose(workingFile);
    free(pathh);
}
void undo_file(){}
int get_command()
{
    char command[1000];
    getstr(command);
    if (command[0] == '0' && command[1] == '0')
    {
        exit(0);
        return 0;
    }
    char *opName = strtok(command, " ");
    char *other = strtok(NULL, "");
    if (!strcmp(opName, "open"))
    {
        char * pathh = full_path(other);
        
        if (check_existance(pathh) == -1)
        {
            NODIR
            getch();
        }else if (check_existance(pathh) == -2)
        {
            NOFILE
            getch();
        }else if (check_existance(pathh) == -3)
        {
            NOACC
            getch();
        }else
        {
            if (temp_2 == 0)
                temp_2 = 1;
            else
                save_file();
            strcpy(path_2, pathh);
            open_file();
        }
        return 0;
    }else if (!strcmp(opName, "undo"))
    {
        undo_file();
        return 0;
    }else if (!strcmp(opName, "save"))
    {
        save_file();
        return 0;
    }else if (!strcmp(opName, "saveas"))
    {
        char * pathh = full_path(other);
        saveAs_file(pathh);
        return 0;
    }else if (!strcmp(opName, "auto-indent"))
    {
        indentation("root\\..setting\\workingfile.txt");
        curPos_2 = -1;
        curPos_2 = -1;
        return 0;
    }else if (!strcmp(opName, "copy"))
    {
        char * pathh = full_path(other);
        saveAs_file(pathh);
        return 0;
    }else
    {
        return 0;
    }
}
int visual_mode()
{
    int lenght = 0;
    int actionStartPosition[2];
    clear();
    move(row_2 - 2, 0);
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    attron(COLOR_PAIR(1));
    printw("  VISUAL  ");
    attroff(COLOR_PAIR(1));
    init_pair(2, COLOR_WHITE, COLOR_CYAN);
    attron(COLOR_PAIR(2));
    printw("  %10s   %c ", fileName_2, save_state_2);
    attroff(COLOR_PAIR(2));
    init_pair(3, COLOR_BLACK, COLOR_MAGENTA);
    chgat(-1, COLOR_PAIR(3), 0, NULL);
    move(0, 0);
    int lineCount = 0;
    char * line = malloc(1000000);
    //line count
    FILE * workingFile = fopen("root\\..setting\\workingfile.txt", "r");
    while (1)
    {
        line[0] = '\0';
        fgets(line, 1000000, workingFile);
        if (line[0] == '\0')
            break;
        lineCount++;
    }
    fclose(workingFile);
    //get to the print line
    workingFile = fopen("root\\..setting\\workingfile.txt", "r");
    int lineNum = 1;
    for (lineNum; lineNum < firstLineNum_2; lineNum++)
        fgets(line, 1000000, workingFile);
    //print lines
    int firstline = lineNum;
    for (lineNum; lineNum < (firstline + row_2 - 2); lineNum++)
    {
        if (lineNum > lineCount)
            break;
        fgets(line, 1000000, workingFile);
        if (line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';
        init_pair(4, COLOR_WHITE, COLOR_GREEN);
        attron(COLOR_PAIR(4));
        printw(" %5d  ", lineNum);
        attroff(COLOR_PAIR(4));
        printw("%s", line);
        if (lineNum < firstline + row_2 - 3 && lineNum < lineCount)
            printw("\n");
    }
    fclose(workingFile);
    refresh();
    //set cursor position
    if (curLine_2 == -1 && curPos_2 == -1)
    {
        getyx(stdscr , curLine_2 , curPos_2);
    }
    else
    {
        workingFile = fopen("root\\..setting\\workingfile.txt", "r");
        for (lineNum = 0; lineNum <= curLine_2 + firstLineNum_2 - 1; lineNum++)
            fgets(line, 1000000, workingFile);
        fclose(workingFile);
        if (lineNum != lineCount && curPos_2 > (strlen(line)-1) + 8)
            curPos_2 = (strlen(line)-1) + 8;
        if (lineNum == lineCount && curPos_2 > strlen(line) + 8)
            curPos_2 = strlen(line) + 8;
    }
    move(curLine_2, curPos_2);
    //\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//
    int startHighlight[2];
    int endHighlight[2];
    if (firstLineNum_2 + curLine_2  < Vline_2)
    {
        startHighlight[0] = firstLineNum_2 + curLine_2;
        startHighlight[1] = curPos_2 - 8;
        endHighlight[0] = Vline_2;
        endHighlight[1] = Vpos_2;
    }else if (firstLineNum_2 + curLine_2 > Vline_2)
    {
        startHighlight[0] = Vline_2;
        startHighlight[1] = Vpos_2;
        endHighlight[0] = firstLineNum_2 + curLine_2;
        endHighlight[1] = curPos_2 - 8;
    }else if (firstLineNum_2 + curLine_2 == Vline_2)
    {
        startHighlight[0] = Vline_2;
        endHighlight[0] = Vline_2;
        if (curPos_2 - 8 <= Vpos_2)
        {
            startHighlight[1] = curPos_2 - 8;
            endHighlight[1] = Vpos_2;
        }
        if (curPos_2 - 8 > Vpos_2)
        {
            startHighlight[1] = Vpos_2;
            endHighlight[1] = curPos_2 - 8;
        }
    }
    actionStartPosition[0] = startHighlight[0];
    actionStartPosition[1] = startHighlight[1];
    workingFile = fopen("root\\..setting\\workingfile.txt", "r");
    for (int i = 1; i < startHighlight[0]; i++)
        fgets(line, 1000000, workingFile);
    for (int i = 0; i < startHighlight[1]; i++)
        fgetc(workingFile);
    if (startHighlight[0] < firstLineNum_2)
        move(0,8);
    else
        move(startHighlight[0] - firstLineNum_2, startHighlight[1] + 8);
    init_pair(5, COLOR_BLACK, COLOR_WHITE);
    attron(COLOR_PAIR(5));
    char c;
    while (1)
    {
        if (startHighlight[0] == endHighlight[0] && startHighlight[1] == endHighlight[1])
            break;
        c = fgetc(workingFile);
        if (c == '\n')
        {
            startHighlight[0]++;
            startHighlight[1] = 0;
            if (startHighlight[0] >= firstLineNum_2 && startHighlight[0] <= firstLineNum_2 + row_2 - 3)
            {
                move(startHighlight[0] - firstLineNum_2,8);
            }
        }else
        {
            if (startHighlight[0] >= firstLineNum_2 && startHighlight[0] <= firstLineNum_2 + row_2 - 3)
                printw("%c", c);
            startHighlight[1]++;
        }
        lenght++;
    }
    attroff(COLOR_PAIR(5));
    fclose(workingFile);
    move(curLine_2,curPos_2);
    //\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//\//
    while (1)
    {
        int ch = getch();
        switch (ch)
        {
        case KEY_F(1):
            exit(0);
            return 2;
            break;
        case 27:
            return 0;
            break;
        case 'k':
            if (curLine_2 > 0)
            {
                if (curLine_2 <= 3 && firstLineNum_2 > 1)
                    (firstLineNum_2)--;
                else
                    (curLine_2)--;
            }
            return 2;
            break;
        case 'j':
            if (curLine_2 < lineCount - 1 && curLine_2 < row_2 - 3)
            {
                if (curLine_2 >= row_2 - 6 && lineCount > firstLineNum_2 + row_2 - 2 )
                    (firstLineNum_2)++;
                else
                    (curLine_2)++;
            }
            return 2;
            break;
        case 'l':
            workingFile = fopen("root\\..setting\\workingfile.txt", "r");
            for (int i = 0; i <= curLine_2 + firstLineNum_2 - 1; i++)
            {
                fgets(line, 1000000, workingFile);
            }
            fclose(workingFile);
            if ((line[strlen(line)-1] == '\n' && curPos_2 < strlen(line) + 7) || (line[strlen(line)-1] != '\n' && curPos_2 < strlen(line) + 8))
                (curPos_2)++;
            return 2;
            break;
        case 'h':
            if (curPos_2 > 8)
                (curPos_2)--;
            return 2;
            break;
        case 'd':
            removestr("root\\..setting\\workingfile.txt",actionStartPosition,lenght,1);
            curLine_2 = -1;
            curPos_2 = -1;
            save_state_2 = 43;
            return 0;
            break;
        case 'y':
            copy("root\\..setting\\workingfile.txt",actionStartPosition,lenght,1);
            curLine_2 = -1;
            curPos_2 = -1;
            save_state_2 = 43;
            return 0;
            break;
        case 'c':
            cut("root\\..setting\\workingfile.txt",actionStartPosition,lenght,1);
            curLine_2 = -1;
            curPos_2 = -1;
            save_state_2 = 43;
            return 0;
            break;
        default:
            break;
        }
    }
}
int normal_mode()
{
    clear();
    move(row_2 - 2, 0);
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    attron(COLOR_PAIR(1));
    printw("  NORMAL  ");
    attroff(COLOR_PAIR(1));
    init_pair(2, COLOR_WHITE, COLOR_CYAN);
    attron(COLOR_PAIR(2));
    printw("  %10s   %c ", fileName_2, save_state_2);
    attroff(COLOR_PAIR(2));
    init_pair(3, COLOR_BLACK, COLOR_MAGENTA);
    chgat(-1, COLOR_PAIR(3), 0, NULL);
    move(0, 0);
    int lineCount = 0;
    char * line = malloc(1000000);
    //line count
    FILE * workingFile = fopen("root\\..setting\\workingfile.txt", "r");
    while (1)
    {
        line[0] = '\0';
        fgets(line, 1000000, workingFile);
        if (line[0] == '\0')
            break;
        lineCount++;
    }
    fclose(workingFile);
    workingFile = fopen("root\\..setting\\workingfile.txt", "r");
    //handle zero line
    if (lineCount == 0)
    {
        init_pair(4, COLOR_WHITE, COLOR_GREEN);
        attron(COLOR_PAIR(4));
        printw(" %5d  ", 1);
        attroff(COLOR_PAIR(4));
    }
    //get to the print line
    int lineNum = 1;
    for (lineNum; lineNum < firstLineNum_2; lineNum++)
        fgets(line, 1000000, workingFile);
    //print lines
    int firstline = lineNum;
    for (lineNum; lineNum < (firstline + row_2 - 2); lineNum++)
    {
        if (lineNum > lineCount)
            break;
        fgets(line, 1000000, workingFile);
        if (line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';
        init_pair(4, COLOR_WHITE, COLOR_GREEN);
        attron(COLOR_PAIR(4));
        printw(" %5d  ", lineNum);
        attroff(COLOR_PAIR(4));
        printw("%s", line);
        if (lineNum < firstline + row_2 - 3 && lineNum < lineCount)
            printw("\n");
    }
    fclose(workingFile);
    refresh();
    //set cursor position
    if (curLine_2 == -1 && curPos_2 == -1)
    {
        getyx(stdscr , curLine_2 , curPos_2);
    }
    else
    {
        workingFile = fopen("root\\..setting\\workingfile.txt", "r");
        for (lineNum = 0; lineNum <= curLine_2 + firstLineNum_2 - 1; lineNum++)
            fgets(line, 1000000, workingFile);
        fclose(workingFile);
        if (lineNum != lineCount && curPos_2 > (strlen(line)-1) + 8)
            curPos_2 = (strlen(line)-1) + 8;
        if (lineNum == lineCount && curPos_2 > strlen(line) + 8)
            curPos_2 = strlen(line) + 8;
    }
    move(curLine_2, curPos_2);
    //handle keys
    while (1)
    {
        int pos[] = {curLine_2 + firstLineNum_2 , curPos_2 - 8};
        int ch = getch();
        switch (ch)
        {
        case KEY_F(1):
            exit(0);
            return 0;
            break;
        case ':':
        case '/':
            return 1;
            break;
        case 'k':
            if (curLine_2 > 0)
            {
                if (curLine_2 < 4 && firstLineNum_2 > 1)
                    firstLineNum_2--;
                else
                    curLine_2--;
            }
            return 0;
            break;
        case 'j':
            if (curLine_2 < lineCount - 1 && curLine_2 < row_2 - 3)
            {
                if (curLine_2 > (row_2 - 3) - 4 && lineCount > (firstLineNum_2 - 1) + (row_2 - 2))
                    (firstLineNum_2)++;
                else
                    (curLine_2)++;
            }
            return 0;
            break;
        case 'l':
                (curPos_2)++;
            return 0;
            break;
        case 'h':
            if (curPos_2 > 8)
                (curPos_2)--;
            return 0;
            break;
        case 'u':
            return 5;
            break;
        case 'v':
            return 2;
            break;
        case 'p':
            paste("root\\..setting\\workingfile.txt",pos);
            save_state_2 = 43;
            return 0;
            break;
        case '=':
            indentation("root\\..setting\\workingfile.txt");
            curLine_2 = -1;
            curPos_2 = -1;
            save_state_2 = 43;
            return 0;
            break;
        default:
            break;
        }
    }
}
int main()
{
    path_2 = malloc(200);
    fileName_2 = malloc(50);
    strcpy(fileName_2,"");
    temp_2 = 0;
    save_state_2 = 32;
    row_2, col_2;
    curLine_2 = -1;
    curPos_2 = -1;
    firstLineNum_2 = 1;
    FILE * workingFile = fopen("root\\..setting\\workingfile.txt", "w");fclose(workingFile);
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, row_2, col_2);
    start_color();
    int pass = 0;
    while (1)
    {
        if (pass == 0)
        {
            pass = normal_mode();
            if (pass == 2)
            {
                Vline_2 = firstLineNum_2 + curLine_2;
                Vpos_2 = curPos_2 - 8;
            }
        }
        if (pass == 1)
        {
            move(row_2-1,0);
            echo();
            printw(":");
            pass = get_command();
            noecho();
        }
        if (pass == 2)
        {
            pass = visual_mode();
        }
        if (pass == 5)
            undo_file();
        
    }
    refresh();
    endwin();
    return 0;
}
// int main()
// {
//     while (1)
//     {
//         // get command
//         char command[1000];
//         gets(command);
//         if (!strcmp(command, ""))
//             continue;
//         // program exit command
//         if (command[0] == '0' && command[1] == '0')
//             return 0;
//         // commandName extraction
//         char *opName = strtok(command, " ");
//         char *other = strtok(NULL, "");
//         // check and pass to the right function
//         if (!strcmp(opName, "createfile"))
//         {
//             char *path = full_path(other);
//             if (create(path) == -1)
//                 FILEEXIST
//         }
//         else if (!strcmp(opName, "insertstr"))
//         {
//             char *path = full_path(other);
//             char *string = extraction(other, "--str ", 0);
//             int *position = position_extraction(other);
//             if (check_existance(path) == -1)
//                 NODIR
//             else if (check_existance(path) == -2)
//                 NOFILE
//             else
//                 undo_backup(path);
//             if (insert(path, string, position) == -5)
//             {
//                 NOPOS
//                 undo_delete_last_record(path);
//             }
//         }
//         else if (!strcmp(opName, "cat"))
//         {
//             char *path = full_path(other);
//             if (check_existance(path) == -1)
//                 NODIR
//             else if (check_existance(path) == -2)
//                 NOFILE
//             else
//                 cat(path);
//         }
//         else if (!strcmp(opName, "removestr"))
//         {
//             char *path = full_path(other);
//             if (check_existance(path) == -1)
//                 NODIR
//             else if (check_existance(path) == -2)
//                 NOFILE
//             else
//             {
//                 undo_backup(path);
//                 int *position = position_extraction(other);
//                 int size = size_extraction(other);
//                 int forward = -1;
//                 if (options(other, " -f") == 1)
//                     forward = 1;
//                 int error = removestr(path, position, size, forward);
//                 if (error == -5)
//                 {
//                     NOPOS
//                     undo_delete_last_record(path);
//                 }
//                 else if (error == -6)
//                 {
//                     NOSIZE
//                     undo_delete_last_record(path);
//                 }
//             }
//         }
//         else if (!strcmp(opName, "copystr"))
//         {
//             char *path = full_path(other);
//             if (check_existance(path) == -1)
//                 NODIR
//             else if (check_existance(path) == -2)
//                 NOFILE
//             else
//             {
//                 undo_backup(path);
//                 int *position = position_extraction(other);
//                 int size = size_extraction(other);
//                 int forward = -1;
//                 if (options(other, " -f") == 1)
//                     forward = 1;
//                 int error = copy(path, position, size, forward);
//                 if (error == -5)
//                 {
//                     NOPOS
//                     undo_delete_last_record(path);
//                 }
//                 else if (error == -6)
//                 {
//                     NOSIZE
//                     undo_delete_last_record(path);
//                 }
//             }
//         }
//         else if (!strcmp(opName, "cutstr"))
//         {
//             char *path = full_path(other);
//             if (check_existance(path) == -1)
//                 NODIR
//             else if (check_existance(path) == -2)
//                 NOFILE
//             else
//             {
//                 undo_backup(path);
//                 int *position = position_extraction(other);
//                 int size = size_extraction(other);
//                 int forward = -1;
//                 if (options(other, " -f") == 1)
//                     forward = 1;
//                 int error = cut(path, position, size, forward);
//                 if (error == -5)
//                 {
//                     NOPOS
//                     undo_delete_last_record(path);
//                 }
//                 else if (error == -6)
//                 {
//                     NOSIZE
//                     undo_delete_last_record(path);
//                 }
//             }
//         }
//         else if (!strcmp(opName, "pastestr"))
//         {
//             char *path = full_path(other);
//             int *position = position_extraction(other);
//             if (check_existance(path) == -1)
//                 NODIR
//             else if (check_existance(path) == -2)
//                 NOFILE
//             else
//                 undo_backup(path);
//             if (paste(path, position) == -5)
//             {
//                 NOPOS
//                 undo_delete_last_record(path);
//             }
//         }
//         //else if (!strcmp(opName, "find"))
//         // {
//         //     char *path = full_path(other);
//         //     if (check_existance(path) == -1)
//         //         NODIR
//         //     else if (check_existance(path) == -2)
//         //         NOFILE
//         //     else
//         //     {
//         //         char *string = extraction(other, "--str ", 1);
//         //         int at = at_option(other);
//         //         int count = options(other, "-count ");
//         //         int byword = options(other, "-byword ");
//         //         int all = options(other, "-all ");
//         //     }
//         // }
//         //else if (!strcmp(opName, "replace"))
//         // {
//         // }
//         else if (!strcmp(opName, "grep"))
//         {
//             char * string = extraction(other, "--str ",0);
//             char * pathes = strstr(other, "--files ")+8;
//             int count_option = 0;
//             count_option = options(other, "-c");
//             int list_option = 0;
//             list_option = options(other, "-l");
//             FILE * grepp = fopen("root\\..setting\\grep.txt", "w");fclose(grepp);
//             FILE * greppl = fopen("root\\..setting\\grepl.txt", "w");fclose(greppl);
//             while (1)
//             {
//                 if (pathes == NULL)
//                     break;
//                 char * path = malloc(200);
//                 if (pathes[0] == '"')
//                     path = strtok(pathes,"\"");
//                 else
//                     path = strtok(pathes," ");
//                 path = path + 1;

//                 while (1)
//                 {
//                     char *place = strstr(path, "/");
//                     if (place == NULL)
//                         break;
//                     *place = '\\';
//                 }
//                 pathes = strtok(NULL, "");
//                 if (check_existance(path) == -1)
//                     NODIR
//                 else if (check_existance(path) == -2)
//                     NOFILE
//                 grep(path, string);
//             }
//             if (count_option == 0 && list_option == 0)
//             {
//                 FILE * grep = fopen("root\\..setting\\grep.txt", "r");
//                 while (1)
//                 {
//                     char c = fgetc(grep);
//                     if (c == EOF)
//                         break;
//                     printf("%c",c);
//                 }
//                 fclose(grep);
//             }else if (count_option == 1)
//             {
//                 FILE * grep = fopen("root\\..setting\\grep.txt", "r");
//                 int count = 0;
//                 while (1)
//                 {
//                     char c = fgetc(grep);
//                     if (c == EOF)
//                         break;
//                     if (c == '\n')
//                     {
//                         count++;
//                     }

//                 }
//                 fclose(grep);
//                 printf("%d\n",count);
//             }else if (list_option == 1)
//             {
//                 FILE * grepl = fopen("root\\..setting\\grepl.txt", "r");
//                 while (1)
//                 {
//                     char c = fgetc(grepl);
//                     if (c == EOF)
//                         break;
//                     printf("%c",c);
//                 }
//                 fclose(grepl);

//             }
//             remove("root\\..setting\\grep.txt");
//             remove("root\\..setting\\grepl.txt");
//         }
//         else if (!strcmp(opName, "undo"))
//         {
//             char *path = full_path(other);
//             if (check_existance(path) == -1)
//                 NODIR
//             else if (check_existance(path) == -2)
//                 NOFILE
//             else
//                 undo(path);
//         }
//         else if (!strcmp(opName, "auto-indent"))
//         {
//             char *path = full_path(other);
//             if (check_existance(path) == -1)
//                 NODIR
//             else if (check_existance(path) == -2)
//                 NOFILE
//             else
//             {
//                 undo_backup(path);
//                 indentation(path);
//             }
//         }
//         else if (!strcmp(opName, "compare"))
//         {
//             char *path1 = extraction(other, "--file1 ", 0) + 1;
//             while (1)
//             {
//                 char *place = strstr(path1, "/");
//                 if (place == NULL)
//                     break;
//                 *place = '\\';
//             }
//             char *path2 = extraction(other, "--file2 ", 0) + 1;
//             while (1)
//             {
//                 char *place = strstr(path2, "/");
//                 if (place == NULL)
//                     break;
//                 *place = '\\';
//             }
//             compare(path1, path2);
//         }
//         else if (!strcmp(opName, "tree"))
//         {
//             directory_tree(atoi(other));
//         }
//         else
//             printf("invalid command\n");
//     }
// }
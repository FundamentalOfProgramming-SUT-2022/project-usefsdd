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
#include <stdbool.h>
#include <dirent.h>
// defining errors
#define FILEEXIST printf("ERROR: THIS FILE IS ALREADY EXIST...\n");
#define NOPOS printf("ERROR: INCORRECT POSITION...\n");
#define NOFILE printf("ERROR: FILE NOT EXIST...\n");
#define NODIR printf("ERROR: WRONG PATH...\n");
#define NOSIZE printf("ERROR: WRONG SIZE...\n");
#define NOACC printf("ERROR: NOT ALLOWED TO TOUCH THIS PATH...\n");
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
    strcpy(path, extraction(command, "--file ", 0) + 1);
    while (1)
    {
        char *place = strstr(path, "/");
        if (place == NULL)
            break;
        *place = '\\';
    }
    return path;
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
    if (strstr(path, "..setting") != NULL || strstr(path, "..undoroot") != NULL)
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
int find(char *path, char *string, int pos)
{
    char pattern[50];
    strcpy(pattern, string);
    int index = 0;
    int lenght = 0;
    int position = pos;
    FILE *file = fopen(path, "r");
    for (int i = 0; i < pos; i++)
        fgetc(file);
    while (1)
    {
        char c = fgetc(file);
        if (c == EOF)
            return -1;
        if (c == pattern[index])
        {
            index++;
            lenght++;
        }
        else
        {
            position++;
            if (c == '\n' && lenght != 0)
                fseek(file, -1 * (lenght + 1), SEEK_CUR);
            else
                fseek(file, -1 * lenght, SEEK_CUR);
            index = 0;
            lenght = 0;
        }
        if (index == strlen(pattern))
        {
            return position;
        }
    }
    fclose(file);
}
int replace(char *path, char *str1, char *str2, int at, int all)
{
    if (all == 1)
    {
        while (1)
        {
            int position = 0;
            position = find(path, str1, 0);
            FILE *file = fopen(path, "r");
            FILE *rep = fopen("root\\..setting\\replace.txt", "w");
            for (int i = 0; i < position; i++)
            {
                char b = fgetc(file);
                fputc(b, rep);
            }
            fprintf(rep,"%s", str2);
            for (int i = 0; i < strlen(str1); i++)
                fgetc(file);
            while (1)
            {
                char b = fgetc(file);
                if (b == EOF)
                    break;
                fputc(b, rep);
            }
            fclose(rep);
            fclose(file);
            file = fopen(path, "w");
            rep = fopen("root\\..setting\\replace.txt", "r");
            while (1)
            {
                char b = fgetc(rep);
                if (b == EOF)
                    break;
                fputc(b, file);
            }
            fclose(rep);
            fclose(file);
            remove("root\\..setting\\replace.txt");
        }
    }
    else
    {

        int position = 0;
        for (int i = 0; i < at; i++)
        {
            position = find(path, str1, position);
            position += strlen(str1);
        }
        FILE *file = fopen(path, "r");
        FILE *rep = fopen("root\\..setting\\replace.txt", "w");
        for (int i = 0; i < position; i++)
        {
            char b = fgetc(file);
            fputc(b, rep);
        }
        fprintf(rep, "%s", str2);
        for (int i = 0; i < strlen(str1); i++)
            fgetc(file);
        while (1)
        {
            char b = fgetc(file);
            if (b == EOF)
                break;
            fputc(b, rep);
        }
        fclose(rep);
        fclose(file);
        file = fopen(path, "w");
        rep = fopen("root\\..setting\\replace.txt", "r");
        while (1)
        {
            char b = fgetc(rep);
            if (b == EOF)
                break;
            fputc(b, file);
        }
        fclose(rep);
        fclose(file);
        remove("root\\..setting\\replace.txt");
    }
}
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
    FILE *file = fopen(path, "r");
    FILE *indentFile = fopen("root\\..setting\\indentfile.txt", "w");
    int indent = 0;    // indent of each line
    int cursorPos = 0; // last writed pos
    int nowPos = 0;
    int oneSpace = 0;  //(bool)space between line & {
    int lineStart = 0; //(bool)check if the line started
    char c;
    while (1)
    {
        c = fgetc(file);
        nowPos++;
        // find the position of last char before { or }
        if (c == '{' || c == '}')
        {
            char x;
            while (1)
            {
                fseek(file, -2, SEEK_CUR);
                x = fgetc(file);
                nowPos--;
                if (x == '\n')
                    fseek(file, -1, SEEK_CUR);
                if (x != ' ' && x != '\n')
                {
                    if (x == '{' || x == '}' || x == EOF)
                        oneSpace = 0;
                    else
                        oneSpace = 1;
                    break;
                }
            }
            // get to the last writed char
            fseek(file, 0, SEEK_SET);
            for (int i = 0; i < cursorPos; i++)
                fgetc(file);
            // write up to the nowpos
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
                    fprintf(indentFile, " {\n");
                else if (oneSpace == 0)
                    fprintf(indentFile, "{\n");
                for (int i = 0; i < 4 * indent; i++)
                    fputc(' ', indentFile);
            }
            if (c == '}')
            {
                indent--;
                fputc('\n', indentFile);
                for (int i = 0; i < 4 * indent; i++)
                    fputc(' ', indentFile);
                fprintf(indentFile, "}\n");
                for (int i = 0; i < 4 * indent; i++)
                    fputc(' ', indentFile);
            }
            // get back to {} (ahead)
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
        // end of the program
        if (c == EOF)
        {
            break;
        }
    }
    fclose(file);
    fclose(indentFile);
    // print corrected text in the file
    file = fopen(path, "w");
    indentFile = fopen("root\\..setting\\indentfile.txt", "r");
    char *line = malloc(1000000);
    while (1)
    {
        line[0] = '\0';
        fgets(line, 1000000, indentFile);
        if (line[0] == '\0')
            break;
        int flagPrintLine = 0; // check lines and not print free lines
        for (int i = 0; i < strlen(line); i++)
        {
            if (line[i] != ' ' && line[i] != '\n' && line[i] != '\0')
            {
                flagPrintLine = 1;
                break;
            }
        }
        if (flagPrintLine == 1)
            fprintf(file, "%s", line);
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
    if (depth == 0)
    {
        printf("root");
    }
    
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
int main()
{
    while (1)
    {
        // get command
        char command[1000];
        gets(command);
        if (!strcmp(command, ""))
            continue;
        // program exit command
        if (command[0] == '0' && command[1] == '0')
            return 0;
        // commandName extraction
        char *opName = strtok(command, " ");
        char *other = strtok(NULL, "");
        // check and pass to the right function
        if (!strcmp(opName, "createfile"))
        {
            char *path = full_path(other);
            if (create(path) == -1)
                FILEEXIST
        }
        else if (!strcmp(opName, "insertstr"))
        {
            char *path = full_path(other);
            char *string = extraction(other, "--str ", 0);
            int *position = position_extraction(other);
            if (check_existance(path) == -1)
                NODIR
            else if (check_existance(path) == -2)
                NOFILE
            else if (check_existance(path) == -3)
                NOACC
            else
                undo_backup(path);
            if (insert(path, string, position) == -5)
            {
                NOPOS
                undo_delete_last_record(path);
            }
        }
        else if (!strcmp(opName, "cat"))
        {
            char *path = full_path(other);
            if (check_existance(path) == -1)
                NODIR
            else if (check_existance(path) == -2)
                NOFILE
            else if (check_existance(path) == -3)
                NOACC
            else
                cat(path);
        }
        else if (!strcmp(opName, "removestr"))
        {
            char *path = full_path(other);
            if (check_existance(path) == -1)
                NODIR
            else if (check_existance(path) == -2)
                NOFILE
            else if (check_existance(path) == -3)
                NOACC
            else
            {
                undo_backup(path);
                int *position = position_extraction(other);
                int size = size_extraction(other);
                int forward = -1;
                if (options(other, " -f") == 1)
                    forward = 1;
                int error = removestr(path, position, size, forward);
                if (error == -5)
                {
                    NOPOS
                    undo_delete_last_record(path);
                }
                else if (error == -6)
                {
                    NOSIZE
                    undo_delete_last_record(path);
                }
            }
        }
        else if (!strcmp(opName, "copystr"))
        {
            char *path = full_path(other);
            if (check_existance(path) == -1)
                NODIR
            else if (check_existance(path) == -2)
                NOFILE
            else if (check_existance(path) == -3)
                NOACC
            else
            {
                undo_backup(path);
                int *position = position_extraction(other);
                int size = size_extraction(other);
                int forward = -1;
                if (options(other, " -f") == 1)
                    forward = 1;
                int error = copy(path, position, size, forward);
                if (error == -5)
                {
                    NOPOS
                    undo_delete_last_record(path);
                }
                else if (error == -6)
                {
                    NOSIZE
                    undo_delete_last_record(path);
                }
            }
        }
        else if (!strcmp(opName, "cutstr"))
        {
            char *path = full_path(other);
            if (check_existance(path) == -1)
                NODIR
            else if (check_existance(path) == -2)
                NOFILE
            else if (check_existance(path) == -3)
                NOACC
            else
            {
                undo_backup(path);
                int *position = position_extraction(other);
                int size = size_extraction(other);
                int forward = -1;
                if (options(other, " -f") == 1)
                    forward = 1;
                int error = cut(path, position, size, forward);
                if (error == -5)
                {
                    NOPOS
                    undo_delete_last_record(path);
                }
                else if (error == -6)
                {
                    NOSIZE
                    undo_delete_last_record(path);
                }
            }
        }
        else if (!strcmp(opName, "pastestr"))
        {
            char *path = full_path(other);
            int *position = position_extraction(other);
            if (check_existance(path) == -1)
                NODIR
            else if (check_existance(path) == -2)
                NOFILE
            else if (check_existance(path) == -3)
                NOACC
            else
                undo_backup(path);
            if (paste(path, position) == -5)
            {
                NOPOS
                undo_delete_last_record(path);
            }
        }
        else if (!strcmp(opName, "find"))
        {
            char *path = full_path(other);
            char *string = extraction(other, "--str ", 1);
            int at = at_option(other);
            int count = options(other, "-count");
            int byword = options(other, "-byword");
            int all = options(other, "-all");
            if (check_existance(path) == -1)
                NODIR
            else if (check_existance(path) == -2)
                NOFILE
            else
            {
                int position = 0;
                int counter = 0;
                if (byword == 0 && count == 0 && all == 0)
                {
                    for (int i = 0; i < at; i++)
                    {
                        if (i != 0)
                            position += strlen(string);
                        position = find(path, string, position);
                    }
                    printf("%d\n", position);
                }
                if (byword == 0 && count != 0 && all == 0)
                {
                    while (1)
                    {
                        position = find(path, string, position);
                        if (position == -1)
                        {
                            printf("%d\n", counter);
                            break;
                        }
                        else
                            counter++;
                        position += strlen(string);
                    }
                }
                if (byword == 0 && count == 0 && all != 0)
                {
                    while (1)
                    {
                        if (counter != 0)
                            position += strlen(string);
                        position = find(path, string, position);
                        
                        if (position != -1)
                        {
                            if (counter != 0)
                                printf(" , ");
                            printf("%d", position);
                            counter++;
                        }
                        else
                        {
                            printf("\n");
                            break;
                        }
                    }
                }
                // byword
                int wordNum = 0;
                if (byword == 1 && count == 0 && all == 0)
                {
                    for (int i = 0; i < at; i++)
                    {
                        if (i != 0)
                            position += strlen(string);
                        position = find(path, string, position);

                    }
                    FILE *file = fopen(path, "r");
                    for (int i = 0; i < position; i++)
                    {
                        char x = fgetc(file);
                        if (x == ' ' || x == '\n')
                            wordNum++;
                    }
                    fclose(file);
                    printf("%d\n", wordNum);
                }
                if (byword == 1 && count == 0 && all != 0)
                {
                    while (1)
                    {
                        wordNum = 0;
                        position = find(path, string, position);
                        if (counter != 0)
                            position += strlen(string);
                        if (position != -1)
                        {
                            FILE *file = fopen(path, "r");
                            for (int i = 0; i < position; i++)
                            {
                                char x = fgetc(file);
                                if (x == ' ' || x == '\n')
                                    wordNum++;
                            }
                            fclose(file);
                            if (counter != 0)
                                printf(" , ");
                            printf("%d", wordNum);
                            counter++;
                        }
                        else
                        {
                            printf("\n");
                            break;
                        }
                    }
                }
            }
        }
        else if (!strcmp(opName, "replace"))
        {
            char *path = full_path(other);
            char *str1 = extraction(other, "--str1 ", 1);
            char *str2 = extraction(other, "--str2 ", 1);
            int at = at_option(other);
            int all = options(other, "-all");
            if (check_existance(path) == -1)
                NODIR
            else if (check_existance(path) == -2)
                NOFILE
            else
            {
                replace(path,str1,str2,at,all);
            }
        }
        else if (!strcmp(opName, "grep"))
        {
            int error = 0;
            char *string = extraction(other, "--str ", 0);
            char *pathes = strstr(other, "--files ") + 8;
            int count_option = 0;
            count_option = options(other, "-c");
            int list_option = 0;
            list_option = options(other, "-l");
            FILE *grepp = fopen("root\\..setting\\grep.txt", "w");
            fclose(grepp);
            FILE *greppl = fopen("root\\..setting\\grepl.txt", "w");
            fclose(greppl);
            while (1)
            {
                if (pathes == NULL)
                    break;
                char *path = malloc(200);
                if (pathes[0] == '"')
                    path = strtok(pathes, "\"");
                else
                    path = strtok(pathes, " ");
                path = path + 1;

                while (1)
                {
                    char *place = strstr(path, "/");
                    if (place == NULL)
                        break;
                    *place = '\\';
                }
                pathes = strtok(NULL, "");
                if (check_existance(path) == -1)
                {
                    error = 1;
                    NODIR
                }
                else if (check_existance(path) == -2)
                {
                    error = 1;
                    NOFILE
                }
                else if (check_existance(path) == -3)
                {
                    error = 1;
                    NOACC
                }
                else
                    grep(path, string);
            }
            if (error == 0)
            {
                if (count_option == 0 && list_option == 0)
                {
                    FILE *grep = fopen("root\\..setting\\grep.txt", "r");
                    while (1)
                    {
                        char c = fgetc(grep);
                        if (c == EOF)
                            break;
                        printf("%c", c);
                    }
                    fclose(grep);
                }
                else if (count_option == 1)
                {
                    FILE *grep = fopen("root\\..setting\\grep.txt", "r");
                    int count = 0;
                    while (1)
                    {
                        char c = fgetc(grep);
                        if (c == EOF)
                            break;
                        if (c == '\n')
                        {
                            count++;
                        }
                    }
                    fclose(grep);
                    printf("%d\n", count);
                }
                else if (list_option == 1)
                {
                    FILE *grepl = fopen("root\\..setting\\grepl.txt", "r");
                    while (1)
                    {
                        char c = fgetc(grepl);
                        if (c == EOF)
                            break;
                        printf("%c", c);
                    }
                    fclose(grepl);
                }
            }
            remove("root\\..setting\\grep.txt");
            remove("root\\..setting\\grepl.txt");
        }
        else if (!strcmp(opName, "undo"))
        {
            char *path = full_path(other);
            if (check_existance(path) == -1)
                NODIR
            else if (check_existance(path) == -2)
                NOFILE
            else if (check_existance(path) == -3)
                NOACC
            else
                undo(path);
        }
        else if (!strcmp(opName, "auto-indent"))
        {
            char *path = full_path(other);
            if (check_existance(path) == -1)
                NODIR
            else if (check_existance(path) == -2)
                NOFILE
            else if (check_existance(path) == -3)
                NOACC
            else
            {
                undo_backup(path);
                indentation(path);
            }
        }
        else if (!strcmp(opName, "compare"))
        {
            char *path1 = extraction(other, "--file1 ", 0) + 1;
            while (1)
            {
                char *place = strstr(path1, "/");
                if (place == NULL)
                    break;
                *place = '\\';
            }
            char *path2 = extraction(other, "--file2 ", 0) + 1;
            while (1)
            {
                char *place = strstr(path2, "/");
                if (place == NULL)
                    break;
                *place = '\\';
            }
            if (check_existance(path1) == -1 || check_existance(path2) == -1)
                NODIR
            else if (check_existance(path1) == -2 || check_existance(path1) == -2)
                NOFILE
            else if (check_existance(path1) == -3 || check_existance(path1) == -3)
                NOACC
            else
                compare(path1, path2);
        }
        else if (!strcmp(opName, "tree"))
        {
            directory_tree(atoi(other));
        }
        else
            printf("invalid command\n");
    }
}
//created by usef sadidi
//Email: usefsadidi@gmail.com
//Telegram: @usefsdd
//january 2023
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
//defining errors
#define FILEEXIST printf("ERROR: THIS FILE IS ALREADY EXIST...\n");
#define NOPOS printf("ERROR: INCORRECT POSITION...\n");
#define NOFILE printf("ERROR: FILE NOT EXIST...\n");
#define NODIR printf("ERROR: WRONG PATH...\n");
#define NOSIZE printf("ERROR: WRONG SIZE...\n");
//a function for concating strings
char * concat(int count, ...)
{
    va_list ap;
    int i;
    int len = 1;
    va_start(ap, count);
    for(i=0 ; i<count ; i++)
        len += strlen(va_arg(ap, char*));
    va_end(ap);
    char *merged = calloc(sizeof(char),len);
    int null_pos = 0;
    va_start(ap, count);
    for(i=0 ; i<count ; i++)
    {
        char *s = va_arg(ap, char*);
        strcpy(merged+null_pos, s);
        null_pos += strlen(s);
    }
    va_end(ap);
    return merged;
}
//file path & string extraction
char * extraction(char * command, char * control,int find) 
{
    int lenght = strlen(control);
    char * out = (char*)malloc(900 * sizeof(char));
    char * ptr = strstr(command, control);
    if (*(ptr + lenght) == '"')
    {
        char * string_index = ptr + lenght + 1;
        int index = 0;
        while (1)
        {
            if (*(string_index) == '\"' && *(string_index-1) != '\\')
            {
                out[index] = '\0';
                break;
            }
            if (*(string_index) == '\\')
            {
                if (*(string_index+1) == '\\' && *(string_index+2) == 'n')
                {
                    out[index] = '\\';
                    out[index+1] = 'n';
                    index+=2;
                    string_index+=3;
                }else if(*(string_index+1) == 'n')
                {
                    out[index] = '\n';
                    index++;
                    string_index+=2;
                }
                else if(*(string_index+1) == '\\' || *(string_index+1) == '\"')
                {
                    out[index] = *(string_index+1);
                    index++;
                    string_index+=2;
                }else if (*(string_index+1) == '*' && find == 0)
                {
                    out[index] = *(string_index+1);
                    index++;
                    string_index+=2;
                }
            }else
            {
                out[index] = *string_index;
                index++;
                string_index++;
            }
        }
    }else
    {
        char * string_index = ptr + lenght;
        int index = 0;
        while (1)
        {
            if (*(string_index) == ' ' || *(string_index) == '\0')
            {
                out[index] = '\0';
                break;
            }
            out[index] = *string_index;
            index++;
            string_index++;
        }
    }
    return out;
}
//convert the path to windows standard form
char * full_path(char * command)
{
    char * path = (char *)malloc(200 * sizeof(char));
    strcpy(path,extraction(command, "--file ",0)+1);
    while (1)
    {
        char * place = strstr(path,"/");
        if (place == NULL)
            break; 
        *place = '\\';
    }
    return path;
}
//string position extraction
int * position_extraction(char * command)
{
    char * s = (char*)malloc(1000*sizeof(char));
    strcpy(s , command);
    char * position = (char*)malloc(50 * sizeof(char));
    char * pos = strstr(s, "--pos ");
    if(pos == NULL)
        return NULL;
    position = strtok(pos+6," ");
    char *lineNum = strtok(position, ":");
    char *startPos = strtok(NULL, "");
    int * pospos = (int*)malloc(2 *sizeof(int));
    pospos[0] = atoi(lineNum);
    pospos[1] = atoi(startPos);
    return pospos;
}
//string size extraction
int size_extraction(char * command)
{
    char * s = (char*)malloc(1000*sizeof(char));
    strcpy(s , command);
    char * size = (char*)malloc(10 * sizeof(char));
    char * siz = strstr(s, "-size ");
    if(siz == NULL)
        return -1;
    size = strtok(siz+6," ");
    return atoi(size);
}
// get options
int options(char * command , char *optionName)
{

    char * s = (char*)malloc(1000*sizeof(char));
    strcpy(s , command);
    char * option = strstr(s, optionName);
    if(option == NULL)
        return 0;
    return 1;
}
//at option
int at_option(char * command)
{
    char * s = (char*)malloc(1000*sizeof(char));
    strcpy(s , command);
    char * at = strstr(s, "-at ");
    if(at == NULL)
        return 1;
    char *num = strtok(at+4," ");
    return atoi(num);
}
//check existance of a file or directory
int check_existance(char * path)
{
    char dir[200];
    int end = strlen(path) - 1;
    for (int i = end ; i >= 0; i--)
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
        dir[j+1] = '\0';
    }
    if (access (dir, F_OK) != 0)
        return -1;
    
    if (access (path, F_OK) != 0)
        return -2;
    return 0;
}
//command's function
int create(char * path)
{
    char * address = (char*)malloc(200 * sizeof(char));
    address = strtok(path,"/");
    char * dir = (char*)malloc(20 * sizeof(char));
    dir = strtok(NULL,"/");
    char * dir2 = (char*)malloc(20 * sizeof(char));
    dir2 = strtok(NULL,"/");
    while(dir2 != NULL)
    {
        address = concat(3,address, "\\", dir);
        _mkdir(address);
        strcpy(dir,dir2);
        dir2 = strtok(NULL, "/");
    }
    char * file = concat(3,address, "\\", dir);
    if (access (file, F_OK) == 0)
        return -1;
    else
    {
        FILE * fptr;
        fptr = fopen(file, "w");
        fclose(fptr);
    }
}
int insert(char * path, char * string , int * position)
{
    int line = 1;
    int posInLine = 0;
    FILE * file = fopen(path,"r");
    FILE * insert = fopen("root\\..setting\\insert.txt","w");
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
                return-5;
            }
            line++;
            posInLine = 0;
        }else
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
    file = fopen(path,"w");
    insert = fopen("root\\..setting\\insert.txt","r");
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
int cat(char* path)
{
    FILE * file = fopen(path, "r");
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
int removestr(char * path, int * position, int size, int backfor)
{
    int removepos = 0;
    int posall = 0;
    int line = 1;
    int posInLine = 0;
    FILE * file = fopen(path,"r");
    FILE * removee;
    if (backfor == 1)
        removee = fopen("root\\..setting\\removee.txt","w");
    while (1)
    {
        if ((line == position[0] && posInLine == position[1]) || posall ==position[2] )
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
                return- 5;
            }
            line++;
            posInLine = 0;
        }else
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
    }else
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
        file = fopen(path,"w");
        removee = fopen("root\\..setting\\removee.txt","r");
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
        int pos[3] = {-1,-1,removepos - size};
        removestr(path,pos,size,1);
    }
}
int copy(char * path, int * position, int size, int backfor)
{
    int copypos = 0;
    int posall = 0;
    int line = 1;
    int posInLine = 0;
    FILE * file = fopen(path,"r");
    while (1)
    {
        if ((line == position[0] && posInLine == position[1]) || posall ==position[2] )
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
                return- 5;
            }
            line++;
            posInLine = 0;
        }else
        {
            posInLine++;
        }
        posall++;
    }
    if (backfor == 1)
    {
        FILE * clipboard = fopen("root\\..setting\\clipboard.txt","w");
        for (int i = 0; i < size; i++)
        {
            char b = fgetc(file);
            if (b == EOF)
            {
                fclose(clipboard);
                fclose(file);
                return -6;
            }
            fputc(b,clipboard);
        }
        fclose(file);
        fclose(clipboard);
    }else
    {
        if (size > posall)
            return -6;
        copypos = posall;
    }
    if (backfor == -1)
    {
        int pos[3] = {-1,-1,copypos - size};
        copy(path,pos,size,1);
    }
}
int cut(char * path, int * position, int size, int backfor)
{
    int cutpos = 0;
    int posall = 0;
    int line = 1;
    int posInLine = 0;
    FILE * file = fopen(path,"r");
    FILE * removee;
    if (backfor == 1)
        removee = fopen("root\\..setting\\removee.txt","w");
    while (1)
    {
        if ((line == position[0] && posInLine == position[1]) || posall ==position[2] )
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
                return- 5;
            }
            line++;
            posInLine = 0;
        }else
        {
            posInLine++;
        }
        posall++;
    }
    if (backfor == 1)
    {
        FILE * clipboard =fopen("root\\..setting\\clipboard.txt","w");
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
    }else
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
        file = fopen(path,"w");
        removee = fopen("root\\..setting\\removee.txt","r");
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
        int pos[3] = {-1,-1,cutpos - size};
        cut(path,pos,size,1);
    }
}
int paste(char * path, int * position)
{
    int line = 1;
    int posInLine = 0;
    FILE * file = fopen(path,"r");
    FILE * insert = fopen("root\\..setting\\insert.txt","w");
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
                return-5;
            }
            line++;
            posInLine = 0;
        }else
        {
            posInLine++;
        }
    }
    FILE * clipboard = fopen("root\\..setting\\clipboard.txt","r");
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
    file = fopen(path,"w");
    insert = fopen("root\\..setting\\insert.txt","r");
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
int find(char * path, char * position, int at, int count, int byword, int all)
{
    
}
int replace(char * command)
{

}
int grep(char * command)
{
    
}
int undo(char * command)
{

}
int indentation(char * command)
{
    
}
int compare(char * command)
{
    
}
int directory_tree(char * command)
{

}
int pipe()
{

}
int main()
{
    while(1)
    {
        //get command
        char command[1000];
        gets(command);
        if (!strcmp(command,""))
            continue;
        //program exit command
        if (command[0] == '0' && command[1] == '0')  return 0;
        //commandName extraction
        char * opName = strtok(command," ");
        char * other = strtok(NULL,"");
        // check and pass to the right function   
        if (!strcmp(opName, "createfile"))
        {
            char * path = extraction(other, "--file ",0);
            if(create(path) == -1)
                FILEEXIST
        }
        else if (!strcmp(opName, "insertstr"))
        {
            char * path = full_path(other);
            char * string = extraction(other, "--str ",0);
            int * position = position_extraction(other);
            if(check_existance(path) == -1)
                NODIR
            else if(check_existance(path) == -2)
                NOFILE
            else if (insert(path, string, position) == -5)
                NOPOS
        }
        else if(!strcmp(opName, "cat"))
        {
            char * path = full_path(other);
            if(check_existance(path) == -1)
                NODIR
            else if(check_existance(path) == -2)
                NOFILE
            else
             cat(path);
        }
        else if(!strcmp(opName, "removestr"))
        {
            char * path = full_path(other);
            if(check_existance(path) == -1)
                NODIR
            else if(check_existance(path) == -2)
                NOFILE
            else
            {
                int * position = position_extraction(other);
                int size = size_extraction(other);
                int forward = -1;
                if (options(other, " -f") == 1)
                    forward = 1;
                int error = removestr(path, position, size, forward);
                if(error == -5)
                    NOPOS
                else if(error == -6)
                    NOSIZE
            }
        }
        else if (!strcmp(opName, "copystr"))
        {
            char * path = full_path(other);
            if(check_existance(path) == -1)
                NODIR
            else if(check_existance(path) == -2)
                NOFILE
            else
            {
                int * position = position_extraction(other);
                int size = size_extraction(other);
                int forward = -1;
                if (options(other, " -f") == 1)
                    forward = 1;
                int error = copy(path, position, size, forward);
                if(error == -5)
                    NOPOS
                else if(error == -6)
                    NOSIZE
            }
        }
        else if(!strcmp(opName, "cutstr"))
        {
            char * path = full_path(other);
            if(check_existance(path) == -1)
                NODIR
            else if(check_existance(path) == -2)
                NOFILE
            else
            {
                int * position = position_extraction(other);
                int size = size_extraction(other);
                int forward = -1;
                if (options(other, " -f") == 1)
                    forward = 1;
                int error = cut(path, position, size, forward);
                if(error == -5)
                    NOPOS
                else if(error == -6)
                    NOSIZE
            }
        }
        else if (!strcmp(opName, "pastestr"))
        {
            char * path = full_path(other);
            int * position = position_extraction(other);
            if(check_existance(path) == -1)
                NODIR
            else if(check_existance(path) == -2)
                NOFILE
            else if (paste(path, position) == -5)
                NOPOS
        }
        else if (!strcmp(opName, "find"))
        {
            char * path = full_path(other);
            if(check_existance(path) == -1)
                NODIR
            else if(check_existance(path) == -2)
                NOFILE
            else
            {
                char * string = extraction(other, "--str ",1);
                int at = at_option(other);
                int count = options(other, "-count ");
                int byword = options(other, "-byword ");
                int all = options(other, "-all ");
            }
        }
        else if (!strcmp(opName, "replace"))
            replace(other);
        else if (!strcmp(opName, "grep"))
            grep(other);        
        else if(!strcmp(opName, "undo"))
            undo(other);
        else if(!strcmp(opName, "auto-indent"))
            indentation(other);
        else if (!strcmp(opName, "compare"))
            compare(other);
        else if (!strcmp(opName, "tree"))
            directory_tree(other);
        else
           printf("invalid command\n");
        
    }
}
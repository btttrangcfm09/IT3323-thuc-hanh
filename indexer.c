#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
 
#define LIM_A 10000
#define LIM_B 1000
#define LIM_C 10000
#define LIM_D 3000
 
typedef struct {
    char key[LIM_B];
    char pos[LIM_D];
    int count;
} Node;
 
Node table[LIM_A];
int tsize = 0;
 
char banlist[LIM_A][LIM_B];
int bansize = 0;
 
void lowerize(char *s) {
    for (int i = 0; s[i]; i++) s[i] = tolower(s[i]);
}
 
int checkBan(const char *x) {
    for (int i = 0; i < bansize; i++)
        if (strcmp(x, banlist[i]) == 0) return 1;
    return 0;
}
 
int checkProper(const char *w, char prev) {
    return (isupper(w[0]) && prev != '.');
}
 
int lookup(const char *w) {
    for (int i = 0; i < tsize; i++)
        if (strcmp(table[i].key, w) == 0) return i;
    return -1;
}
 
void insertKey(const char *w, int line) {
    int p = lookup(w);
    char sline[10];
    snprintf(sline, sizeof(sline), "%d", line);
 
    if (p < 0) {
        strncpy(table[tsize].key, w, LIM_B - 1);
        strncpy(table[tsize].pos, sline, LIM_D - 1);
        table[tsize].count = 1;
        tsize++;
    } else {
        if (strlen(table[p].pos) + strlen(sline) + 2 < LIM_D) {
            strcat(table[p].pos, ", ");
            strcat(table[p].pos, sline);
            table[p].count++;
        }
    }
}
 
int sorter(const void *x, const void *y) {
    return strcmp(((Node*)x)->key, ((Node*)y)->key);
}
 
int main(int argc, char *argv[]) {
    // Kiểm tra tham số dòng lệnh
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        printf("Example: %s alice30.txt result.txt\n", argv[0]);
        return 1;
    }

    char *inputFile = argv[1];
    char *outputFile = argv[2];

    FILE *fs = fopen("stopw.txt", "r");
    FILE *fv = fopen(inputFile, "r");

    if (!fs) {
        printf("Error: cannot open stopw.txt\n");
        return 1;
    }
    
    if (!fv) {
        printf("Error: cannot open input file '%s'\n", inputFile);
        fclose(fs);
        return 1;
    }    while (fscanf(fs, "%999s", banlist[bansize]) != EOF) {
        lowerize(banlist[bansize]);
        bansize++;
    }
    fclose(fs);
 
    char buffer[LIM_C];
    int curLine = 0;
 
    while (fgets(buffer, sizeof(buffer), fv)) {
        curLine++;
        int L = strlen(buffer);
        char token[LIM_B];
        int ti = 0;
 
        for (int i = 0; i <= L; i++) {
            if (isalpha(buffer[i])) {
                if (ti < LIM_B - 1) token[ti++] = buffer[i];
            } else {
                if (ti > 0) {
                    token[ti] = '\0';
                    char prev = (i > 0 ? buffer[i - 1] : ' ');
 
                    char t2[LIM_B];
                    strncpy(t2, token, LIM_B);
                    lowerize(t2);
 
                    if (!checkProper(token, prev) && !checkBan(t2)) {
                        insertKey(t2, curLine);
                    }
                    ti = 0;
                }
            }
        }
    }
 
    fclose(fv);
 
    qsort(table, tsize, sizeof(Node), sorter);

    FILE *fo = fopen(outputFile, "w");
    if (!fo) {
        printf("Error: cannot create output file '%s'\n", outputFile);
        return 1;
    }

    for (int i = 0; i < tsize; i++) {
        fprintf(fo, "%-12s %d, %s\n", table[i].key, table[i].count, table[i].pos);
    }

    fclose(fo);
    
    printf("Successfully processed %d words from '%s'\n", tsize, inputFile);
    printf("Results written to '%s'\n", outputFile);

    return 0;
}
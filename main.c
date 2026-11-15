#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TABLE_SIZE 101

typedef struct WordNode {
    char *word;
    int freq;
    struct WordNode *next;
} WordNode;

typedef struct {
    WordNode *buckets[TABLE_SIZE];
    int total;
    int unique;
} Index;

unsigned long hash(const char *str) {
    unsigned long h = 5381;
    int c;
    while ((c = *str++)) h = ((h << 5) + h) + c;
    return h % TABLE_SIZE;
}

Index* init_index() {
    Index *idx = malloc(sizeof(Index));
    if (!idx) exit(1);
    idx->total = 0;
    idx->unique = 0;
    for (int i = 0; i < TABLE_SIZE; i++) idx->buckets[i] = NULL;
    return idx;
}

void add_word(Index *idx, const char *raw) {
    if (strlen(raw) < 2) return;

    char word[64];
    int i = 0;
    while (raw[i] != '\0' && i < 63) {
        word[i] = tolower((unsigned char)raw[i]);
        i++;
    }
    word[i] = '\0';

    unsigned long h = hash(word);
    WordNode *curr = idx->buckets[h];

    while (curr) {
        if (strcmp(curr->word, word) == 0) {
            curr->freq++;
            idx->total++;
            return;
        }
        curr = curr->next;
    }

    WordNode *node = malloc(sizeof(WordNode));
    if (!node) exit(1);
    node->word = strdup(word);
    node->freq = 1;
    node->next = idx->buckets[h];
    idx->buckets[h] = node;

    idx->total++;
    idx->unique++;
}

void parse_file(const char *filename, Index *idx) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return;
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, f) != -1) {
        char *p = line;
        while (*p != '\0') {
            while (*p != '\0' && !isalpha((unsigned char)*p)) p++;
            if (*p == '\0') break;

            char *start = p;
            while (*p != '\0' && isalpha((unsigned char)*p)) p++;

            char temp = *p;
            *p = '\0';
            add_word(idx, start);
            *p = temp;
        }
    }

    free(line);
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.txt\n", argv[0]);
        return 1;
    }

    Index *idx = init_index();
    parse_file(argv[1], idx);

    printf("Total words: %d\n", idx->total);
    printf("Unique words: %d\n", idx->unique);

    for (int i = 0; i < TABLE_SIZE; i++) {
        WordNode *curr = idx->buckets[i];
        while (curr) {
            WordNode *tmp = curr;
            curr = curr->next;
            free(tmp->word);
            free(tmp);
        }
    }
    free(idx);

    return 0;
}

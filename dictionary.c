/****************************************************************************
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Hashing algorithm referenced from the /r/cs50 subreddit:
 * https://www.reddit.com/r/cs50/comments/1x6vc8/pset6_trie_vs_hashtable/
 *
 * Implements a dictionary's functionality.
 ***************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"

#define HASHTABLE_SIZE 3000

typedef struct node
{
    char word[LENGTH + 1];
    struct node* next;
}
node;

node* hashTable[HASHTABLE_SIZE];

// prototype declaration
int calculateHash(char* key);

// Will hold the size of the dictionary
int wordCount = 0;

/**
 * Returns true if word is in dictionary else false.
 * Does case insensitive search.
 */
bool check(const char* word)
{
    // temp variable to store the lower-case string
    char lowered[LENGTH + 1];
    
    // it suffices to calculate the string length only once
    int wordSize = strlen(word);
    
    // we convert every character to lowercase
    for (int i = 0; i < wordSize; i++)
    {
        lowered[i] = tolower(word[i]);
    }
    
    // terminator character is required at the end of our lower-cased
    // string, otherwise the comparison will surely fail
    lowered[wordSize] = '\0';
    
    // calculate the hash value of the lowered string
    // which gives us the place to search for in the dictionary
    int idx = calculateHash(lowered);
    
    // if the given hash table location is empty, then the word
    // is not in our dictionary
    if (hashTable[idx] == NULL) return false;
    
    // we initialize a search helper cursor node
    node* seek = hashTable[idx];
    
    // we seek thru the given hash table index using the seek
    // node, comparing all the words along the way to find our match
    while (seek != NULL)
    {
        if (strcmp(lowered, seek->word) == 0) return true;
        
        seek = seek->next;
    }
    
    // after seeking thru if we come up empty, then the word is
    // definately not in the dictionary
    return false;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    // we open the given dictionary file in read-only mode
    FILE* fp = fopen(dictionary, "r");
    
    // unaccessable or empty file leads to us reporting a failure
    if (fp == NULL) return false;
    
    // variable to store the currently read dictionary word
    char buffer[LENGTH + 1];
    
    // actual line-by-line reading until the end of the file is hit
    while(fscanf(fp, "%s\n", buffer) != EOF)
    {
        // initialization of the temp node
        node* inWord = malloc(sizeof(node));
        
        // the currently read word is copied to the temp node
        strcpy(inWord->word, buffer);
        
        // let's calculate the words' place in our hash table
        int idx = calculateHash(buffer);
        
        // if the word would be the first in the hash table's given
        // list, then we seat it to be the head of that given list,
        // otherwise we just append it to the list
        if (hashTable[idx] == NULL)
        {
            hashTable[idx] = inWord;
            inWord->next = NULL;
        }
        else
        {
            inWord->next = hashTable[idx];
            hashTable[idx] = inWord;
        }
        
        // a successful line read means incrementing the size 
        // of the dictionary
        wordCount++;
    }
    
    // after reading its' contents, we close the dictionary file
    fclose(fp);
    
    // loading the dictionary was successful
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return wordCount;
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    // temp index variable used for traversing the lists
    int idx = 0;
    
    // we loop thru the lists of the hash table until hitting its' end
    while (idx < HASHTABLE_SIZE)
    {
        // if the list is already NULL then we skip onto the next list
        if (hashTable[idx] == NULL) idx++;
        
        // upon hitting a list with elements, we use a temporary seek node
        // to loop thru the list, freeing up the elements one-by-one by
        // stepping the seek cursor over the list items. This is necessary
        // in order to keep track of all the elements of the list even after
        // their previous node has been freed up.
        else
        {
            node* seek = hashTable[idx];
            hashTable[idx] = seek->next;
            free(seek);
        }
        
        idx++;
    }
    
    return true;
}

// calculates the hash value of a given character, using a left-shift
// arithmetic shift operator - it is proven to be faster than multiplying
int calculateHash(char* key)
{
    unsigned int hash = 0;
    
    for (int i = 0, n = strlen(key); i < n; i++)
    {
        hash = (hash << 2) ^ key[i];
    }
 
    return hash % HASHTABLE_SIZE;
}

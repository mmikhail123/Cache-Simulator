#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//cache line which acts like a node 
struct cacheLine{
    int valid;
    unsigned long tag;
    struct cacheLine* next;
    int age;
    int usage;
};

//cache which is an array of linked lists 
struct cache{
    int numofSets;
    struct cacheLine** adjLists;
};

//makes the cache
struct cache* makeCache(int numofSets){
    struct cache* cache = malloc(sizeof(struct cache));
    cache->numofSets = numofSets;

    //malloc adjLists which are number of sets
    cache->adjLists = malloc(numofSets * sizeof(struct cacheLine*));

    //initializes adjLists as NULL
    for(int i = 0; i < numofSets; i++){
        cache->adjLists[i] = NULL;
    }

    return cache;
}

//make node aka cache line 
struct cacheLine* makecacheLine(unsigned long tagValue){
    struct cacheLine* newLine = malloc(sizeof(struct cacheLine));

    newLine->tag = tagValue; //assigns tag to tag
    newLine->valid = 1; //initializes valid to be 1 since cache is empty
    newLine->next = NULL; //next pointing to NULL 
    newLine->age = 0;
    newLine->usage = 0;

    return newLine;
}

void addcacheLineFIFO(struct cache* cache, unsigned long tag, int setIndex, int age){
    struct cacheLine* check = NULL;
    struct cacheLine* cacheLine = makecacheLine(tag);
    cacheLine->age = age;

    if(cache->adjLists[setIndex] == NULL){
        cacheLine->next = cache->adjLists[setIndex];
        cache->adjLists[setIndex] = cacheLine;
    } else {
        check = cache->adjLists[setIndex];
        while(check->next != NULL){
            check = check->next;
        }
        cacheLine->next = check->next;
        check->next = cacheLine;
    }
}

void addcacheLineLRU(struct cache* cache, unsigned long tag, int setIndex){
    struct cacheLine* check = NULL;
    struct cacheLine* cacheLine = makecacheLine(tag);
    cacheLine->usage = 1;

    if(cache->adjLists[setIndex] == NULL){
        cacheLine->next = cache->adjLists[setIndex];
        cache->adjLists[setIndex] = cacheLine;
    } else {
        check = cache->adjLists[setIndex];
        while(check->next != NULL){
            check = check->next;
        }
        cacheLine->next = check->next;
        check->next = cacheLine;
    }
}

int degree(struct cache* cache, int setIndex){
    struct cacheLine* temp = cache->adjLists[setIndex];
    int count = 0;
    while(temp){
        count++;
        temp = temp->next;
    }
    return count; 
}

int checkHit(struct cache* cache, unsigned long tag, int setIndex){
    struct cacheLine* temp = cache->adjLists[setIndex];
    int count = 0;
    while(temp){
        if(temp->tag == tag){
            count = 1;
            break;
        }
        temp = temp->next;
    }
    return count;
}




int main(int argc, char *argv[]){
    int cachehit = 0;
    int cachemiss = 0;
    int memread = 0;
    int memwrite = 0;

    int globalAge = 0;
    //int usage = 0; 
    
    //32 assoc:2 fifo 4 trace1.txt
    int cacheSize;
    char stringAssoc[100];
    int assoc;
    char replacePolicy[10];
    int blockSize;

    sscanf(argv[1], "%d", &cacheSize); 
    strcpy(stringAssoc, argv[2]+6);
    sscanf(stringAssoc, "%d", &assoc);
    sscanf(argv[3], "%s", replacePolicy);
    sscanf(argv[4], "%d", &blockSize);

    int blockBits = log2(blockSize);
    int numSets = (cacheSize)/(assoc * blockSize);
    int setBits = log2(numSets);
    int tagBits = 48 - (blockBits + setBits);

    //printf("%s\n", replacePolicy);

    FILE* file = fopen(argv[5], "r");

    struct cache* cache = makeCache(numSets);
    int checkH = 0;
    int checkdegree = 0;

    char whattodo[100];
    unsigned long address; 
    while(fscanf(file, "%s %lx", whattodo, &address) == 2){
        //printf("what: %s add: %ld\n", whattodo, address);
        globalAge++;
        int setValue = (address >> blockBits) & (unsigned long)(pow(2,setBits) - 1); //gets setvalue 
        unsigned long tagValue = (address >> (blockBits+setBits)) & (unsigned long)(pow(2, tagBits) - 1); //gets tag value
               
        checkH = checkHit(cache,tagValue, setValue);
        //printf("checkh: %d", checkH);
        checkdegree = degree(cache, setValue);
        //printf("degree: %d", checkdegree);
        
        //FIFO REPLACE POLICY
        if(strcmp("fifo", replacePolicy) == 0){
            //cache hit
            if(checkH != 0){
                if(strcmp("R", whattodo) == 0){
                    cachehit++;
                }
                if(strcmp("W", whattodo) == 0){
                    cachehit++;
                    memwrite++;
                }
            } else {
                //cache miss
                if(checkH == 0){
                    if(strcmp("R", whattodo) == 0){
                        cachemiss++;
                        memread++;
                    }
                    if(strcmp("W", whattodo) == 0){
                        cachemiss++;
                        memread++;
                        memwrite++;
                    }
                }
                //theres space to fill in new line
                if(checkdegree < assoc){
                    //printf("HELLO\n");
                    addcacheLineFIFO(cache, tagValue, setValue, globalAge);

                } else {
                    //traverse and look for youngest block
                    struct cacheLine* minLine = cache->adjLists[setValue];
                    struct cacheLine* temp = cache->adjLists[setValue];
                    while(temp){
                        if(temp->age < minLine->age){
                            minLine = temp;
                        }
                        temp = temp->next;
                    }
                    //write over youngest block with new tagVal/current global age
                    minLine->tag = tagValue;
                    minLine->age = globalAge;
                }
            }
        }

        if(strcmp("lru", replacePolicy) == 0){
            if(checkH != 0){
                //increments usage of the line thats being used again
                struct cacheLine* temp = cache->adjLists[setValue];
                while(temp){
                    if(temp->tag == tagValue){
                        temp->usage = temp->usage + 1;
                    }
                    temp = temp->next;
                }
                
                if(strcmp("R", whattodo) == 0){
                    cachehit++;
                }
                if(strcmp("W", whattodo) == 0){
                    cachehit++;
                    memwrite++;
                }
            } else {
                //cache miss
                if(checkH == 0){
                    if(strcmp("R", whattodo) == 0){
                        cachemiss++;
                        memread++;
                    }
                    if(strcmp("W", whattodo) == 0){
                        cachemiss++;
                        memread++;
                        memwrite++;
                    }
                }

                if(checkdegree < assoc){
                    addcacheLineLRU(cache, tagValue, setValue);
                } else {
                    struct cacheLine* leastUsed = cache->adjLists[setValue];
                    struct cacheLine* temp = cache->adjLists[setValue];
                    while(temp){
                        if(temp->usage < leastUsed->usage){
                            leastUsed= temp;
                        }
                        temp = temp->next;
                    }
                    leastUsed->tag = tagValue;
                    leastUsed->usage = 1;
                }
            }
        }
    }
    printf("memread:%d\nmemwrite:%d\ncachehit:%d\ncachemiss:%d\n", memread, memwrite, cachehit, cachemiss);
    int v;
    for(v = 0; v < cache->numofSets; v++){
        struct cacheLine* temp = cache->adjLists[v];
        while(temp!=NULL){
            struct cacheLine* next = temp->next;
            free(temp);
            temp = next;
        }
    }
    free(cache->adjLists);
    free(cache);
}




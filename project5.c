#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "Hashtable.h"

int numWords;

void printHashTable(struct table *t);

static int comp(const void *a, const void *b){//Comparison method to be used by qsort() to sort page ranks
  if (*(double*)a > *(double*)b){//Checks which is greater, and returns the corresponding value(list sorted in descending order)
    return -1;
  } else if (*(double*)a < *(double*)b){
    return 1;
  } else {
    return 0;
  }
}

void stop_word(struct table* table){
	int i = 0;
	int j = 0;

	struct node* currNode;

	for (i = 0; i < table->numBuckets; i++){//Go through each bucket in the table
		currNode = table->buckets[i].head;
		for (j = 0; j < table->buckets[i].numNodes; j++){//Go through each word in the list
			if (currNode->idf <= 0.00001){//if the IDF of that node in the list is 0 (less than a small number), delete that word from the list.
				remNode(table, currNode->word);
				currNode = currNode->next;
			}
		}
	}
}

void rank(struct table* table, char** query, int numDocs){
  int i = 0;
  int j = 0;
  int k = 0;
  double *documentWordSum = malloc(sizeof(double) * (numDocs));
  double *originalOrder = malloc(sizeof(double) * (numDocs));
  int *dontPrint = malloc(sizeof(int) * numDocs);
  int *docsPrinted = malloc(sizeof(int));
  int docsPrintedIndex = 0;
  int flag = 0;
  int flag2 = 0;

  //Figures out which documents contain 0 of the words, and thus which document not to print, BEFORE removing the stop words.
  for (j = 0; j < numDocs; j++){
    for (i = 0; i < numWords; i++){//Go through all words in the query, and see if the find freq returns 0 for all
	if (findFreq(table, query[i], j + 1) >= 0.5){
        break;
      }
    }
    if(i == numWords){ //If the loop did not break before going through all numWords, then the document must not have any of the words. THIS HAS TO BE DONE BEFORE STOP WORDS ARE REMOVED.
      dontPrint[k] = j + 1; //Record which document should not be printed.
      k++;
   } else {
      dontPrint[k] = 0; //Guarantees that the rest of the array will be 0'ed, besides for the document to not be printed.
      k++;
    }
  }



  idf(table, numDocs);//Changes the hashtable frequency to account for the idf
  stop_word(table);//removes stop words

  for (i = 0; i < numDocs; i++){
	documentWordSum[i] = 0; //Set the array at each index to 0 in the beginning.
	for (j = 0; j < numWords; j++){
		documentWordSum[i] += findFreq(table, query[j], i + 1);//Sums the frequency of every word for each document
	}
	originalOrder[i] = documentWordSum[i];//Save the oringal document tf-idf value order into another array
  }

  qsort(documentWordSum, numDocs, sizeof(double), comp);//Sort the documents by rank


  printf("List of relevant documents: \n");

  for (i = 0; i < numDocs; i++){//For the highest score, go through the original order, and when one matches, the index of that plus one is the document id.
    for (j = 0; j < numDocs; j++){//This loop goes through the orininal document order array
      if (fabs(originalOrder[j] - documentWordSum[i]) < 0.00001){//Checks if this document
        for (k = 0; k < docsPrintedIndex; k++){
          if (docsPrinted[k] == j+1){//Prevents the same document from being printed twice.
            flag = 1;
            break;
          }
        }
        if (flag == 0){//If flag is still 0, then the document hasn't already been printed
          for (k = 0; k < numDocs; k++){//Go through the documents that should not be printed (has none of the query words).
            if (dontPrint[k] == j+1){
              flag2 = 1;
            }
          }
          //If it hasn't hit a break, then the document is not part of the don't print, so its ok to print
          if (flag2 == 0){
            printf("D%d.txt\n", j + 1);
            docsPrinted[docsPrintedIndex] = j + 1;//Makes sure the document is not printed again
            docsPrintedIndex++;
            break;
          } else {
            flag2 = 0;
            continue;
          }
        }
        flag = 0;
      }
    }
  }
}

int training(struct table* table){
  //Read in all the words of the documents into the hashtable.
  FILE *inFile = malloc(sizeof(FILE));//If inFile is a pointer to a file, then we must need to allocate memory sizeof(FILE) on the stack
  int i = 1;
  char fileName[100];
  char* buffer = malloc(LINE_MAX * sizeof(char));
  char* word;


  sprintf(fileName, "./dir/D%d.txt", i);//Set fileName = the path + the index variable of the document


  while ((inFile = fopen(fileName, "r")) != NULL){  //continue to read in files until opening it yields NULL (aka, no file exists)
     // printf("Opened file: %s\n", fileName);
      while(fgets(buffer, LINE_MAX, inFile) != NULL){ //continue to read lines into buffer until the last line.
        //split it up by word, and add them to the hashtable
        if (strcmp(buffer, "\n") == 0){//If the line is empty, but there are still more lines to be read, skip to next read line
          continue;
        }

        if (buffer[strlen(buffer) - 1] == '\n'){ //If the last character is a newline (all lines besides the last line), remove it
          buffer[strlen(buffer) - 1] = '\0';
        }

        word = strtok(buffer, " ");  //Read the first word and set the string for strtok

        if(word != NULL){
          hash_table_insert(table, word, i);//Insert the first word into the table
        }

        while ((word = strtok(NULL, " ")) != NULL){
          hash_table_insert(table, word, i);//Insert every word in the line to the hashtable
        }
      }

      fclose(inFile);//Close the file when you are done

      i++;
      sprintf(fileName, "../Project5/dir/D%d.txt", i);//Set the filename to Di+1.txt
  }

  free(buffer);

  return i - 1;//i will be 1 past the set of documents.

}

void printHashTable(struct table* t){
  int i = 0;

  for (i = 0 ;i < t->numBuckets; i++){
    printList(t->buckets + i); //should be the ith term in the array of pointers
  }
  //just call linked list print on each bucket
}


char** read_query(){
  int i = 0;
  char** query; //Query needs to be a double pointer because you have a pointer to the word, and that is a pointer to an array of characaters
  char* word;
  char buffer[LINE_MAX];

  printf("Enter your search query: ");
  fgets(buffer, LINE_MAX, stdin);  //First fgets to clear the left over newline from the scanf.
  fgets(buffer, LINE_MAX, stdin);
  printf("\n");

  query = malloc(sizeof(char*));  //Malloc query to be the number of termns. Then, you need to malloc each one according to its size (strlen)

  word = strtok(buffer, " ");  //Read the first word and set the string for strtok
  query[i] = malloc(sizeof(char) * (strlen(word) + 1));  //Malloc that specific word for
  strcpy(query[i], word);

  while ((word = strtok(NULL, " ")) != NULL){
    i++;
    query = realloc(query, sizeof(char*) * i+1); //Increase the size of query by 1 for each new word that is read in.
    query[i] = malloc(sizeof(char) * (strlen(word) + 1));
    strcpy(query[i], word);
  }
  query[i][strlen(query[i]) - 1] = '\0';  //The last character in the last word is going to have a newline character, which is at index of 1 less than the string length. Set that to end string character to delete it.

  numWords = i + 1;//The number of words will be the last index of query + 1
  return query;
}

int main(){
  int numBuckets;
  char** query;
  int i = 0;

  struct table *table = malloc(sizeof(struct table)); //Memory allocated for table (should only have to allocate space for one pointer)

  //Read user input for number of bucetks:
  printf("How many buckets should the hashtable have?");
  scanf("%d", &numBuckets);

  //Malloc that many buckets
  table->buckets = malloc(sizeof(struct bucket) * numBuckets);
  table->numBuckets = numBuckets;

  //Read in search query

  query = read_query();

  i = training(table);
  rank(table, query, i);
  printHashTable(table);

  printf("\n");
  return 0;
}

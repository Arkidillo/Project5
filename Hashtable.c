#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Hashtable.h"

struct bucket* hash_code(struct table* table, char* word){
  int j = 0;//Calculate the hash value of the word
  int sum = 0;
  struct bucket *list;
  for (j = 0; j < strlen(word); j++){
    sum += (int)word[j];  //Sums the ascii values of the word
  }
  list = table->buckets + (sum % table->numBuckets);
  return list;
}

void hash_table_insert(struct table* t, char* word, int id)
{  //Insert function takes input as pointer to an array  of characters (string), and i, the id of the document.
  addNode(hash_code(t, word), word, id); //Calls linked list add on the bucket, using the modulus hashing algorithm, passing it an actual bucket
}

void printList(struct bucket *list)
{
  /* Print a list from head to tail. */
  /* DO NOT MODIFY THIS CODE */
  struct node *entry;
  if(list == NULL) {
    printf("List has not been initialized!\n");
    return;
  }
  entry = list->head;
  printf("List Contents: + length: %d\n", list->numNodes);
  while(entry != NULL)
  {
    printf("  %s idf: %f\n", entry->word, entry->idf);
    printDocs(entry->head);
    entry = entry->next;
  }
}
void printDocs(struct docNode *head){
	while(head!=NULL){
		printf("\t%d: %f\n", head->id, head->frequency);
		head = head->next;
	}
}


void addNode(struct bucket *list, char* word, int id)
{
  struct node *newNode;
  struct node *currNode = list->head;
  if (list == NULL){//Checks if the list has not been initialized
    printf("ERROR: List is empty \n");
    return;
  }


    if (currNode == NULL){//checks if the list is empty, if it is , it will add the node there
      	list->numNodes = 1;
	newNode = malloc(sizeof(struct node));
	newNode->word = malloc(strlen(word) + 1);
	newNode->idf = 0;
	newNode->numDocs = 1;
	newNode->head = malloc(sizeof(struct docNode));
	strcpy(newNode->word, word);
	newNode->head = NULL;
	newNode->next = NULL;
	addDoc(newNode, id);
	list->head = newNode;
      return;
    }

	if (strcmp(currNode->word, word) == 0){//if the head has the word you are looking to add, add a document there
		currNode->numDocs += addDoc(currNode, id);
		return;
	}
      while(currNode->next != NULL){//iterates through the loop until you reach the end of the list
        if (strcmp(currNode->next->word, word) == 0){//If the words are the same, and from the same doccument, then increment the frequency of the word, and don't insert it
          currNode->next->numDocs +=  addDoc(currNode->next, id);//returns a 0 if the document was already counted
          return;
        }

        currNode = currNode->next;

      }

      if (strcmp(currNode->word, word) == 0){//LAST WORD: if the words are the same, and from the same doccument, then increment the frequency of the word, and don't insert it
	  currNode->numDocs +=  addDoc(currNode, id);
          return;
        }
	list->numNodes++;
	newNode = malloc(sizeof(struct node));
	newNode->word = malloc(strlen(word) + 1);
	newNode->idf = 0;
	newNode->numDocs = 1;
	newNode->head = malloc(sizeof(struct docNode));
	strcpy(newNode->word, word);
	newNode->head = NULL;
	newNode->next = NULL;
	addDoc(newNode, id);//make sure the doc is inserted into the linked list of documents
	currNode->next = newNode;//inserts the node once you have reacahed the end of the list
      return;

}

int remNode(struct table *table, char* word)
{
  struct bucket *list = hash_code(table, word);
  int i = 1;
  struct node *currNode = list->head;
  struct node *tmpNextNode;
  if (list == NULL){
    //No way to return pointer to a new empty list.
    printf("ERROR: List is empty \n");
    return -1;
  }

   if (list->head == NULL) {//Handle empty list
       return -1;
    }
  list->numNodes--;
    if (strcmp(currNode->word, word) == 0) {//Handle remove head
      freeAll(list->head->head);
      list->head = currNode->next;
      free(currNode->word);
      free(currNode);
      return 0;
    }


    tmpNextNode = currNode->next;
    while (currNode->next != NULL){//Iterate through the list until you reach the end of the list, or you reach the node with the input name. Doesn't break/return when the word is found, so should delete all instances.
      if (strcmp(currNode->next->word, word) == 0) {
        freeAll(currNode->next->head);
	tmpNextNode = currNode->next;
        currNode->next = currNode->next->next;
        free(currNode->word);
	free(tmpNextNode);
        return i;
      }
      currNode = currNode->next;
      i++;
    }

  //if you reach the end of the while loop without returning, the node must not be in the list, so you don't need to do anything
    return -1;
}
/*
double sumDoc(struct table *table, int id){
  int i = 0;
  double sum = 0;
  struct bucket *list;

  for (i = 0; i < table->numBuckets; i++){
    list = table->buckets + i; //list is a point to one of the buckets in the table's array of buckets.

    if (list == NULL){
      printf("ERROR: List is empty \n");
      return -1;
    }

    struct node* currNode = list->head;
    while (currNode != NULL) {
      if (currNode->id == id){
        sum += currNode->frequency;//adds the frequency of each word to the sum of the total words of the document
      }
      currNode = currNode->next;
    }
  }
  return sum;
}

void normalizeFreq(struct table *table, int id, double sum){//Takes the sum of the total number of words in a document
  int i = 0;
  struct bucket *list;

  for (i = 0; i < table->numBuckets; i++){
    list = table->buckets + i; //list is a pointer to one of the buckets in the table's array of buckets.

    if (list == NULL){
      printf("ERROR: List is empty \n");
      return;
    }

    struct node* currNode = list->head;
    while (currNode != NULL) {//Handle empty list
      if (currNode->id == id){
        currNode->frequency = currNode->frequency/sum;//Sets all nodes frequency equal to rawFrequency/N to normalize/
      }
      currNode = currNode->next;
    }
  }
}
*/
double sumWord(struct table *table, char* word){
  //Go through the table, and sum how many documents the word appears in. Save a list of documents that the word has been see in. THERE IS ONLY 1 INSTANCE OF EACH WORD PER DOCUMENT. EVERY TIME YOU SEE IT, YOU KNOW IT IS A UNIQUE DOCUMENT.
 int sum = 0;

  struct bucket *list = hash_code(table, word);  //list will be a pointer to the bucket where the word hashes to.
  struct node *currNode = list->head;
    if (list == NULL){
      return 0;
    }

    while (currNode != NULL) {
      if (strcmp(currNode->word, word) == 0){ //If you find the word, increment how many document it is seen in.
        sum++;
      }
      currNode = currNode->next;
    }
  return sum;
}

double findFreq(struct table *table, char* word, int id){

  struct bucket *list = hash_code(table, word);

  struct node *currNode = list->head;
  struct docNode *docNode;
  while (currNode != NULL){
    if (strcmp(currNode->word, word) == 0){
	docNode = currNode->head;
	while(docNode != NULL){
		if(docNode->id == id){
			return docNode->frequency;
		}
		docNode = docNode->next;
	}
    }
    currNode = currNode->next;
 }
  return 0;
}

void idf(struct table *table, int numDocs){
        //Overall: Go through each bucket in the table. Go through each word in that bucket. Find how many documents that word appears in using sumWord (below). Multiply the current document's frequency with the log(numDocs/sumWorD).
	int i = 0;
	int j = 0;

	struct node *currNode;

	for (i = 0; i < table->numBuckets; i++){
		currNode = table->buckets[i].head;
		for (j = 0; j < table->buckets[i].numNodes; j++){
			//This will go through every word. For each word, multiply the word by log(NumDocs/ nubmer of documents the word appears in). This is calculated by sumWord.
			currNode->idf = log10((double)numDocs/(double)(currNode->numDocs));
			changeFreq(currNode->head, currNode->idf);
			currNode = currNode->next;
		}
	}

	return;
}

int addDoc(struct node *node, int id){//adds a document to the linkedlist of documents for a particular word
	struct docNode *currNode = node->head;
	if(currNode == NULL){
		node->head = createDoc(id);
		return 1;
	}

	if(currNode->id == id){
		currNode->frequency++;
		return 0;
	}
	while (currNode->next != NULL){//Go through the list, if it is already in the list, increment the term frequency
		if(currNode->next->id == id){
			currNode->next->frequency++;
			return 0;
		}
	currNode = currNode->next;//Go to the next node in the document list.
	}
	currNode->next = createDoc(id);
return 1;
}
/*
int findTf(struct docNode *head, int id){//return the tf of a document.
	while (head != NULL){//Go through the list and find the id.
		if(head->id == id){
			return head->tf;
		}
	head = head->next;
	}//If you went through the list, and you don't find the id, then the list doesn't contain that document
return -1;
}
*/
void freeAll(struct docNode *head){
	struct docNode *tmp;
	while (head != NULL){
		tmp = head->next;
		free(head);
		head = tmp;
	}
}

void changeFreq(struct docNode *head, double idf){//the frequnecy of a word  in a document in the linked list of documents = tf * idf of the word
	while(head != NULL){
		head->frequency = idf * head->frequency;
		head = head->next;
	}
}

struct docNode* createDoc(int id){
	struct docNode *newNode = malloc(sizeof(struct docNode));
	newNode->next = NULL;
	newNode->id = id;
	newNode->frequency = 1.0;
	return newNode;
}

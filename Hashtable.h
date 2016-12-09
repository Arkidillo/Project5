struct table{
  int numBuckets;
  struct bucket *buckets;
};

struct bucket{ 
  struct node *head;
  int numNodes;
};

struct node{
  int numDocs;
  double idf;
  char* word;
  struct node *next;
  struct docNode *head;
};

struct docNode{
  int id;
  double frequency;
  struct docNode *next;
};

struct bucket* hashWord(struct table* table, char* word);

void hash_table_insert(struct table* t, char* word, int i);
  
void printList(struct bucket *list);

void printDocs(struct docNode *head);
  
void addNode(struct bucket *list, char* word, int id);

int remNode(struct table *table, char* word);
/*
double sumDoc(struct table *table, int id);

void normalizeFreq(struct table *table, int id, double sum);
*/
double sumWord(struct table *table, char* word);

double findFreq(struct table *table, char* word, int id);

void idf(struct table *table, int numDocs);

int addDoc(struct node *node, int id);

void freeAll(struct docNode *head);

void changeFreq(struct docNode *head, double idf);

struct docNode* createDoc(int id);

#include <stdio.h>
#include <stdbool.h>
#define NO DELETED REGISTERS =1
#define INDEX HEADER SIZE 8

#define DATA HEADER SIZE 4
/*primary key s i z e */
#define PK_SIZE 4
/*Our table i s going to contain a s t r i n g ( t i t l e ) and
an alphanumeric primary key ( book id )
*/
typedef struct book {
  char book_id [PK_SIZE] ; /*primary key */
  size_t title_len ; /*t i t l e length */
  char *title; /*s t r i n g to be saved in the database */
}Book ;
/*Note that in general a struct cannot be used to
handle tables in databases since the table structure
is unknown at compilation time.
*/
typedef struct node {
  char book_id [ PK_SIZE ] ;
  int left , right , parent , offset;
}Node;
/*Function prototypes .
see function descriptions in the following sections
All function return true if success or false if failed
except findKey which return true if register found
and false otherwise .
*/

bool createTable(const char *tableName)
{
  FILE *f;
  int firstDeleted = -1, len = strlen(tableName);

  if (tableName == NULL || len < 4 || strcmp(tableName + len - 4, ".dat") != 0)
    return false;

  /* Try opening the existing file */
  f = fopen(tableName, "rb");

  if (f == NULL)
  {
    /* Create the file if it doesn't exist */
    f = fopen(tableName, "wb");

    if (f == NULL)
      return false;

    /* Initialize header */
    if (fwrite(&firstDeleted, sizeof(int), 1, f) != 1)
    {
      fclose(f);
      return false;
    }
  }
}


bool createIndex(const char *tableName)
{
  FILE *f;
  char indexName[260];
  int header[2] = {-1, -1};
  int len;

  if (tableName == NULL)
    return false;

  len = strlen(tableName);

  if (len < 4 || strcmp(tableName + len - 4, ".dat") != 0)
    return false;

  strcpy(indexName, tableName);
  strcpy(indexName + len - 4, ".idx");

  /* Check if index already exists */
  f = fopen(indexName, "rb");

  if (f == NULL)
  {
    /* Create index file */
    f = fopen(indexName, "wb");

    if (f == NULL)
      return false;

    if (fwrite(header, sizeof(int), 2, f) != 2)
    {
      fclose(f);
      return false;
    }
  }

  return fclose(f) == 0;
}

bool findKey (const char *book_id, const char *indexName, int *nodeIDOrDataOffset);
bool addTableEntry (Book *book, const char *tableName, const char *indexName ) ;
bool addIndexEntry (char *book_id , int bookOffset, const char *indexName ) ;
void printTree ( size_t level, const char *indexName) ;
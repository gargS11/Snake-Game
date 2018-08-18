 //SNAKE GAME FINAL INCLUDING HASHING AND HAVING OPTION OF SAVING A FILE.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int R = 18;
int C = 24; 
struct snake{
  int x, y;
  char value;
  struct snake *next;
};
struct food {
  int x, y;
  char value;
};
struct node{
  int x, y;
  struct node *next; 
};
struct hashtable{
  int count;
  struct node *head;
};
void displayMatrix(char matrix[R][C]){
  int i, j;
  for(i = 0; i < R; i++)
  {
    for(j = 0; j < C; j++)
      printf("%c ", matrix[i][j]);
    printf("\n");
  }
}
void deleteHashNode(struct hashtable *htable, int x, int y){
  int key = x + y;
  htable[key].count--;
  struct node *prev, *temp;
  temp = htable[key].head;
  if(temp->x == x && temp->y == y)
    htable[key].head = temp->next;
  else{
    prev = temp;
    temp = temp->next;
    while(temp->x != x && temp->y != y)
    {
      prev = temp;
      temp = temp->next;
    }
    prev->next = temp->next;
  }
}
void insertHashNode(struct hashtable *htable, int x, int y){
    int key = x + y;
    htable[key].count++;
    struct node *n;
    n = (struct node *)malloc(sizeof(struct node));
    n->x = x;
    n->y = y;
    n->next = htable[key].head;
    htable[key].head = n;
}
void transferSnakeToHash(struct snake *shead, struct hashtable *htable){
  struct snake *ptr;
  ptr = shead->next;
  int key;
  while(ptr != NULL){
    insertHashNode(htable, ptr->x, ptr->y);
    ptr = ptr->next;
  }
}
int checkCollision(struct hashtable *htable, int x, int y){
  int key = x + y;
  if(htable[key].count > 0)
  {
    struct node *ptr;
    ptr = htable[key].head;
    while(ptr != NULL)
    {
      if(ptr->x == x && ptr->y == y){
	    return 0;
      }
      ptr = ptr->next;
    }
  }
  return 1;
}
struct snake* insertNode(struct snake *shead, int a, int b){
    struct snake *n, *ptr;
    n = (struct snake *)malloc(sizeof(struct snake));
    n->x = a;
    n->y = b;
    if(shead == NULL){
      n->value = 'o';
      n->next = shead;
      shead = n;
    }
    else{
      ptr = shead;
      while(ptr->next != NULL)
        ptr = ptr->next;
      n->value = 'x';
      n->next = ptr->next;
      ptr->next = n;
    }
    return shead;
}
void swap(int *a, int *b){
  int t = *a;
  *a = *b;
  *b = t;
}
struct snake* changeSnakeDirection(struct hashtable *htable,struct snake *shead,
                                   char direction,char matrix[R][C],int flag)
{
   int prevx = shead->x, prevy = shead->y;
   insertHashNode(htable, prevx, prevy);
   switch(direction)
   {
     case 'l': shead->y = shead->y - 1; 
               if(shead->y == 0)
                 shead->y = C-2;
       break;
     case 'r': shead->y = shead->y + 1;
               if(shead->y == C-1)
                 shead->y = 1;
       break; 
     case 'u': shead->x = shead->x - 1; 
               if(shead->x == 0)
                 shead->x = R-2;
       break;
     case 'd': shead->x = shead->x + 1; 
               if(shead->x == R-1)
                 shead->x = 1;
       break;
    }
    struct snake *ptr;
    ptr = shead->next;
    while(ptr != NULL){
        swap(&prevx, &ptr->x);
        swap(&prevy, &ptr->y);
        ptr = ptr->next;
    }
   if(flag == 1){
    matrix[prevx][prevy] = ' ';
    deleteHashNode(htable, prevx, prevy); 
   }
  else
    shead = insertNode(shead, prevx, prevy);
  return shead;
}
int inserttemp(char *str, char *temp, int i){
	int l = strlen(temp) - 1;
	while(l >= 0)
	{
		str[i++] = temp[l];
		l--;
	}
	str[i++] = ',';
	return i;
}
void gettemp(int n, char *temp){
	int j = 0;
	while(n){
		temp[j++] = n%10 + '0';
		n = n/10;
	}
	temp[j] = '\0';
}
void savegame(struct snake *shead, struct food sfood,int score,FILE *fil){
  char str[2000];
  char temp[10];
  int i = 0;
  str[i++] = 'S';      //Starting saving Score
  gettemp(score, temp);
  i = inserttemp(str, temp, i);
  
  str[i++] = 'f';      //Starting saving Food 
  gettemp(sfood.x, temp);
  i = inserttemp(str, temp, i);
  gettemp(sfood.y, temp);
  i = inserttemp(str, temp, i);
  
  str[i++] = 's';      //Starting saving Snake
  struct snake *ptr;
  ptr = shead;
  while(ptr != NULL){
    gettemp(ptr->x, temp);
    i = inserttemp(str, temp, i);
    gettemp(ptr->y, temp);
    i = inserttemp(str, temp, i);
    str[i++] = '#';
    ptr = ptr->next;
  }
  str[i] = '\0';
  fil = fopen("snake.txt", "w");
  fprintf(fil, "%s", str);
  fclose(fil);
  //printf("%s\n", str);
}
void transferFoodToMatrix(char matrix[R][C], struct food sfood){
  matrix[sfood.x][sfood.y] = sfood.value;
}
void transferSnakeToMatrix(char matrix[R][C], struct snake *shead){
  struct snake *ptr;
  ptr = shead;
  while(ptr != NULL)
  {
    matrix[ptr->x][ptr->y] = ptr->value;
    ptr = ptr->next;
  }
}
void initHashTable(struct hashtable *htable){
	int i;
	for(i = 0; i < (R+C); i++)
	{
		htable[i].count = 0;
		htable[i].head = NULL;
	}
}
int getnum(char *str, int *i){
  int k = 0;
  while(str[*i] != ',')
  {
    k = k*10 + str[*i]-48;
    *i += 1;
  }
  *i += 1;
  return k;
}
struct snake* getSavedSnake(struct snake *shead, char *str){
  int i = 0, a, b;
  while(str[i] != 's')
    i++;
   if(str[i++] == 's')
   {
    while(str[i] != '\0'){
      a = getnum(str, &i);
      b = getnum(str, &i);
      if(str[i++] == '#')
        shead = insertNode(shead, a, b);
    }
  }
  return shead;
}
struct food getSavedFood(struct food sfood, char *str){
  int i = 0, a, b;
  while(str[i] != 'f')
    i++;
  if(str[i++] == 'f'){
      sfood.x = getnum(str, &i);
      sfood.y = getnum(str, &i);
      sfood.value = '*';
  }
  return sfood;
}
int getSavedScore(char *str){
  int i = 0;
  if(str[i++] == 'S')
    return getnum(str, &i);
}
struct food createFood(struct food sfood){
	srand(time(0));
	sfood.x = rand() % R-2;
	sfood.y = rand() % C-2;
	sfood.value = '*';
	return sfood;
}
struct snake* initSnake(struct snake *shead){
  int k = 5, a = 3, b = 4;
  while(k--){
    shead = insertNode(shead, a, b);
    a++;
  }
  return shead;
}
int userInput(){
  printf(" 1 - To start a New Game\n");
  printf(" 2 - To start the last saved Game\n");
  int option;
  scanf("%d", &option);
  return option;
}
void initMatrix(char matrix[R][C]){
  int i, j;
  matrix[0][0] = matrix[0][C-1] = ' ';
  for(i = 1; i < R; i++){
    matrix[i][0] = '|';
    matrix[i][C-1] = '|';
  }
  for(i = 1; i < C-1; i++){
    matrix[0][i] = '.';
    matrix[R-1][i] = '.';
  }
  for(i = 1; i < R-1; i++)
    for(j = 1; j < C-1; j++)
      matrix[i][j] = ' ';
}
int main()
{
  int score = 0;
  FILE *fil;
  char matrix[R][C];
  initMatrix(matrix);
  struct hashtable *htable=(struct hashtable*)malloc((R+C)*sizeof(struct hashtable));
  //char initdirection = 'u';
  struct snake *shead = NULL;
  struct food sfood;
  int option = userInput();
  if(option == 1)
  {
    shead = initSnake(shead);
    transferSnakeToMatrix(matrix, shead);
    do{
       sfood = createFood(sfood);
    }while(matrix[sfood.x][sfood.y] != ' ');
    transferFoodToMatrix(matrix, sfood);
  }
  else
  {
  	fil = fopen("snake.txt", "r");
  	if(fil != NULL){
        char str[2000];
        fscanf(fil, "%[^\n]s", str);
        fclose(fil);
        score = getSavedScore(str);
        sfood = getSavedFood(sfood, str);
        shead = getSavedSnake(shead, str);
        transferFoodToMatrix(matrix, sfood);
    	transferSnakeToMatrix(matrix, shead);
    }
    else{
		printf("You don't have any saved game\n");
        exit(0);
	}
  }
  printf("INITIALIZING\n");
  initHashTable(htable);
  transferSnakeToHash(shead, htable);
  system("cls");
  printf("\n SCORE:%d\n", score);
  displayMatrix(matrix);
  
  while(1)
  {
    printf(" PRESS s to save the game\n PRESS e to exit\n ");
    char direction;
    scanf(" %c", &direction);
    system("cls");
    if(direction == 's')
    {
      savegame(shead, sfood, score, fil);
      printf("..........GAME SAVED................");
      break;
    }
    else if(direction == 'e'){
	  printf("............You Exit...............");
      exit(0);
    }
    else{
      if(shead->x == sfood.x && shead->y == sfood.y)
      {
      	 score += 2;
         shead = changeSnakeDirection(htable,shead, direction, matrix, 0);
         sfood = createFood(sfood);
         while(1){
           if(checkCollision(htable,sfood.x,sfood.y)&&sfood.x >0 && sfood.y >0){
             break;
           }
           else
             sfood = createFood(sfood);
         }
        transferFoodToMatrix(matrix, sfood);
      }
      else{
        shead = changeSnakeDirection(htable, shead, direction, matrix, 1);
      }
      transferSnakeToMatrix(matrix, shead);
      
      if(checkCollision(htable, shead->x, shead->y)){
	     printf("\n SCORE:%d\n", score);
	     displayMatrix(matrix);
      }
      else{
        printf(".........Game Ends...........");
        break;
      }
    }
  }
  return 0;
}

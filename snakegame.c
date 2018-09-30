#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>  //for srand()

int R = 18; //matrix board length
int C = 24; //matrix board width
//struct of a snake node
struct snake{  
  int x, y;
  char value;
  struct snake *next;
};
//struct of food
struct food {
  int x, y;
  char value;
};
//struct of a node for the hash
struct node{
  int x, y;
  struct node *next; 
};
//hash structure
struct hashtable{ 
  int count;
  struct node *head;
};
//deleting a node in hash
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
//inserting a node in hash
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
//inserting a node in snake
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
//swapping two integers
void swap(int *a, int *b){
  int t = *a;
  *a = *b;
  *b = t;
}
//moving snake according to the direction and updating in hash
struct snake* changeSnakeDirection(struct hashtable *htable,
 struct snake *shead,char direction,char matrix[R][C],struct food sfood)
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
   //if snake eats food then new node is added to the tail of snake
   if(shead->x == sfood.x && shead->y == sfood.y)
	  shead = insertNode(shead, prevx, prevy);
   //otherwise tail of snake is deleted from hash as it has move frwrd
   else{
    	matrix[prevx][prevy] = ' ';
        deleteHashNode(htable, prevx, prevy); 
   }
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
//saving game to the file
void savegame(struct snake *shead,struct food sfood,int score,FILE *fil)
{
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
}
//checking if snake collides with board boundary
int noBoundaryCollision(struct snake *shead){
  if(shead->x==1 && shead->next->x==R-2 ||
     shead->x==R-2 && shead->next->x==1 ||
	 shead->y==1 && shead->next->y==C-2 ||
	 shead->y>=C-2 && shead->next->y==1)
    return 0;
  return 1;
}
//checking if snake collides with itself
int snakeCollision(struct hashtable *htable, int x, int y){
  int key = x + y;
  if(htable[key].count > 0)
  {
    struct node *ptr;
    ptr = htable[key].head;
    while(ptr != NULL)
    {
      if(ptr->x == x && ptr->y == y){
	    return 1;
      }
      ptr = ptr->next;
    }
  }
  return 0;
}
//displaying matrix board
void displayMatrix(char matrix[R][C]){
  int i, j;
  for(i = 0; i < R; i++)
  {
    for(j = 0; j < C; j++)
      printf("%c ", matrix[i][j]);
    printf("\n");
  }
}
//inserting food into matrix board
void transferFoodToMatrix(char matrix[R][C], struct food sfood){
  matrix[sfood.x][sfood.y] = sfood.value;
}
//inserting snake into matrix board
void transferSnakeToMatrix(char matrix[R][C], struct snake *shead){
  struct snake *ptr;
  ptr = shead;
  while(ptr != NULL)
  {
    matrix[ptr->x][ptr->y] = ptr->value;
    ptr = ptr->next;
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
//returning saved snake 
struct snake* getSavedSnake(struct snake *shead, char *str){
  int i = 0, a, b;
  while(str[i] != 's')
    i++;
  if(str[i++] == 's'){
    while(str[i] != '\0'){
      a = getnum(str, &i);
      b = getnum(str, &i);
      if(str[i++] == '#')
        shead = insertNode(shead, a, b);
    }
  }
  return shead;
} 
//returning saved food
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
//returning saved score in the file
int getSavedScore(char *str){
  int i = 0;
  if(str[i++] == 'S')
    return getnum(str, &i);
}
//checking that food should not lie on the snake & boundary
int isNotValid(struct food sfood, struct hashtable *htable){
  //checking if food lies on boundary
  if(sfood.x<=0||sfood.x>=R-1 || sfood.y<=0||sfood.y>=C-1)
    return 1;
  int key = sfood.x + sfood.y; //generating key for food coordinates
  if(htable[key].count > 0){
    struct node *ptr;
    ptr = htable[key].head;
    while(ptr != NULL)
    {
      if(ptr->x == sfood.x && ptr->y == sfood.y)
        return 1;     
      ptr = ptr->next;
    }
  }
  return 0;     //returning zero if food coords are valid
}
//generating random coordinates for food of the snake
struct food createFood(struct food sfood){
	srand(time(0)); //taking current systim to generate rndom food
	sfood.x = rand() % R-2;  //x-coordinate
	sfood.y = rand() % C-2;  //y-coordinate
	sfood.value = '*';       //setting value of food to *
	return sfood;
}
//making entry of snake in the hash
void transferSnakeToHash(struct snake *shead, struct hashtable *htable)
{
  struct snake *ptr;
  ptr = shead->next;
  while(ptr != NULL){
    insertHashNode(htable, ptr->x, ptr->y);//fn to insert node in hash
    ptr = ptr->next;
  }
}
//creating snake with 5 nodes
struct snake* initSnake(struct snake *shead){
  int k = 5, a = 3, b = 4;
  while(k--){
    shead = insertNode(shead, a, b);
    a++;
  }
  return shead;
}
//taking usr option to start a new or saved game and returning it
int userOption(){
  printf(" 1 - To start a New Game\n");
  printf(" 2 - To start the last saved Game\n");
  int option;
  scanf("%d", &option);
  return option;
}
//initializing hashtable every count to zero and head to NULL
void initHashTable(struct hashtable *htable){
	int i;
	for(i = 0; i < (R+C); i++)
	{
		htable[i].count = 0;
		htable[i].head = NULL;
	}
}
//initializing matrix to make a board
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
  int score = 0;  //variable to store score,initially zero
  int level = 1;  //variable to store level,initially one
  FILE *fil;
  char matrix[R][C]; 
  initMatrix(matrix); //initializing matrix
  
  //creating an array object of structure hashtable
  struct hashtable *htable=(struct hashtable*)malloc
    ((R+C)*sizeof(struct hashtable));
  initHashTable(htable);  //initializing hashtable
  struct snake *shead = NULL; //creating head of snake
  struct food sfood;  //creating object of structure food
  
  //asking option from user to start new gane or saved game
  int option = userOption();
  system("cls"); //fn to clear the output screen
  
  if(option == 1)
  {
    shead = initSnake(shead); //create snake
    
    //making snake entry in the hashtable
    transferSnakeToHash(shead, htable);
    do{
       sfood = createFood(sfood); //generating random food
    }while(isNotValid(sfood, htable));//until it is a valid food
  }
  else
  {
  	fil = fopen("snake.txt", "r"); //opening saved file in read mode
  	if(fil != NULL){
        char str[2000];
        fscanf(fil, "%[^\n]s", str);
        fclose(fil);
        score = getSavedScore(str);
        sfood = getSavedFood(sfood, str);
        shead = getSavedSnake(shead, str);
        transferSnakeToHash(shead, htable);
        if(score >= 50)  
          level = 2;
    }
    else{
    //if(fil==NULL)i.e.unable to open the saved file then print & exit
		printf("You don't have any saved game\n");
        exit(0);
	}
  }
  transferFoodToMatrix(matrix, sfood);
  transferSnakeToMatrix(matrix, shead);
  
  while(1)
  {
    //for lev1 checking that snake should not collide with itself
    if(level==1 && !snakeCollision(htable, shead->x, shead->y))
	{
      printf("\n SCORE:%d\t\tLEVEL:%d\n", score, level);
	  displayMatrix(matrix);
    }
    //for lev2 snake should not collide neither itself nor boundary
    else if(level==2 && !snakeCollision(htable, shead->x, shead->y)&&
	  noBoundaryCollision(shead))
	{
      printf("\n SCORE:%d\t\tLEVEL:%d\n", score, level);
	  displayMatrix(matrix);
    }
    //if there is collision then end game
    else{
        printf("............\nOops! Collision\n............\n"); 
		printf("Final Score: %d\n", score);
        printf("............\nGame Ends\n............");
		break;
    }
  	printf(" PRESS s to save the game\n PRESS e to exit\n ");
    char direction;
    scanf(" %c", &direction);
    system("cls"); //fn to clear the output screen
    if(direction == 's')
    {
      savegame(shead, sfood, score, fil); //saving game to the file
      printf("............\nGAME SAVED\n............");
      break;
    }
    else if(direction == 'e'){
      printf("............\nYour Score: %d\n", score);	
	  printf("............\nYou Exit\n............");
      exit(0);
    }
    else{
      //moving snake according to the direction 
      shead =changeSnakeDirection(htable,shead,direction,matrix,sfood);
      transferSnakeToMatrix(matrix, shead);
      if(shead->x == sfood.x && shead->y == sfood.y)
      {
        score += 2; //if food is eaten incrementing score by 2
        do
        {
          sfood = createFood(sfood); //generating random food
        }while(isNotValid(sfood, htable));//until we get a valid food
        transferFoodToMatrix(matrix, sfood);
      }
    } 
    //if score passes 50,increment the level to 2
    if(score==50 && level==1)
        level++;
    else if(score == 100) //if score passes 100, game ends
	{
        printf("Final Score: 100\n.........\nYou Win\n...........");
        break;
    }
  }
  return 0;
}

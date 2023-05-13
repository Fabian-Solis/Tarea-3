#include "hashmap.h"
#include "heap.h"
#include "list.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX 31

typedef struct
{
  char nombre[MAX];
  int prioridad;
  HashMap *Tareas;
  List *TareasPorHacer;
  Stack *registro; // Una pila que registra las funciones del usuario para eliminarlas 
  bool realizado; // registra si la 

} Tarea; // Definimos nuestra estructura a trabajar.


// Subfunciones

void validarOpcion(int *opcion) // Valida las opciones del menú.
{
  char opcionAux[MAX];
  
  while (true) 
  {
    scanf("%30s", opcionAux);
    printf("\n");
      
    if (isdigit(*opcionAux)) 
    {
      *opcion = atoi(opcionAux);
        
      if (*opcion >= 1 && *opcion <= 7) break;
      else printf("Debe ingresar un número válido entre 1 y 7.\n");
    }
    else printf("Debe ingresar un número válido entre 1 y 7.\n");
    
    getchar();
  }
}

void menu() // Opciones del menú.
{
  printf("\nEliga una opción a continuación.\n\n");

  printf("1. Agregar nueva tarea.\n");
  printf("2. Establecer precedencia.\n");
  printf("3. Mostrar tareas pendientes.\n");
  printf("4. Marcar tarea completada.\n");
  printf("5. Deshacer última acción.\n");
  printf("6. Importar tareas.\n");
  printf("7. Salir del programa.\n\n");
}

void validar(int *user_continue) // Validamos que el usuario desee seguir con la ejecución del programa.
{
  char salida[2];
  
  printf("Desea realizar otra operación? (s/n)\n");
  scanf("%1s", salida);
  
  if(strcmp(salida, "n") == 0)
  {
    *user_continue = 0;
    printf("\nGracias por usar el programa, adiós!");
  }
  else while (strcmp(salida, "s") != 0)
  {
    printf("Ingrese una opción válida\n\n");
    printf("Desea realizar otra operación? (s/n)\n");
    scanf("%1s", salida);
  }
}

char *gets_csv_field(char *tmp, int k) //
{
  int open_mark = 0;
  char *ret = (char*) malloc(100 * sizeof(char));
  int ini_i = 0, i = 0;
  int j = 0;
  int last_field = 0;
  
  while (tmp[i + 1] != '\0')
  {  
    if(tmp[i] == '\"')
    {
      open_mark = 1 - open_mark;
      if(open_mark) ini_i = i + 1;
      i++;
      continue;
    }
    
    if(open_mark || tmp[i] != ',')
    {
      if(k == j) ret[i - ini_i] = tmp[i];
      i++;
      continue;
    }

    if(tmp[i] == ',')
    {
      if(k == j) 
      {
        ret[i - ini_i] = 0;
        return ret;
      }
      j++; 
      ini_i = i + 1;
    }
    i++;
  }

  if(k == j) 
  {
    last_field = 1;
    ret[i - ini_i] = 0;
    return ret;
  }

  if (last_field && k == j)
  {
    strcpy(ret + strlen(ret), tmp + ini_i);
    return ret;
  }
  
  return NULL;
}

void preguntarNombreTarea(char *nombreTarea) // Solicita que el usuario ingrese el nombre de la tarea
{
  printf("Ingrese el nombre de usuario de la tarea:\n");
  getchar();
  scanf("%30[^\n]s", nombreTarea);
}

// Funciones 

// 1.
void agregarTarea(HashMap *mapa, bool *registrada)
{
  Tarea *tareaAux = malloc(sizeof(Tarea));
  tareaAux -> Tareas = createMap(1000);
  tareaAux -> registro = stack_create();

  char nombreTarea[MAX]; 
  int prioridad;

  preguntarNombreTarea(nombreTarea);
  strcpy(tareaAux -> nombre, nombreTarea);

  printf("\nIngrese la prioridad de la tiene la tarea:\n");
  scanf("%d", &prioridad);

  tareaAux -> prioridad = prioridad;
  
  insertMap(mapa, tareaAux -> nombre, tareaAux);

  *registrada = true;
  stack_push(tareaAux -> registro, "1");
  
  printf("\nTarea registrada!!\n\n");
}

// 2.
void establecerPrecedencia(HashMap *mapa)
{
  Tarea *tareaAux;



  

  stack_push(tareaAux -> registro, "2");
}

// 3. 
void mostrarTareasPendientes(HashMap *mapa)
{  
  Pair *aux = firstMap(mapa);
  int cont = 1;

  while(aux != NULL)
  {
    Tarea *tareaAux = aux -> value;

    if(aux == NULL) aux = nextMap(mapa);
    else 
    {
      if(cont == 1)
      { 
        puts("~~~~~~~~~~~~~~~~~ TAREAS PENDIENTES ~~~~~~~~~~~~~~~~~\n");
      }
      
      printf("%d. %s (Prioridad: %d)\n\n", cont, tareaAux -> nombre, tareaAux -> prioridad);
      
    }
    
    aux = nextMap(mapa);
    cont++;
  }
  puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~o~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

// 4.
void eliminarTarea(HashMap *mapa)
{
  Tarea *tareaAux;




  
  stack_push(tareaAux -> registro, "4");
}

// 5.
void deshacerAccion(HashMap *mapa)
{
  Tarea *tareaAux;


  
}

// 6.
void importarTareas(HashMap *mapa)
{
  printf("Funcion 6\n\n");
}

// Programa principal
int main() 
{
  /*
                      ____ _             _           _             
                     / ___(_) __ _  __ _| |__  _   _| |_ ___     
                    | |  _| |/ _` |/ _` | '_ \| | | | __/ _ \     
                    | |_| | | (_| | (_| | |_) | |_| | ||  __/     
                     \____|_|\__, |\__,_|_.__/ \__, |\__\___| 
                             |___/             |___/                                                        
  */

  HashMap *mapa = createMap(10001);
  
  int user_continue = 1;

  bool registrada = false;

  printf("Bienvenido! :D\n");

  while(user_continue)
  {
    menu();
    
    int opcion = 0;
    
    validarOpcion(&opcion); // Validamos que opción sea un número.
    
    if(registrada == false && opcion != 1 && opcion != 6 && opcion != 7)
    {
      printf("No hay tareas registradas, debe registrar una primero.\n");
    }
    else
    switch(opcion)
    {
      case 1 :
        agregarTarea(mapa, &registrada);
        validar(&user_continue);
        break;

      case 2 :
        establecerPrecedencia(mapa);
        validar(&user_continue);
        break;

      case 3 :
        mostrarTareasPendientes(mapa);
        validar(&user_continue);
        break;

      case 4 :
        eliminarTarea(mapa);
        validar(&user_continue);
        break;

      case 5 :
        deshacerAccion(mapa);
        validar(&user_continue);
        break;

      case 6 :
        importarTareas(mapa);
        validar(&user_continue);
        break;
      
      case 7 :
        printf("Gracias por usar el programa, adiós!");
        return 0;
    }
    
  }
}
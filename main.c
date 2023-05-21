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

// Definimos nuestra estructura a trabajar.

typedef struct
{
  char nombre[MAX]; // Nombre de la tarea.
  int prioridad; // Prioridad de la tarea.
  int contList; // Largo de la lista de precedencias.
  bool precede; // Registra si la tarea tiene lista de precedencia.
  bool ingresadoHeap; // Registra si una tarea se ingreso al monticulo.
  bool restante; // Registra si tiene tareas precedentes por hacer.
  List *precedencia; // Conitene todas las tareas precedentes de una tarea.
  Stack *listaPrecedentes; // Para almacenar las tareas Precedentes a las cuales fue desasignido(especial para funcion 5)
} Tarea; 

typedef struct
{
  Stack *historial; // Stack de las funciones que va realizando el usuario.
  Stack *agregadas; // Contiene las tareas que fueron agregadas.
  Stack *eliminada; // Contiene las tareas que fueron eliminadas.
  Stack *precedencias; // Guarda de donde es precedente una tarea cuando es eliminada, para despues restaurarla.
} Pila;

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
        
      if (*opcion >= 0 && *opcion <= 6) break;
      else printf("Debe ingresar un número válido entre 0 y 6.\n");
    }
    else printf("Debe ingresar un número válido entre 0 y 6.\n");
    
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
  printf("0. Salir del programa.\n\n");
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
    printf("\nIngrese una opción válida\n\n");
    printf("Desea realizar otra operación? (s/n)\n");
    scanf("%1s", salida);
  }
}

char *gets_csv_field(char *tmp, int k) // Obtenemos la línea k del archivo.
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

  return NULL;
}

void poblarHeap(HashMap *mapaAux, Heap *pendientes) // Se ingresan las tareas que no tienen precedencia ni han sido agregadas al monituclo, y se ordenan por prioridad.
{
  Pair *aux = firstMap(mapaAux);
  
  while(aux != NULL)
  {
    Tarea *tareaAux = aux -> value;
    
    if(tareaAux -> restante == false && tareaAux -> ingresadoHeap == false) 
    {
      heap_push(pendientes, tareaAux -> nombre, tareaAux -> prioridad);
      tareaAux -> ingresadoHeap = true; 
    }
    
    aux = nextMap(mapaAux);
  }
}

void reiniciarBool(HashMap *mapaAux, Heap *pendientes) // Se reinician boleanos a su estado original.
{
  Pair *aux = firstMap(mapaAux);
  
  while(aux != NULL)
  {
    Tarea *tareaAux = aux -> value;
    tareaAux-> ingresadoHeap = false;
    tareaAux -> restante = true;
    aux = nextMap(mapaAux);
  }
}

HashMap* copiarMapa(HashMap* mapa) // Se realiza una copia del mapa original para usarse en la función 3.
{
  HashMap* mapaCopia = createMap(10001);
  Pair* aux = firstMap(mapa);
  while (aux != NULL)
  {
    void* clave = aux->key;
    Tarea* valor = aux->value;

    insertMap(mapaCopia, clave, valor);
    aux = nextMap(mapa);
  }
  return mapaCopia;
}

// Funciones 

// 1.
void agregarTarea(HashMap *mapa, Pila *stack, int *registrada) //Agrega Tareas.
{
  Tarea *tareaAux = malloc(sizeof(Tarea));
  tareaAux->precedencia = createList();
  tareaAux -> listaPrecedentes = stack_create();

  char nombreTarea[MAX];
  int prioridad;

  printf("Ingrese el nombre de la tarea:\n");
  getchar();
  scanf("%30[^\n]s", nombreTarea);
  
  strcpy(tareaAux -> nombre, nombreTarea);

  printf("\nIngrese la prioridad de la tarea:\n");
  scanf("%d", &prioridad);

  tareaAux -> prioridad = prioridad;
  
  insertMap(mapa, tareaAux -> nombre, tareaAux);
  Pair *par = (Pair*)searchMap(mapa, nombreTarea);

  (*registrada)++;
  tareaAux -> precede = false;
  tareaAux -> ingresadoHeap = false;
  tareaAux -> restante = true;

  stack_push(stack -> historial, "1");
  stack_push(stack -> agregadas, tareaAux);
  
  printf("\nTarea agregada!!\n\n");
}

// 2.
void establecerPrecedencia(HashMap *mapa, Pila *stack) // Establece precedencias entre dos Tareas, donde para hacer la tarea2 se tiene que hacer antes la tarea 1.
{
  char nombreTarea1[MAX],nombreTarea2[MAX];

  printf("Ingrese el nombre para establecer la precedecia.\n\n(Tarea Precedente): ");
  getchar();
  scanf("%30[^\n]s", nombreTarea1);
  getchar();
  printf("(Tarea Procedente): ");
  scanf("%30[^\n]s", nombreTarea2);

  if(searchMap(mapa, nombreTarea1) == NULL || searchMap(mapa, nombreTarea2) == NULL)
  {
    printf("\nUna o ambas tareas no han sido agregadas aún.\n\n");
    return;
  }
  
  Pair *tareaPrecedente = searchMap(mapa,nombreTarea1);
  Pair *tareaPorHacer = searchMap(mapa,nombreTarea2);
  Tarea *tareaAux = tareaPrecedente->value;
  Tarea *tareaAux2 = tareaPorHacer->value;

  tareaAux2 -> precede = true;
  pushFront(tareaAux2->precedencia,tareaAux);

  stack_push(stack -> precedencias, tareaAux -> nombre);
  stack_push(stack -> precedencias, tareaAux2 -> nombre);
  stack_push(stack -> historial, "2");
  
  printf("\nPrecedencia establecida!!\n\n");
  
  return;
}

// 3. 
void mostrarTareasPendientes(HashMap *mapa, Heap *pendientes) // Mostramos por pantalla un orden para realizar las tareas por hacer considerando su prioridad y su lista de precedencia.
{ 
  HashMap *mapaAux = createMap(10001);
  mapaAux = copyHashMap(mapa);
  Pair *par = firstMap(mapaAux);
  
  int cont = 1;
  
  while (par != NULL) 
  {
    Tarea *mapAux = par -> value;

    mapAux -> contList = getLength(mapAux -> precedencia);
    if(firstList(mapAux -> precedencia) == NULL) mapAux -> restante = false; 
    
    par = nextMap(mapaAux);
  }

  puts("~~~~~~~~~~~~~~~~~~ TAREAS PENDIENTES ~~~~~~~~~~~~~~~~~~\n");
  while (true)
  {
    poblarHeap(mapaAux, pendientes);
    
    if(heap_size(pendientes) == 0) 
    {
      puts("");
      reiniciarBool(mapa,pendientes);
      puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~o~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      return;
    }
    
    Pair *raizHeap = searchMap(mapaAux, heap_top(pendientes));
    Tarea *tareaAux = raizHeap -> value;
    char *nombreAux = tareaAux ->nombre;

    printf("%d. %s (Prioridad: %d)",cont, tareaAux -> nombre, tareaAux -> prioridad); 
    if(tareaAux -> precede == true)
    {
      char *nombreLista = (char*)firstList(tareaAux -> precedencia);
      int cont = 0;
      
      printf(" - Precedente(s):"); 
      while(nombreLista != NULL)
      {
        if(cont != 0) printf(",");
        printf(" %s", nombreLista);
        
        nombreLista = nextList(tareaAux -> precedencia);
        cont++;
      }
    }
    puts("");

    
    eraseMap(mapaAux, tareaAux -> nombre);
    heap_pop(pendientes);

    
    Pair *parAux = firstMap(mapaAux);
    
    while(parAux != NULL)
    {
      tareaAux = parAux -> value;
      char *nombreLista = firstList(tareaAux -> precedencia);

      while(nombreLista != NULL)
      {
        if(strcmp(nombreAux, nombreLista) == 0) 
        {
          tareaAux -> contList--;
        }
        nombreLista = nextList(tareaAux -> precedencia);
      }
      if(tareaAux -> contList == 0) tareaAux-> restante = false;
      parAux = nextMap(mapaAux);
    }
    cont++;
  }
}

// 4.
void marcarTarea(HashMap *mapa, Pila *stack , int *registrada) // Eliminamos la tarea del la lista de tareas pendientes, considerando si esta en la lita de precedencia de otra tarea.
{
  char nombreTarea[MAX], respuesta[MAX];
  printf("Ingrese el nombre de la tarea a eliminar: \n");
  getchar();
  scanf("%30[^\n]s", nombreTarea);

  if(searchMap(mapa, nombreTarea) == NULL)
  {
    printf("\nNo se ha encontrado la tarea.\n\n");
    return;
  }
  
  Pair *aux = searchMap(mapa,nombreTarea);
  Tarea *tareaEliminar = aux->value;
  
  if(tareaEliminar->precede == true)
  {
    printf("\n¿Está seguro que desea eliminar la tarea? (s/n)\n");
    getchar();
    scanf("%c",respuesta);
    if(strcmp(respuesta, "n") == 0) return;
  }
  
  eraseMap(mapa,nombreTarea);
  Pair *mapaTareas = (Pair*)firstMap(mapa);
  
  while(mapaTareas!= NULL)
  {
    Tarea *tareaAux = (Tarea*) mapaTareas->value;
    char *nombreLista = (char*)firstList(tareaAux->precedencia);
    while(nombreLista != NULL)
    {
      if(strcmp(nombreLista,nombreTarea) == 0)
      { 
        stack_push(tareaEliminar -> listaPrecedentes, tareaAux -> nombre); 
        popCurrent(tareaAux->precedencia);
      }
      nombreLista = nextList(tareaAux -> precedencia);
    }
    if(firstList(tareaAux -> precedencia) == NULL) tareaAux -> precede = false;
    mapaTareas = nextMap(mapa);
  }
  
  printf("\nLa tarea ha sido marcada como completada y eliminada!!\n\n");
  
  (*registrada)--;

  stack_push(stack -> eliminada, tareaEliminar);
  stack_push(stack -> historial, "4");
}

// 5.
void deshacerAccion(HashMap *mapa, Pila *stack, int *registrada) // Deshacemos la ultima acción que hizo el usuario.
{ 
  char *datoRegistro = (char*)stack_pop(stack -> historial);
  
  if(strcmp(datoRegistro, "0") == 0)
  {
    printf("No se pueden deshacer más acciones. \n\n");
    stack_push(stack -> historial, "0");
  }
  else 
  if(strcmp(datoRegistro, "1") == 0) // Deshacer función 1.
  {
    Tarea *tareaAux = stack_pop(stack -> agregadas);

    printf("La última acción fue deshecha y la tarea ' %s ' fue eliminada!!\n\n", tareaAux -> nombre);
    
    eraseMap(mapa, tareaAux -> nombre);
    (*registrada)--;
  }
  else 
  if(strcmp(datoRegistro, "2") == 0) //Deshacer función 2.
  {
    char *nombreTareaProcedente = stack_pop(stack -> precedencias);
    char *nombreTareaPrecedente = stack_pop(stack -> precedencias);

    Pair *aux = searchMap(mapa, nombreTareaProcedente);
    Tarea *tareaAux = aux -> value;

    char *nombreLista = firstList(tareaAux -> precedencia);

    while(nombreLista != NULL)
    {
      if(strcmp(nombreLista, nombreTareaPrecedente) == 0) popCurrent(tareaAux -> precedencia);
      nombreLista = nextList(tareaAux -> precedencia);
    }
    if(firstList(tareaAux -> precedencia) == NULL) tareaAux -> precede = false;
    printf("La última acción fue deshecha y el precedente ' %s ' de la tarea ' %s ' fue eliminado!!\n\n", nombreTareaPrecedente, nombreTareaProcedente);
  }
  else 
  if(strcmp(datoRegistro, "4") == 0) // Deshacer función 4.
  {
    Tarea *tareaMapa = stack_pop(stack -> eliminada);
    insertMap(mapa, tareaMapa -> nombre, tareaMapa);

    char *nombreTarea = stack_pop(tareaMapa -> listaPrecedentes);
    
    while(nombreTarea != NULL)
    {
      Pair *aux = searchMap(mapa, nombreTarea);
      Tarea *tareaAux = (Tarea*)aux -> value;
      
      pushBack(tareaAux -> precedencia, tareaMapa -> nombre);
      tareaAux -> precede = true;
      
      nombreTarea = stack_pop(tareaMapa -> listaPrecedentes);
    }
    
    stack_push(stack -> agregadas, tareaMapa);
    (*registrada)++;
    printf("La última acción fue deshecha y la tarea ' %s ' fue agregada!!\n\n", tareaMapa -> nombre);
  }
}

// 6.
void importarTareas(HashMap* mapa, int *registrada) // Importamos el archivo que desee el usuario.
{
  char nombreArchivo[MAX];
  
  printf("Ingrese el nombre del archivo a importar:\n");
  scanf("%30s", nombreArchivo);
  
  FILE *fp = fopen(nombreArchivo, "r");
  
  if (fp == NULL)
  {
    printf("\nNo hay ningún archivo con ese nombre.\n\n");
    return;
  }
  
  char delimit[] = " \t\r\n\v\f";
  char linea[1024];
  fgets(linea, 1024, fp);

  while (fgets(linea, 1024, fp) != NULL) 
  {
    Tarea* nuevaTarea = (Tarea*)malloc(sizeof(Tarea));
    nuevaTarea->precedencia = createList();
    nuevaTarea -> listaPrecedentes = stack_create();
    char* precedente = "";

    for (int i = 0; i < 3; i++) 
    {
      char* aux = gets_csv_field(linea, i);
      
      if (i == 0) 
      {
        strcpy(nuevaTarea->nombre, aux);
        nuevaTarea -> ingresadoHeap = false;
        nuevaTarea -> restante = true;
        (*registrada)++;
      }
      if (i == 1) 
      {
        int prioridad = atol(aux);
        nuevaTarea->prioridad = prioridad;
      }
      if (i == 2) 
      {
        if (strlen(aux) > 1) 
        {
          precedente = strtok(aux, delimit); // tarea2
          nuevaTarea->precede = true;
          while (precedente != NULL) 
          {
            Tarea* tareaPrecedente = (Tarea*)searchMap(mapa, precedente)->value;
            if (tareaPrecedente != NULL) pushFront(nuevaTarea->precedencia, tareaPrecedente);
            
            precedente = strtok(NULL, delimit);
          }
        }
        else nuevaTarea->precede = false;
      }
    }
    insertMap(mapa, nuevaTarea->nombre, nuevaTarea);
  }

  printf("\nEl archivo ' %s ' fue importado!!\n\n", nombreArchivo);
  fclose(fp);
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
  

  Heap *pendientes = createHeap();

  Pila *stack = malloc(sizeof(Pila));
  stack -> historial = stack_create();
  stack_push(stack -> historial, "0");
  stack -> agregadas = stack_create();
  stack -> precedencias = stack_create();
  stack -> eliminada = stack_create();
  
  
  int user_continue = 1;

  int registrada = 0;

  printf("\nBienvenido! :D\n");

  while(user_continue)
  {
    menu();
    
    int opcion = 0;
    
    validarOpcion(&opcion);
    
    if(registrada == 0 && opcion != 1 && opcion != 6 && opcion != 0 && opcion != 2 && opcion != 5)
    {
      printf("No hay tareas agregadas, debe registrar una primero.\n");
    }
    else
    switch(opcion)
    {
      case 1 :
        agregarTarea(mapa, stack, &registrada);
        validar(&user_continue);
        break;

      case 2 :
        if(registrada < 2) 
        {
          printf("Debe tener al menos dos tareas agregadas para establecer una precedencia.\n");
          break;
        }
        else
        {
          establecerPrecedencia(mapa,stack);
          validar(&user_continue);
          break;
        }
          
      case 3 :
        mostrarTareasPendientes(mapa, pendientes);        
        validar(&user_continue);
        break;

      case 4 :
        marcarTarea(mapa, stack, &registrada);
        validar(&user_continue);
        break;

      case 5 :
        deshacerAccion(mapa, stack, &registrada);
        validar(&user_continue);
        break;

      case 6 :
        importarTareas(mapa, &registrada);
        validar(&user_continue);
        break;
      
      case 0 :
        printf("Gracias por usar el programa, adiós!");
        return 0;
    }
    
  }
}
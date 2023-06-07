#define  _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double get_eclidean_dist();
void find_kmeans();
double update_clusters_and_delta();
void initLayer();
void print();
double * zeros();
double ** zeros_2d();
double max();
double * add();
void free_2d();
void free_all();
double **matrix;
double **k_array;
double **sum_for_cluster;
double *delta_k;
double *elem_in_cluster;
int vectors_num = 0;
int dim = 0;
int K=0;
double EPS = 0.001;

void generalError(){
  printf("An Error Has Occurred\n");
  free_all();
  exit(1);
  }

void free_all(){
  if (matrix != NULL){
    free_2d(matrix, vectors_num);
  }
  if (k_array != NULL){
    free_2d(k_array, K);
  }
  if (sum_for_cluster != NULL){
    free_2d(sum_for_cluster, K);
  }
  if (elem_in_cluster != NULL){
    free(elem_in_cluster);
  }
  if (delta_k != NULL){
    free(delta_k);
  }
}

void free_2d(double **ptr, int N){
  int i;
  double* currDoublePtr;
  for(i = 0; i < N; i++){
    currDoublePtr = ptr[i];
    free(currDoublePtr);
  }
  free(ptr);
}


double update_clusters_and_delta(double * delta_k, double ** sum_for_cluster, double * elem_in_cluster, double ** k_array){
    double *new_cluster;
    int i, j;
    for (i=0; i<K; i++){
        new_cluster = zeros(dim);
        for (j=0; j<dim; j++){
            new_cluster[j] = sum_for_cluster[i][j] / elem_in_cluster[i];
        }
        delta_k[i] = get_eclidean_dist(new_cluster, k_array[i]);
        free(k_array[i]);
        k_array[i] = new_cluster;
    }
    return max(delta_k, K);
}

double get_eclidean_dist(double * vector1, double * vector2){
    double sum = 0;
    int i;
    for (i=0; i<dim; i++){
        sum += pow((vector1[i] - vector2[i]),2);
    }
    return sqrt(sum);
}

double max(double * vector, int vec_len){
    double max_num = vector[0];
    int i;
    for (i=0; i < vec_len; i++){
        if (vector[i] > max_num){
            max_num = vector[i];
        }
    }
    return max_num;
}

double * add(double * vector1, double * vector2) {
    int p;
    double * res = zeros(dim);
    for (p=0; p<dim; p++){
        res[p] = vector1[p] + vector2[p];
    }
    return res;
}

double * zeros(int n){
    double * pointer;
    int i;
    pointer = (double *)calloc(n, sizeof(double));
    if (pointer == NULL){
      generalError();
    }
    for (i=0; i<n; i++){
        pointer[i] = 0.0;
    }
    return pointer;
}


void print(double **arr, int m, int n){
    int i, j;
    for (i = 0; i < m; i++){
      for (j = 0; j < n; j++){
        if (j == n - 1){
          printf("%.4f\n", arr[i][j]);
        }
        else{
          printf("%.4f,", arr[i][j]);
        }
      }
    }
    printf("\n");
}

void find_kmeans(int K, int iter, double **vectors, int dim){
  double min_dist, dist;
  int min_elem;
  double * tmp_vector;
  double max_delta = 0.02;
  int iteration_number = 0;
  int i, j;
  delta_k = zeros(K);
  elem_in_cluster = zeros(K);
  initLayer(&k_array, K, dim);
  initLayer(&sum_for_cluster, K, dim);
  for (i = 0; i < K; i++)
  {
    for (j = 0; j < dim; j++)
    {
      k_array[i][j] = vectors[i][j];
    }
  }

  do
  {
    iteration_number++;
    for (i = 0; i < K; i++)
    {
      delta_k[i] = 0.0;
      elem_in_cluster[i] = 0.0;
      for (j = 0; j < dim; j++)
      {
        sum_for_cluster[i][j] = 0.0;
      }
      
    }
    
    
    for (i=0; i<vectors_num; i++){
      min_dist = get_eclidean_dist(vectors[i], k_array[0]);
      min_elem = 0;
      for (j = 0; j < K; j++){
        dist = get_eclidean_dist(vectors[i], k_array[j]);
        if (dist < min_dist){
          min_dist = dist;
          min_elem = j;
        }
      }
      tmp_vector = sum_for_cluster[min_elem];
      sum_for_cluster[min_elem] = add(vectors[i], sum_for_cluster[min_elem]);
      free(tmp_vector);
      elem_in_cluster[min_elem]++;
    }
    max_delta = update_clusters_and_delta(delta_k, sum_for_cluster, elem_in_cluster, k_array);

  } while (!(max_delta < EPS || iteration_number == iter));
  
  print(k_array, K, dim);
  
}

void initLayer(double ***layer, int rows, int cols)
{
    int i;
    *layer = malloc(sizeof(double *) * rows);
    if (*layer == NULL){
      generalError();
    }

    for (i = 0; i < rows; i++)
    {
        *(*layer + i) = malloc(sizeof(double) * cols);
        if (*(*layer + i) == NULL){
          generalError();
          }
    }
}

struct cord
{
    double value;
    struct cord *next;
};
struct vector
{
    struct vector *next;
    struct cord *cords;
};

void freeLinkedList(struct vector * head_vec){
  struct cord * head_cord, *tmp_cord;
  struct vector * tmp_vector;
  
  while (head_vec != NULL) {
    head_cord = head_vec->cords;
    while (head_cord != NULL){
        tmp_cord = head_cord;
        head_cord = head_cord->next;
        free(tmp_cord);
        }
    tmp_vector = head_vec;
    head_vec = head_vec->next;
    free(tmp_vector);
  }
}

int main(int argc, char* argv[]){
    struct vector *head_vec, *curr_vec, *tmp_vector, *prev_vec;
    struct cord *head_cord, *curr_cord, *tmp_cord;
    double n;
    char c;
    int i = 0, j = 0;
    int iter = 200;
    int counted_dim=0;
    int error = 0, errorK = 0, errorIter = 0;

    if (argc == 3) {
      if (atoi (argv[2]) != atof(argv[2])){
        errorIter = 1;
      } 
      K = atoi (argv[1]);
      iter =  atoi (argv[2]);
      } 
    else if (argc == 2)
    {
      K = atoi (argv[1]);
    }
    else{
      generalError();
      }
    if (atoi (argv[1]) != atof(argv[1])){
        errorK = 1;
      } 

    head_cord = malloc(sizeof(struct cord));
    if (head_cord == NULL){
      generalError();
    }
    curr_cord = head_cord;
    curr_cord->next = NULL;

    head_vec = malloc(sizeof(struct vector));
    if (head_vec == NULL){
      generalError();
    }
    curr_vec = head_vec;
    curr_vec->next = NULL;

   while (scanf("%lf%c", &n, &c) == 2) {
    if (counted_dim == 0){
                dim++;
            }
        if (c == '\n'){
            curr_cord->value = n;
            curr_vec->cords = head_cord;
            curr_vec->next = malloc(sizeof(struct vector));
            if (curr_vec->next == NULL){
              generalError();}
            prev_vec = curr_vec;
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_cord = malloc(sizeof(struct cord));
            if (head_cord == NULL){
               generalError();}
            curr_cord = head_cord;
            curr_cord->next = NULL;

            vectors_num++;            
            counted_dim = 1;
            continue;
        }
        
        curr_cord->value = n;
        curr_cord->next = malloc(sizeof(struct cord));
        if (curr_cord->next == NULL){
          generalError();}
        curr_cord = curr_cord->next;
        curr_cord->next = NULL;
    }
    prev_vec->next = NULL;
    free(curr_vec);
    free(curr_cord);
  
  if (K <= 1 || K >= vectors_num || errorK) {
      error = 1;
      printf("Invalid number of clusters!\n");
    }
  if (iter <= 1 || iter >= 1000 || errorIter) {
      error = 1;
      printf("Invalid maximum iteration!\n");
    }
  if (error){
    freeLinkedList(head_vec);
    free_all();
    exit(1);
  }


  initLayer(&matrix, vectors_num, dim);

  while (head_vec != NULL) {
    head_cord = head_vec->cords;
    j = 0;
    while (head_cord != NULL){
        matrix[i][j] = head_cord->value;
        tmp_cord = head_cord;
        head_cord = head_cord->next;
        free(tmp_cord);
        j++;
        }
    tmp_vector = head_vec;
    head_vec = head_vec->next;
    free(tmp_vector);
    i++;
    }
  
  find_kmeans(K, iter, matrix, dim);
  free_all();
  return 0;
}
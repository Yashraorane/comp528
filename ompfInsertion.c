// Include the necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// Declare function prototypes
int *farthest_insertion(double **coordinates, int number_of_coordinates);
double **readCoords(char *filename, int numOfCoords);
int readNumOfCoords(char *fileName);
double **distance_matrix_calculate(double **coordinates, int number_of_coordinates);
void *writeTourToFile(int *tour, int tourlength, char *filename);


// Define the DBL_MAX constant
double DBL_MAX = 1.7976931348623157e+308;
double DBL_MIN = -1.7976931348623157e+308;

int main(int arg, char *arr[]) {
  //Fetching the coord file and reading coordinates
  char *input_filename = arr[1];
  int number_of_coordinates = readNumOfCoords(input_filename);
  double **points = readCoords(input_filename, number_of_coordinates);

  //Start timer
  double start_time = omp_get_wtime();

  //Calculation of distance matrix
  double **dist_matrix = distance_matrix_calculate(points, number_of_coordinates);
  
  //Calling farthest tour function
  int *partial_tour = farthest_insertion(dist_matrix, number_of_coordinates);
  
  //End Timer
  double end_time = omp_get_wtime();

  //Print the time
  printf("Exceution time %f seconds\n", end_time-start_time);

  //Printing tour in output.txt file
  char *output_filename = arr[2];
  writeTourToFile(partial_tour, number_of_coordinates + 1, output_filename);

 //Freeing up the memory allocated
  free(dist_matrix);
  free(partial_tour);

  return 0;
}

//With the coordinates we will use distance formula to find distance and store it in matrix 
double **distance_matrix_calculate(double **points, int number_of_coordinates) {
  //We will use malloc function allocating memory in our matrix
  double **dist_matrix = malloc(sizeof(double *) * number_of_coordinates);
  //parallelising dist calculation
  #pragma omp parallel for
  for (int i = 0; i < number_of_coordinates; i++) {
    dist_matrix[i] = malloc(sizeof(double) * number_of_coordinates);

    for (int j = 0; j < number_of_coordinates; j++) {
      if(i==j){
        dist_matrix[i][j]=0.0;
      } else {
        double x = points[i][0] - points[j][0];
        double y = points[i][1] - points[j][1];
        dist_matrix[i][j]=sqrt(x*x+y*y);
      }
    }
  }return dist_matrix;
}
// Finding farthest insertion tour
int* farthest_insertion(double** dist_matrix, int number_of_coordinates) {
    //We will use malloc function allocating memory in our one-d array
    int* partial_tour = (int*)malloc((number_of_coordinates +1) * sizeof(int));

    //using calloc function to allocating memory at starting point
    int* visited = (int*)calloc(number_of_coordinates, sizeof(int));
    if(!partial_tour || !visited){
        // Free memory allocation, getting segmentation error at tour
        free(partial_tour);
        free(visited);
        return NULL;
    }
    visited[0]=1;
    double max_dist = DBL_MIN;
    int nearest_vertex = -1;
    //since tour will start from zero, setting zero at all index at intial phase
    for (int i = 0; i < number_of_coordinates; i++) {
        partial_tour[i] = 0;
    }
    //finding first max cost from zero
    for(int vertex = 1; vertex<number_of_coordinates;vertex++){
        double insert_distance = dist_matrix[0][vertex];
        //comparing distance of vertex th position
        //insert_distance calculated at each iteration is getting check if true then at vertex th position is updated in nearest and max_dist is now having new value
        if(insert_distance>max_dist){
            nearest_vertex=vertex;
            max_dist=insert_distance;
        }
    }

    //placing new nearest city in partial_tour[] and changing flag to 1 in visited[]
    partial_tour[1]=nearest_vertex;
    visited[nearest_vertex]=1; 
for(int index =2;index<number_of_coordinates;index++){
    double min_dist = DBL_MAX;
    int nearest_vertex = -1;
    int insertion_position = 0;
    max_dist=DBL_MIN;
    int max_vertex = -1;

    #pragma omp parallel
    {
        double local_max_dist = DBL_MIN;
        int local_max_vertex = -1;

        #pragma omp for nowait
        for (int vertex = 1; vertex < number_of_coordinates; vertex++)
        {
            if(!visited[vertex]){
                for (int i = 0; i < index; i++)
                {
                    double current_distance = dist_matrix[partial_tour[i]][vertex];
                    //we are finding out the cost for each vertex between two index of partial tour[]
                    //insert_distance calculated at each iteration is getting check if true then at vertex th position is updated and 
                    //max current_distance is now having new value
                    if(current_distance>local_max_dist){
                        local_max_dist=current_distance;
                        local_max_vertex=vertex;
                    }
                }
                
            }
        }
      //using critcal , also substituting private variables to global to reduce effort
      #pragma omp critical
      {
        if(local_max_dist>max_dist){
            max_dist=local_max_dist;
            max_vertex=local_max_vertex;
        }
      }  
    }
    for (int i = 0; i < number_of_coordinates-1; i++)
    {
        double insert_distance = dist_matrix[partial_tour[i]][max_vertex]+
                                 dist_matrix[max_vertex][partial_tour[i+1]]-
                                 dist_matrix[partial_tour[i]][partial_tour[i+1]];
        if (insert_distance<min_dist){
            nearest_vertex=max_vertex;
            insertion_position=i;
            min_dist=insert_distance;
        }
    }
    //changing flag to 1 in visited[]
    visited[nearest_vertex]=1;
    //shift the old cities by 1 position so that new city can be added
    for (int i = number_of_coordinates; i > insertion_position+1; i--)
    {
        partial_tour[i]=partial_tour[i-1];
    }
    // Insert the new farthest city at the correct position
    partial_tour[insertion_position+1]=nearest_vertex;
}
return partial_tour;


}

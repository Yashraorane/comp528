// Include the necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

//Pre-Declaring functions
int *cheapest_insertion(double **coordinates, int number_of_coordinates);
double **readCoords(char *filename, int numOfCoords);
int readNumOfCoords(char *fileName);
double **distance_matrix_calculate(double **coordinates, int number_of_coordinates);
void *writeTourToFile(int *tour, int tourlength, char *filename);

//Define the DBL_MAX constant which can be use as maximum floating value
double DBL_MAX = 1.7976931348623157e+308;

int main(int arg, char *arr[]) {
    //Fetching the coord file and reading coordinates
    char *input_filename = arr[1];
    int number_of_coordinates = readNumOfCoords(input_filename);
    double **points = readCoords(input_filename, number_of_coordinates);

    //Start timer
    double start_time = omp_get_wtime();

    //Calculation of distance matrix
    double **dist_matrix = distance_matrix_calculate(points,number_of_coordinates);
    //Calling cheapest tour function
    int *path = cheapest_insertion(dist_matrix, number_of_coordinates);
  
    //End Timer
    double end_time = omp_get_wtime();

    //Print the time
    printf("Exceution time %f seconds\n", end_time-start_time);
    
    //Printing tour in output.txt file
    char *output_filename = arr[2];
    writeTourToFile(path, number_of_coordinates + 1, output_filename);

    //Freeing up the memory allocated
    free(dist_matrix);
    free(path);

  return 0;
}


//With the coordinates we will use distance formula to find distance and store it in matrix 
double **distance_matrix_calculate(double **points, int number_of_coordinates){
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
           double x = points[i][0]-points[j][0];
           double y = points[i][1]-points[j][1];
        dist_matrix[i][j]=sqrt(x*x+y*y);
      }
    }
  }return dist_matrix;
}



//finding cheapest insertion tour
int* cheapest_insertion(double** dist_matrix, int number_of_coordinates) {
    //We will use malloc function allocating memory in our one-d array
    int* path = (int*)malloc((number_of_coordinates + 1) * sizeof(int));
    //using calloc function to allocating memory at starting point
    int* cityvisited = (int*)calloc(number_of_coordinates, sizeof(int));
    //setting flag 1 since its starting point
    cityvisited[0] = 1;
    double min_dist = DBL_MAX;
    int nearest = -1;

    //since tour will start from zero, setting zero at all index at intial phase
    for (int a = 0; a < number_of_coordinates; a++) {
        path[a] = 0;
    }
    //finding first minimum cost from zero
    for (int v = 1; v < number_of_coordinates; v++) {
        double insert_distance = dist_matrix[0][v];
        //comparing distance of vth position
        //insert_distance calculated at each iteration is getting check if true then at vth position is updated in nearest and min_dist is now having new value
        if (insert_distance < min_dist) {
            nearest = v;
            min_dist = insert_distance;
        }
    }
    //placing new nearest city in path[] and changing flag to 1 in cityvisited[] 
    path[1] = nearest;
    cityvisited[nearest] = 1;
 
    for (int index = 1; index < number_of_coordinates - 1; index++) {
        //reinitialising min distance for rest of the traversing
        min_dist = DBL_MAX;
        int insertion_position = 0;
        //comparing distance for cities not visited by using cordinates added in path[]
        //parallelising minimun distance finding between elements of tour
        #pragma omp parallel
        {  
            //reinitialising local min distance for rest of the traversing
            double local_min_dist = DBL_MAX;
            int local_nearest = -1, local_insertion_position = 0;
 
            #pragma omp for nowait
            for (int i = 0; i <= index; i++) {
                for (int v = 1; v < number_of_coordinates; v++) {
                    if (!cityvisited[v]) {
                        double local_insert_distance = dist_matrix[path[i]][v] + dist_matrix[v][path[i + 1]] - dist_matrix[path[i]][path[i + 1]];
                        //we are finding out the cost for each vertex between two index of path[]
                        //insert_distance calculated at each iteration is getting check if true then at vth position is updated in nearest and 
                        //min_dist is now having new value
                        if (local_insert_distance < local_min_dist) {
                            local_nearest = v;
                            local_insertion_position = i;
                            local_min_dist = local_insert_distance;
                        }
                    }
                }
            }
            //using critcal , also substituting private variables to global to reduce effort
            #pragma omp critical
            {
                if (local_min_dist < min_dist) {
                    nearest = local_nearest;
                    insertion_position = local_insertion_position;
                    min_dist = local_min_dist;
                }
            }
        }
        //changing flag to 1 in cityvisited[]
        cityvisited[nearest] = 1;

        //shift the old cities by 1 position so that new city can be added
        for (int i = index + 1; i > insertion_position; i--) {
        path[i] = path[i - 1];
    }
 
    // Insert the new nearest city at the correct position
    path[insertion_position + 1] = nearest;
    }
 
    return path;
}


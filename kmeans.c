# include <stdio.h>
# include <stdlib.h>

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
struct data_point_index
{
    double *data_point;
    int index;
};
struct cluster
{
    double *centroid;
    double *sum_vec;
    int num_of_dp;
};



typedef struct vector vector;
typedef struct cord cord;
typedef struct data_point_index data_point_index;
typedef struct cluster cluster;

vector* read_file(char*,int);
data_point_index* from_file_to_arr_of_dp(int,int,vector*);
cluster* from_file_to_arr_of_clusters(data_point_index*,int,int);

data_point_index *arr_of_data_points;
cluster* arr_of_clusters;



vector* read_file(char *argv, int k)
{
    struct vector *head_vec, *curr_vec, *next_vec;
    struct cord *head_cord, *curr_cord, *next_cord;
    int rows = 0, cols = 0;
    double n;
    char c;

    head_cord = malloc(sizeof(struct cord));
    curr_cord = head_cord;
    curr_cord->next = NULL;

    head_vec = malloc(sizeof(struct vector));
    curr_vec = head_vec;
    curr_vec->next = NULL;

    while (scanf("%lf%c", &n, &c) == 2)  // valid read
    {
        cols++;
        if (c == '\n')  // if c , 
        {
            // init new row
            curr_cord->value = n;
            curr_vec->cords = head_cord;
            curr_vec->next = malloc(sizeof(struct vector));
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_cord = malloc(sizeof(struct cord));
            curr_cord = head_cord;
            curr_cord->next = NULL;
            rows++;
            continue;
        }
        // in same line in file
        
        curr_cord->value = n;
        curr_cord->next = malloc(sizeof(struct cord));
        curr_cord = curr_cord->next;
        curr_cord->next = NULL;
    }
    cols = cols / rows;
    arr_of_data_points = from_file_to_arr_of_dp(rows,cols,head_vec);
    arr_of_clusters = from_file_to_arr_of_clusters(arr_of_data_points,k, cols);
    return head_vec;
}

data_point_index* from_file_to_arr_of_dp(int rows,int cols,vector* head_vec){
    struct vector *curr_vec, *next_vec;
    struct cord *curr_cord, *next_cord;
    data_point_index *data_points_arr = calloc(rows,sizeof(data_point_index));
    curr_vec = head_vec;
    for(int i = 0 ; i < rows ; ++i){
        curr_cord = curr_vec->cords;
        data_points_arr[i].data_point = calloc(cols,sizeof(double));
        data_points_arr[i].index = -1;
        //printf("i=%d\n",i);
        //printf("i=%d\n",data_points_arr[i].index);
        for(int j = 0 ; j < cols ; ++j){
            //printf("j=%d\n",j);
            data_points_arr[i].data_point[j] = curr_cord->value;
            //printf("%f\n",data_points_arr[i].data_point[j]);
            curr_cord = curr_cord -> next;
        }
        curr_vec = curr_vec->next;
    }
    return data_points_arr;
}

cluster* from_file_to_arr_of_clusters(data_point_index* arr_of_data_points, int k, int cols){
    cluster* curr_cluster;
    cluster* clusters_arr = calloc(k,sizeof(cluster));
    for(int i = 0 ; i < k ; i++){
        curr_cluster = &clusters_arr[i];
        curr_cluster->num_of_dp = 1;
        curr_cluster->centroid = calloc(cols,sizeof(double));
        curr_cluster->sum_vec = calloc(cols,sizeof(double));


        //copy data point
        for(int j = 0 ; j < cols ; j++){
            curr_cluster->centroid[j] = arr_of_data_points[i].data_point[j];
            curr_cluster->sum_vec[j] = arr_of_data_points[i].data_point[j];
        }

    }
    return clusters_arr;

}


int main(int argc, char* argv[]){
    //TODO: change the args order
    read_file(argv[2], atoi(argv[1]));
    return 0;
}
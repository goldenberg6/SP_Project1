# include <stdio.h>
# include <stdlib.h>
# include <math.h>

struct cord {
  double value;
  struct cord * next;
};
struct vector {
  struct vector * next;
  struct cord * cords;
};
struct data_point_index {
  double * data_point;
  int index;
};
struct cluster {
  double * centroid;
  double * sum_vec;
  int num_of_dp;
};

typedef struct vector vector;
typedef struct cord cord;
typedef struct data_point_index data_point_index;
typedef struct cluster cluster;

vector * read_file_dynamically();
int calc_rows(vector * );
int calc_cols(vector * );
data_point_index * from_file_to_arr_of_dp(int, int, vector * );
cluster * from_file_to_arr_of_clusters(data_point_index * , int, int);
void kmeans(int, int, int, int, data_point_index * , cluster * );
void assign_dp_to_closest_cluster(int, int, int, data_point_index * , cluster * );
void calc_new_centroid(int, double * , int, cluster * );
double calc_dist_centroid_centroid(int, double * , int , cluster * );
int calc_closest_cluster(int, int, int, data_point_index * , cluster * );
void compute_new_sum(int, int, char, int, data_point_index * , cluster * );
double calc_dist_dp_centroid(int, int, int, data_point_index * , cluster * );
void printres(int, int, cluster * );
int is_integer(char *);

vector * read_file_dynamically() {
  vector * head_vec, * curr_vec;
  cord * head_cord, * curr_cord;
  double n;
  char c;

  head_cord = malloc(sizeof(struct cord));
  curr_cord = head_cord;
  curr_cord -> next = NULL;

  head_vec = malloc(sizeof(struct vector));
  curr_vec = head_vec;
  curr_vec -> next = NULL;

  while (scanf("%lf%c", & n, & c) == 2) {
    if (c == '\n') {

      curr_cord -> value = n;
      curr_vec -> cords = head_cord;
      curr_vec -> next = malloc(sizeof(struct vector));
      curr_vec = curr_vec -> next;
      curr_vec -> next = NULL;
      head_cord = malloc(sizeof(struct cord));
      curr_cord = head_cord;
      curr_cord -> next = NULL;
      continue;
    }
    /* in same line in file */

    curr_cord -> value = n;
    curr_cord -> next = malloc(sizeof(struct cord));
    curr_cord = curr_cord -> next;
    curr_cord -> next = NULL;
  }
  return head_vec;
}

int calc_rows(vector * head_vec) {
  /*TODO: take care if there are no rows */
  int rows = 0;
  vector * curr_vec = head_vec;
  while (curr_vec -> next != NULL) {
    rows++;
    curr_vec = curr_vec -> next;
  }
  return rows;
}

int calc_cols(vector * head_vec) {
  int cols = 0;
  cord * curr_cord = head_vec -> cords;
  while (curr_cord != NULL) {
    cols++;
    curr_cord = curr_cord -> next;
  }
  return cols;

}

data_point_index * from_file_to_arr_of_dp(int rows, int cols, vector * head_vec) {
  vector * curr_vec;
  cord * curr_cord;
  int i;
  int j;
  data_point_index * data_points_arr = calloc(rows, sizeof(data_point_index));
  curr_vec = head_vec;
  
  for (i = 0; i < rows; i++) {
    curr_cord = curr_vec -> cords;
    data_points_arr[i].data_point = calloc(cols, sizeof(double));
    data_points_arr[i].index = -1;
    for (j = 0; j < cols; j++) {
      data_points_arr[i].data_point[j] = curr_cord -> value;
      curr_cord = curr_cord -> next;
    }
    curr_vec = curr_vec -> next;
  }
  return data_points_arr;
}

cluster * from_file_to_arr_of_clusters(data_point_index * arr_of_data_points, int k, int cols) {
  cluster * curr_cluster;
  int i;
  int j;
  cluster * clusters_arr = calloc(k, sizeof(cluster));
  for (i = 0; i < k; i++) {
    curr_cluster = & clusters_arr[i];
    curr_cluster -> num_of_dp = 1;
    curr_cluster -> centroid = calloc(cols, sizeof(double));
    curr_cluster -> sum_vec = calloc(cols, sizeof(double));
    arr_of_data_points[i].index = i;

    /*copy data point */
    for (j = 0; j < cols; j++) {
      curr_cluster -> centroid[j] = arr_of_data_points[i].data_point[j];
      curr_cluster -> sum_vec[j] = arr_of_data_points[i].data_point[j];
    }

  }
  return clusters_arr;

}

void kmeans(int k, int iter, int rows, int cols, data_point_index * arr_of_data_points, cluster * arr_of_clusters) {
  int i;
  int cluster_index;
  int entry;
  const double epsilon = 0.001;
  int exists_bigger_than_epsilon = 1;
  int iterations = 0;
  double * new_centroid;

  while (iterations < iter && exists_bigger_than_epsilon == 1) {
    /* assign data points to clusters */

    for (i = 0; i < rows; i++) {
      assign_dp_to_closest_cluster(i, k, cols, arr_of_data_points, arr_of_clusters);
    }

    /* calc new centroid */
    exists_bigger_than_epsilon = 0;
    for (cluster_index = 0; cluster_index < k; cluster_index++) {
      new_centroid = calloc(cols, sizeof(double));
      calc_new_centroid(cluster_index, new_centroid, cols, arr_of_clusters);
      if (calc_dist_centroid_centroid(cluster_index, new_centroid, cols, arr_of_clusters) >= epsilon) {
        exists_bigger_than_epsilon = 1;
      }
      /* update cluster to new centroid */
      for (entry = 0; entry < cols; entry++) {
        arr_of_clusters[cluster_index].centroid[entry] = new_centroid[entry];
      }
      free(new_centroid);

    }
    iterations += 1;
  }
}

void assign_dp_to_closest_cluster(int dp_index, int k, int cols, data_point_index * arr_of_data_points, cluster * arr_of_clusters) {
  int index_of_closest_cluster = calc_closest_cluster(dp_index, k, cols, arr_of_data_points, arr_of_clusters);
  int index_of_prev_cluster = arr_of_data_points[dp_index].index;
  /* the dp stays in the same cluster */
  if (index_of_closest_cluster == index_of_prev_cluster) {
    return;
  } else {

    if (index_of_prev_cluster != -1) {
      /* change num of associated dps to cluster */
      arr_of_clusters[index_of_prev_cluster].num_of_dp -= 1;
      /* decrease prev sum */
      compute_new_sum(dp_index, index_of_prev_cluster, '-', cols, arr_of_data_points, arr_of_clusters);
    }
    arr_of_clusters[index_of_closest_cluster].num_of_dp += 1;
    /* increase new sum */
    compute_new_sum(dp_index, index_of_closest_cluster, '+', cols, arr_of_data_points, arr_of_clusters);
    /* change index to new cluster */
    arr_of_data_points[dp_index].index = index_of_closest_cluster;

  }
}

int calc_closest_cluster(int dp_index, int k,int cols, data_point_index * arr_of_data_points, cluster * arr_of_clusters) {
  int cluster_index;
  double min_dist = __DBL_MAX__;
  int min_cluster_index = 0;
  double dist;
  for (cluster_index = 0; cluster_index < k; cluster_index++) {
    dist = calc_dist_dp_centroid(dp_index, cluster_index, cols, arr_of_data_points, arr_of_clusters);
    if (dist < min_dist) {
      min_dist = dist;
      min_cluster_index = cluster_index;
    }
  }
  return min_cluster_index;
}

double calc_dist_dp_centroid(int dp_index, int cluster_index, int cols, data_point_index * arr_of_data_points, cluster * arr_of_clusters) {
  int entry;
  double sum = 0;
  for (entry = 0; entry < cols; entry++) {
    double dp_entry = arr_of_data_points[dp_index].data_point[entry];
    double centroid = arr_of_clusters[cluster_index].centroid[entry];
    sum += pow(dp_entry - centroid, 2);
  }

  return sqrt(sum);
}

void compute_new_sum(int dp_index, int cluster_index, char op, int cols, data_point_index * arr_of_data_points, cluster * arr_of_clusters) {
  int entry;
  for (entry = 0; entry < cols; entry++) {
    if (op == '-') {
      arr_of_clusters[cluster_index].sum_vec[entry] -= arr_of_data_points[dp_index].data_point[entry];
    } else {
      arr_of_clusters[cluster_index].sum_vec[entry] += arr_of_data_points[dp_index].data_point[entry];
    }
  }
}

void calc_new_centroid(int cluster_index, double * new_centroid, int cols, cluster * arr_of_clusters) {
  int entry;
  for (entry = 0; entry < cols; entry++) {
    new_centroid[entry] = arr_of_clusters[cluster_index].sum_vec[entry] / arr_of_clusters[cluster_index].num_of_dp;
  }
}

double calc_dist_centroid_centroid(int cluster_index, double * new_centroid, int cols, cluster * arr_of_clusters) {
  int entry;
  double sum = 0;
  for (entry = 0; entry < cols; entry++) {
    double old_centroid_entry = arr_of_clusters[cluster_index].centroid[entry];
    double new_centroid_entry = new_centroid[entry];
    sum += pow(old_centroid_entry - new_centroid_entry, 2);
  }
  return sqrt(sum);

}

void printres(int k, int cols, cluster * arr_of_clusters) {
  int i;
  int j;
  double shifted, result;
  int truncated;
  for (i = 0; i < k; i++) {
    for (j = 0; j < cols; j++) {
      shifted = arr_of_clusters[i].centroid[j] * 10000.0;
      shifted += 0.5;
      truncated = (int)shifted;
      result = (double)truncated / 10000.0;
      if (j < cols - 1) {
        printf("%.4f,", result);
      } else {
        printf("%.4f\n", result);
      }
    }
  }

}

int is_integer(char *str) {
    while (*str) {
        if (*str < '0' || *str > '9') {
            return 0; 
        }
        str++;
    }
    return 1;
}

int main(int argc, char * argv[]) {
  vector * head_vec;
  data_point_index * arr_of_data_points;
  cluster * arr_of_clusters;
  int iter;
  int rows = 0;
  int cols = 0;
  /*todo check if first is k (not ony input file) */
  /* todo free all memory */
  if(is_integer(argv[1]) == 0){  // todo do we need these validations?
    printf("%s\n", "An Error Has Occurred");
    return 1;
  }
  else{
    int k = atoi(argv[1]);
  if (argc == 3) {
    iter = atoi(argv[2]);
    if(iter <=1 || iter >= 1000){
      printf("%s\n", "An Error Has Occurred");
      return 1;
    }
    head_vec = read_file_dynamically();

  } else {
    if (argc == 2) {
      head_vec = read_file_dynamically();
      iter = 200;
    } else {
      printf("%s\n", "An Error Has Occurred");
      return 1;
    }
  }

  rows = calc_rows(head_vec);
  if(k <= 1 || k >= rows){
    printf("%s\n", "An Error Has Occurred");
    return 1;
  }

  cols = calc_cols(head_vec);
  arr_of_data_points = from_file_to_arr_of_dp(rows, cols, head_vec);
  arr_of_clusters = from_file_to_arr_of_clusters(arr_of_data_points, k, cols);

  kmeans(k, iter, rows, cols, arr_of_data_points, arr_of_clusters);
  printres(k, cols, arr_of_clusters);

  }

  return 0;

}
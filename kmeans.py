import sys
import math

EPS = 0.001


def get_euclidean_dist(vector1, vector2):
    return norm(vector1, vector2)


def find_kmeans(k: int, iter: int, vectors: list):
    K_array = vectors[:k]
    max_delta = 0.02
    iteration_number = 0
    while not (max_delta < EPS or iteration_number == iter):
        delta_k = zeros(k)
        iteration_number += 1
        elem_in_cluster = [0 for _ in range(k)]
        sum_for_cluster = [zeros(len(vectors[0])) for i in range(k)]
        for vector in vectors:
            min_dist = math.inf
            min_elem = None
            for j, curr_vec in enumerate(K_array):
                dist = get_euclidean_dist(vector, curr_vec)
                if dist < min_dist:
                    min_dist = dist
                    min_elem = j
            sum_for_cluster[min_elem] = add(vector, sum_for_cluster[min_elem])
            elem_in_cluster[min_elem] += 1
        max_delta = update_clusters_and_delta(delta_k, sum_for_cluster, elem_in_cluster, K_array)
    for i in range(K):
        line = ""
        for j in range(len(K_array[0])):
            K_array[i][j] = round(K_array[i][j], 4)
            if j == len(K_array[0]) - 1:
                line += f"{K_array[i][j]}"
            else:
                line += f"{K_array[i][j]},"
        print(line)


def update_clusters_and_delta(delta_k, sum_for_cluster, elem_in_cluster, k_array):
    for i in range(len(delta_k)):
        new_cluster = zeros(len(sum_for_cluster[0]))
        for j in range(len(sum_for_cluster[0])):
            new_cluster[j] = sum_for_cluster[i][j] / elem_in_cluster[i]
        delta_k[i] = get_euclidean_dist(new_cluster, k_array[i])
        k_array[i] = new_cluster
    return max(delta_k)


def zeros(n):
    return [0 for _ in range(n)]


def add(vector1, vector2):
    res = [0 for i in range(len(vector1))]
    for i in range(len(vector1)):
        res[i] = vector1[i] + vector2[i]
    return res


def norm(vector1, vector2):  # assume same size
    sum = 0
    for i in range(len(vector1)):
        sum += (vector1[i] - vector2[i]) ** 2
    return math.sqrt(sum)


def max(vector):
    max = 0
    for i in range(len(vector)):
        if vector[i] > max:
            max = vector[i]
    return max


# read arguments and run the program

if __name__ == '__main__':

    try:
        K = sys.argv[1]
        if len(sys.argv) == 4:
            iter = sys.argv[2]
            input = sys.argv[3]
        elif len(sys.argv) == 3:
            iter = 200
            input = sys.argv[2]

        f = open(input, "r")
        vectors = [[float(elem) for elem in vector.rstrip('\n').split(',')] for vector in f.readlines()]

        # check errors
        error = False
        if not str.isdigit(K):
            print("Invalid number of clusters!")
            error = True
        elif int(K) >= len(vectors) or int(K) <= 1:
            print("Invalid number of clusters!")
            error = True

        if not str.isdigit(iter):
            print("Invalid maximum iteration!")
            error = True
        elif int(iter) >= 1000 or int(iter) <= 1:
            print("Invalid maximum iteration!")
            error = True

        if error:
            exit(1)

        K = int(K)
        iter = int(iter)

        find_kmeans(K, iter, vectors)

    except Exception as e:
        print("An Error Has Occurred")
        exit(1)

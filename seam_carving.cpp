#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <limits>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

class MinHeap {
public:
    void initialize(const vector<int>& distances) {
        heap.clear();
        position_map.clear(); // Clear position map
        for (int i = 0; i < distances.size(); ++i) {
            heap.push_back({i, distances[i]});
            position_map[i] = i; // Update position map
        }
        make_heap(heap.begin(), heap.end(), greater<>());
    }

    pair<int, int> pop() {
        pop_heap(heap.begin(), heap.end(), greater<>());
        auto element = heap.back();
        heap.pop_back();
        return element;
    }

    bool empty() const {
        return heap.empty();
    }

    void update(int node, int score) {
        // Update score in heap
        int pos = position_map[node];
        heap[pos].second = score;
        push_heap(heap.begin(), heap.begin() + pos + 1, greater<>());
    }

private:
    vector<pair<int, int>> heap;
    unordered_map<int, int> position_map; // Map to store positions of pixels in the heap
};

int energy_difference(int pixel1, int pixel2) {
    return abs(pixel1 - pixel2);
}

vector<vector<pair<int, int>>> generate_adjacency_list(const vector<vector<int>>& image) {
    int rows = image.size();
    int cols = image[0].size();
    int num_vertices = rows * cols + 2;
    vector<vector<pair<int, int>>> adjacency_list(num_vertices);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int u = r * cols + c;

            // Connect each pixel to three pixels in the upper row
            if (r > 0) {
                int v1 = (r - 1) * cols + max(c - 1, 0); // Pixel to the left
                int v2 = (r - 1) * cols + c;             // Pixel directly above
                int v3 = (r - 1) * cols + min(c + 1, cols - 1); // Pixel to the right

                int weight1 = energy_difference(image[r][c], image[r - 1][max(c - 1, 0)]);
                int weight2 = energy_difference(image[r][c], image[r - 1][c]);
                int weight3 = energy_difference(image[r][c], image[r - 1][min(c + 1, cols - 1)]);

                adjacency_list[u].emplace_back(v1, weight1);
                adjacency_list[u].emplace_back(v2, weight2);
                adjacency_list[u].emplace_back(v3, weight3);
            }

            // Connect source to the first row of pixels
            if (r == 0) {
                adjacency_list[num_vertices - 2].emplace_back(u, 0);
            }

            // Connect the last row of pixels to the target
            if (r == rows - 1) {
                adjacency_list[u].emplace_back(num_vertices - 1, 0);
            }
        }
    }

    return adjacency_list;
}

vector<int> dijkstra(const vector<vector<pair<int, int>>>& adjacency_list, int source, int target) {
    int n = adjacency_list.size();
    vector<int> distances(n, numeric_limits<int>::max());
    vector<int> predecessors(n, -1);
    MinHeap pq;

    distances[source] = 0;
    pq.initialize(distances);

    while (!pq.empty()) {
        auto elem = pq.pop();
        int u = elem.first;
        int dist_u = elem.second;

        if (u == target) {
            break;
        }

        for (const auto& edge : adjacency_list[u]) {
            int v = edge.first;
            int weight = edge.second;
            int new_dist = dist_u + weight;
            // Only update distance if it improves
            if (new_dist < distances[v]) {
                distances[v] = new_dist;
                predecessors[v] = u;
                pq.update(v, new_dist);
            }
        }
    }

    // Trace back the shortest path to reconstruct the seam
    vector<int> path;
    for (int at = target; at != -1; at = predecessors[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    return path;
}

vector<int> find_minimum_seam(const vector<vector<int>>& image) {
    auto adjacency_list = generate_adjacency_list(image);
    int source = adjacency_list.size() - 2;
    int target = adjacency_list.size() - 1;
    vector<int> path = dijkstra(adjacency_list, source, target);

    int rows = image.size();
    int cols = image[0].size();
    vector<int> seam;

    for (int i = 1; i < path.size() - 1; ++i) { // Exclude source and target
        int r = path[i] / cols;
        int c = path[i] % cols;
        seam.push_back(c);
    }

    

    return seam;
}

vector<vector<int>> read_pixels(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file " + filename);
    }

    vector<vector<int>> pixels;
    string line;
    while (getline(file, line)) {
        vector<int> row;
        int value;
        istringstream iss(line);
        while (iss >> value) {
            row.push_back(value);
        }
        pixels.push_back(row);
    }
    return pixels;
}

void save_seam(const vector<int>& seam, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file " + filename);
    }

    for (int s : seam) {
        file << s << " ";
    }
    file << endl;
}

int main() {
    try {
        vector<vector<int>> image = read_pixels("pixels.txt");

        cout << "Read pixels from file successfully. Image dimensions: " 
             << image.size() << "x" << (image.empty() ? 0 : image[0].size()) << endl;

        vector<int> seam = find_minimum_seam(image);
        cout << "Calculated minimum weighted seam:";
        for (int s : seam) {
            cout << " " << s;
        }
        cout << endl;

        save_seam(seam, "seam.txt");

        cout << "Seam saved to seam.txt successfully." << endl;
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
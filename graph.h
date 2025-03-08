#ifndef GRAPH_H
#define GRAPH_H
#include <QSet>
#include <QDebug>
#include <unordered_map>
#include <queue>
#include <unordered_set>

template <typename T1, typename T2>
struct std::hash<QPair<T1, T2>> {
    size_t operator() (const QPair<T1, T2>& pair) const noexcept {
        return hash<T1>()(pair.first) ^ hash<T2>()(pair.second);
    }
};


template <typename T>
class graph {
private:
    std::unordered_map<T, QSet<T>> adj_list;

    bool dfs(const T& node, std::unordered_set<T>& visited, std::unordered_set<T>& rec_stack) {
        visited.insert(node);
        rec_stack.insert(node);

        for (const auto& neighbor : adj_list[node]) {
            if (rec_stack.find(neighbor) != rec_stack.end()) {
                return true;
            }
            if (visited.find(neighbor) == visited.end()) {
                if (dfs(neighbor, visited, rec_stack)) {
                    return true;
                }
            }
        }
        rec_stack.erase(node);
        return false;
    }

public:
    void insert_node(const T& node) {
        if (adj_list.find(node) == adj_list.end()) {
            adj_list[node] = QSet<T>();
        }
    }

    void delete_node(const T& node) {
        adj_list.erase(node);
        for (auto& pair : adj_list) {
            auto& neighbors = pair.second;
            neighbors.remove(node);
        }
    }

    void insert_edge(const T& from, const T& to) {
        insert_node(from);
        insert_node(to);
        adj_list[from].insert(to);
    }

    void delete_edge(const T& from, const T& to) {
        auto it = adj_list.find(from);
        if (it != adj_list.end()) {
            auto& neighbors = it->second;
            neighbors.remove(to);
        }
    }

    void delete_income_edges(const T& node) {
        if(adj_list.find(node) == adj_list.end()) return;
        for (auto& pair : adj_list) {
            auto& neighbors = pair.second;
            neighbors.remove(node);
        }
    }

    void delete_outgo_edges(const T& node) {
        auto it = adj_list.find(node);
        if (it != adj_list.end()) {
            it->second.clear();
        }
    }

    bool find(const T& node) const {
        return adj_list.find(node) != adj_list.end();
    }

    bool depth_first_search() {
        std::unordered_set<T> visited;
        std::unordered_set<T> rec_stack;

        for (const auto& pair : adj_list) {
            const T& node = pair.first;
            if (visited.find(node) == visited.end()) {
                if (dfs(node, visited, rec_stack)) {
                    return true;
                }
            }
        }
        return false;
    }

    void traverse(const T& start_node, std::function<void(const T&)> operation) {
        std::unordered_set<T> visited;
        std::queue<T> queue;
        queue.push(start_node);
        visited.insert(start_node);

        while (!queue.empty()) {
            T current_node = queue.front();
            queue.pop();
            if(current_node == start_node) continue;
            operation(current_node);
            for (const auto& neighbor : adj_list[current_node]) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    queue.push(neighbor);
                }
            }
        }
    }

    void print() {
        for (const auto& pair : adj_list) {
            qDebug() << "Node " << pair.first << " -> ";
            for (const auto& neighbor : pair.second) {
                qDebug() << neighbor << " ";
            }
            qDebug() << '\n';
        }
    }
};

#endif // GRAPH_H

#pragma once

#include <vector>
#include <algorithm>

template <typename T>
class SparseSet
{
private:
    //index of sparse == dense[x].first | sparse's value is index of component in dense
    // sparse[dense[x].first] == x || dense[x].first == id
    std::vector<size_t> _sparse;
    //index of dense == sparse[x] | dense's value is dense[y].second
    // dense[sparse[id]] = (val.second == T) (val.first = x)
    std::vector<std::pair<size_t, T>> _dense;
    // const -1 expression
    static constexpr size_t INVALID = static_cast<size_t>(-1);

public:
    SparseSet(size_t maxId) : _sparse(maxId, INVALID) {}
    ~SparseSet() {}


    class Iterator
    {
    private:
        
        std::pair<size_t, T>* ptr;

    public:
        Iterator(std::pair<size_t, T>* pointer) : ptr(pointer) {};

        std::pair<size_t, T>& operator*(){ return *ptr; }
        Iterator& operator++(){ ptr++; return *this;}
        bool operator!=(const Iterator& other){return ptr != other.ptr;}
        bool operator==(const Iterator& other){return ptr == other.ptr;}

    };

    Iterator begin(){return Iterator(_dense.data());}
    Iterator end(){return Iterator(_dense.data() + _dense.size());}
    
    

    bool exists(const size_t& id) {return id < _sparse.size() && _sparse[id] != INVALID;}

    T& getComponent(const size_t& id){
        return _dense[_sparse[id]].second;
    }

    void insert(std::pair<size_t, T> p){
        if (exists(p.first)) return;
        _sparse[p.first] = _dense.size();
        _dense.push_back(std::move(p));
    }

    void vectorInsert(std::pair<size_t, T> p){
        if (exists(p.first)) 
        {
            _dense[_sparse[p.first]].second.push_back(p.second);
            return;
        }
        _sparse[p.first] = _dense.size();
        _dense.push_back(std::move(p));
    }

    void vectorExtract(size_t client,size_t id){
        if (!exists(client)) return;
        if(_dense[_sparse[client]].second.size() == 1){
            _sparse[_dense[_dense.size() - 1].first] = _sparse[client];
            _dense[_sparse[client]] = _dense[_dense.size() - 1];
            _sparse[client] = INVALID;
            _dense.pop_back();
        }else{
            auto& v = _dense[_sparse[client]].second;
            //finds the id index in the vector
            auto it = std::find(v.begin(), v.end(), id);
            if (it != v.end()) {
                *it = v.back();   // overwrite the found element with the last one
                v.pop_back();
            }
        }
    }

    void extract(size_t id){
        if (!exists(id)) return;
        _sparse[_dense[_dense.size() - 1].first] = _sparse[id];
        _dense[_sparse[id]] = _dense[_dense.size() - 1];
        _sparse[id] = INVALID;
        _dense.pop_back();
    }
};

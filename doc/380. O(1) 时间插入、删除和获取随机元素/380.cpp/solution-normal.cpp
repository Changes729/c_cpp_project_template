#include <algorithm>
#include <cctype>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

class RandomizedSet {
public:
  RandomizedSet() : array{}, array_handle{0}, h_map{} {}

  bool insert(int val) {
    bool ret = false;
    if (h_map.end() == h_map.find(val)) {
      array[array_handle] = val;
      h_map.insert({val, array_handle});
      ++array_handle;
      ret = true;
    }
    return ret;
  }

  bool remove(int val) {
    bool ret = false;
    auto val_itr = h_map.find(val);
    if (val_itr != h_map.end()) {
      int replace_index = val_itr->second;
      --array_handle;
      array[replace_index] = array[array_handle];
      h_map.erase(val_itr);
      val_itr = h_map.find(array[array_handle]);
      if (val_itr != h_map.end()) {
        val_itr->second = replace_index;
      }
      ret = true;
    }
    return ret;
  }

  int getRandom() {
    int index = rand() % array_handle;
    return array[index];
  }

private:
  int array[200000];
  int array_handle;
  unordered_map<int, int> h_map;
};

/**
 * Your RandomizedSet object will be instantiated and called as such:
 * RandomizedSet* obj = new RandomizedSet();
 * bool param_1 = obj->insert(val);
 * bool param_2 = obj->remove(val);
 * int param_3 = obj->getRandom();
 */
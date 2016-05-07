A simple trie data structure.

* Each node has a symbol and a pointer to the next and to the alternative one.
* If a node hasn't a negative child it stores a pointer to his father's node.
* If a node hasn't a positive child it stores a pointer to a value.

Example: A trie below contains the pairs key/value ("aaa"/1, "aba"/2, "abb"/3). Vertical branches are positive (it means that the current symbol of a key is equal to node's symbol). Horizontal branches are negative (it means that the current symbol of a key isn't equal to node's symbol).

``` Example
 ['a'] <---+
   |       |
 ['a'] - ['b'] <---+
   |       |       |
 ['a']   ['a'] - ['b'] <-+
   |       |       |     |
 ['\0']  ['\0']  ['\0']--+
   |       |       |
  (1)     (2)     (3)
 ```

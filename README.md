A simple trie data structure ([wiki](https://en.wikipedia.org/wiki/Trie#Implementation_strategies)).

* Each node has a symbol and a pointer to the next and to the alternative one.
* If a node hasn't a negative child it stores a pointer to his father's node.
* If a node hasn't a positive child it stores a pointer to a value.

Example: A trie below contains the pairs of key/value ("aaa"/D1, "aba"/D2, "abb"/D3 "cb"/D4). Vertical branches(green) are positive (it means that the current symbol of a key is equal to node's symbol). Horizontal branches(red) are negative (it means that the current symbol of a key isn't equal to node's symbol). Dotted branches show links to a parent node. Blue branches point to value nodes.

![example](https://rawgit.com/DerShokus/trie/master/example.svg "Trie example")

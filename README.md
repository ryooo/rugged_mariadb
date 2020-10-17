rugged_mariadb
============

Enables rugged(libgit2 bindings in Ruby, visit by https://github.com/libgit2/rugged) to store git objects and references into MariaDB.



## Installation

    bundle config --local build.rugged_mariadb "--with-cppflags=-Ipath_to/include/mysql -DLIBGIT2_NO_FEATURES_H"

Add this line to you application's Gemfile:

    gem 'rugged_mariadb', github: 'ryooo/rugged_mariadb'

And then execute:

    bundle install


## Usage
```
mariadb_backend = Rugged::MariaDB::Backend.new(host:'localhost', port:3306, username:'**', password:'**', database:'**')
repo = Rugged::Repository.bare('repo-name', backend: mariadb_backend)

oid = repo.write("This is a blob.", :blob)
index = repo.index
index.add(path: "README.md", oid: oid, mode: 0100644)
options = {
  tree: index.write_tree(repo),
  author: { email: "testuser@github.com", name: 'Test Author', time: Time.now },
  committer: { email: "testuser@github.com", name: 'Test Author', time: Time.now },
  message: "Making a commit via Rugged!",
  update_ref: 'HEAD',
  parents: repo.head.empty? ? [] : [ repo.head.target ].compact,
}

Rugged::Commit.create(repo, options)
```

Each instance of the backend consumes a single MySql connection.

Enjoy it!

## Contributing
Fork devchild/rugged_mariadb on GitHub, make it awesomer (preferably in a branch named for the topic), send a pull request.

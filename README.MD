# Operating Systems Project

## Assignment

[Phase-1](https://github.com/vicentecsousa/MAGNAEATS/blob/master/rsc/so2021_2022-projecto1-fase1.pdf)

[Phase-2](https://www.youtube.com/watch?v=dQw4w9WgXcQ)

## How to use

### 1. Clone

Use one of the following

```sh
git clone https://github.com/vicentecsousa/MAGNAEATS.git
git clone git@github.com:vicentecsousa/MAGNAEATS.git
```

then cd to the folder

```
cd MAGNAEATS
```

### 2. Compile

```sh
## Normal
make
## Debug
make debug
```

### 3. Test

At the moment there are no tests

In the future run test by executing

```sh
make test
```

### 4. Run

```sh
./bin/magnaeats [max_ops] [buffers_size] [n_restaurants n_drivers] [n_clients]
```


## How to work

After cloning run to prevent changing the include files by mistake

```sh
git update-index --skip-worktree include/client.h
git update-index --skip-worktree include/driver.h
git update-index --skip-worktree include/m
git update-index --skip-worktree include/main.h
git update-index --skip-worktree include/process.h
git update-index --skip-worktree include/restaurant.h
```

Run this to prevent changing vscode project settings with your own personal settings

```sh
git update-index --skip-worktree .vscode/*
```

Open vscode install c\c++ and go for it


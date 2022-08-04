# Operating Systems Project

## Assignment

[Phase-1](https://github.com/JoaoAAnjos/MAGNAEATS/blob/master/rsc/so2021_2022-projecto1-fase1.pdf)

[Phase-2](https://github.com/JoaoAAnjos/MAGNAEATS/blob/master/rsc/so2021_2022-projecto2_v3.pdf)

## About

This project was developed in the context of a Operating Systems class while taking software engineering at FCUL. The main goal of the project was to develop an application on C, that would mimic a system of requests and answers, similar to a food-delivery app (hence the name MagnaEats). There are three types of participants, through which the messages (requests) flow: clients, restaurants and drivers. 
The project was developed in two phases: 
* The first one focused on process management, memory allocation and the creation of the makefile, all essential knowledge to develop an app in C.
* The second focused on Synchronization (concurrent memory acess) ,implemented through the use of semaphores, file handling and signals.

The final grade for this project was 18.78/20

## How to use

### 1. Clone

Use one of the following

```sh
git clone https://github.com/JoaoAAnjos/MAGNAEATS.git
git clone git@github.com:JoaoAAnjos/MAGNAEATS.git
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
## All
make all
```

### 3. Test

At the moment there are no tests

In the future run test by executing

```sh
make test
```

### 4. Run

```sh
./bin/magnaeats [max_ops] [buffers_size] [n_restaurants] [n_drivers] [n_clients]
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


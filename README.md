# TODO checker

Simple command line tool for checking TODOs in files.

<p align=center>
  <img src="./assets/showcase.png">
</p>

## Building
```console
$ git clone https://github.com/wakozim/todo_checker.git
$ gcc -O3 -Wall -Wextra -o todo_checker todo_checker.c
$ ./todo_checker
```

## Usage

To run the checker, you can simply call `./todo_checker [FILE|DIR]...`. The argument(s) can be any file or directory. For example:
```sh
./todo_checker ./example.c
```

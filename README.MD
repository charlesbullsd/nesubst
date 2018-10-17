# nesubst

nesubst – is the **n**ew **e**nv**subst**. Just like envsubst, but more like bash.

Written in C. Fast. 14KB binary. Statically compiled.

# TL;DR

nesubst performs environment variable parameter expansions in arbitrary data streams.

```
echo 'I am currently in the $PWD directory' | nesubst
```

# Why?

Ever heard of the `${DOLLAR}` hack?

Also, [bash parameter expansions](https://www.gnu.org/software/bash/manual/html_node/Shell-Parameter-Expansion.html) are pretty nice.

# Examples?

If you know how to bash, then you already know how to use nesubst!

```
echo 'hi $CAT!'        | CAT=cat nesubst # hi cat!
echo 'hi ${CAT}!'      | CAT=cat nesubst # hi cat!
echo 'hi ${CAT-$DOG}!' | DOG=dog nesubst # hi dog!
echo 'hi ${CAT-dog}!'  |         nesubst # hi dog!
```

# Docker

There is a Dockerfile to compile the program as well as to build the image.
It is based on alpine linux, even though it is statically compiled, because it can benefit from the other commands that come with it (like `cat`).

# TODO

- implement more of the bash parameter expansion features/syntax
- (maybe) add parameters for changing token characters

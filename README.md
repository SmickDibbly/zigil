# Zigil

Description to come...

# Building
On a linux-kernel OS, from the zigil/build/ directory, run the shell command:

```Shell
make --file=linux.mk
```

This will create zigil/build/linux/libzigil.a, to be linked to other projects at your leisure. The API consists of all the header files in zigil/src/ (header files from any further subdirectories thereof are not part of the API).

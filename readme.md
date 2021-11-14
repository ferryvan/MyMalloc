Author：ferryvan

RUN:

```bash
make

gcc test.c -pthread -I ./inc -L . -lMyMallocFree -o out.a

export LD_LIBRARY_PATH=.

./out.a
```

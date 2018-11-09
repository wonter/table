# TABLE

Table is an in memory key-value store library

**NOTE: Table is just my toy project, it may have some bugs, so do not use Table in productio.**

## Features

* Keys and values are arbitrary byte arrays
* Data is stored sorted by key
* The basic operations are `put(key,value)`, `get(key)`, `del(key)`
* Support for persisting data to disk
* Safe to use Table in multithreaded single writer multi-reader code

## Build

TODO

*Maybe use CMake*

## Quick Start

### Opening/Closing a Table

```cpp
table::Options options;
options.create_if_missing = true;
table::Table table(options, "test_table");

table::Status s = table.open();
if(!s.good()) {
    std::cerr << s.string() << std::endl;
}

table.close();
```

### Reads/Writes

```cpp
std::string key = "key";
std::string value = "value";

s = table.put(key, value);
if (!s.good()) {
    std::cerr << s.string() << std::endl;
}

s = table.get(key, &value);
if (!s.good()) {
    std::cerr << s.string() << std::endl;
} else {
    std::cout << value << std::endl;
}
```

### Persisting data

```cpp
std::string key = "key";
std::string value = "value";

s = table.put(key, value);
if (!s.good()) {
    std::cerr << s.string() << std::endl;
}

s = table.dump();
if (!s.good()) {
    std::cerr << s.string() << std::endl;
}
```

or set `options.dump_when_close = true;`

## Architecture

![architecture](https://user-images.githubusercontent.com/17780091/48275355-3de27c00-e480-11e8-9b2b-ea879a445bba.png)

## Performance

* CPU: Intel Core i5 2.3 GHz
* CPU Cores(physical) num: 2
* CPU Cores(logical) num:  4
* CPU L2 Cache：256 KB
* CPU L3 Cache：4MB
* Key Size：  16
* Value Size：100
* GCC Optimization Level：O2

#### Put Performance

Put 100,000 entries into a empty Table

| Times | Spend |
| --- | ------- |
|  1  |   178ms |
|  2  |   172ms |
|  3  |   155ms |
|  4  |   152ms |
|  5  |   146ms |

Put 1,000,000 entries into a empty Table

| Times | Spend |
| --- | ------- |
|  1  |  3094ms |
|  2  |  2619ms |
|  3  |  2614ms |
|  4  |  2614ms |
|  5  |  2631ms |

#### Get Performance

Table has 100,000 entries, and we get 10,000 different keys from Table

| Times | Spend |
| --- | ------- |
|  1  |   10ms  |
|  2  |   11ms  |
|  3  |   12ms  |
|  4  |   14ms  |
|  5  |   12ms  |

Table has 1,000,000 entries, and we get 10,000 different keys from Table

| Times | Spend |
| --- | ------- |
|  1  |   30ms  |
|  2  |   35ms  |
|  3  |   29ms  |
|  4  |   29ms  |
|  5  |   27ms  |

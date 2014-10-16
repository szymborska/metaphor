# Metaphor

## Building the code

Metaphor currently has a dependency on jansson. After installing it, run
the following commands to build metaphor:

```
% sh autogen.sh
% ./configure --prefix=/usr
% make all
```

We recommend running the quality checks after buliding. 

## Running quality checks

**Running Tests**

```
% make run_tests
```

**Profiling Code**

```
% make static_analysis 
```

**Static Analysis**

```
% make static_analysis 
```

**Run All Checks**

```
% make quality
```

## Installing

To install the code run the command:

```
% sudo make install 
```



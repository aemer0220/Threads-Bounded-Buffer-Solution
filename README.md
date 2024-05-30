# CS441/541 Threads (Bounded Buffer)

## Author(s):

Alexandra Emerson


## Date:

3/31/2024


## Description:

This program implements a solution to the bounded buffer problem. Users have the ability to specify the sleep time, number of producer threads, number of consumer threads, and the size of the buffer.


## How to build the software

Provide the command 'make' to utilize the given Makefile. This will compile the program

## How to use the software

To run the Bounded Buffer program, please provide the following command...
```
./bounded-buffer #1 #2 #3 [4] 
```
where the hashtagged integers are replaced by three (mandatory) above-zero integers and an optional fourth above-zero integer. These arguments correspond to and specify specific parameters within the program, as follows:  

Integer #  | Parameter
------------- | -------------
#1   | Sleep time before termination
#2   | Number of producer threads
#3   | Number of consumer threads
#4*   | Buffer-size

__\* indicates that specifying the buffer size is optional.__ If not provided, the buffer size is automatically set to 10.  

__Other input variants will not be accepted by the program.__  

__Example input 1:__ ./bounded-buffer 5 4 3 2
__Example input 2:__ ./bounded-buffer 2 2 5

## How the software was tested
* __Run with non-integers as input:__ To further test input validity, I did several runs with various non-integer parameters.

 __Example test input:__
```
  ./bounded-buffer a b c d
  ./bounded-buffer # # #
```
All examples above are incorrect inputs and failed as expected.
* __Run with 0s for each input:__ To further test input validity, I did several runs with 0s in each of the parameters

 __Example test input:__
```
  ./bounded-buffer 1 2 3 0
  ./bounded-buffer 1 2 0 3
  ./bounded-buffer 1 0 2 3
  ./bounded-buffer 0 1 2 3
```
All examples above are valid inputs and the program ran as expected
* __Run with different numbers of inputs:__ To test input validity, I did several runs with various numbers of integer parameters.

  __Example test input:__
```
  ./bounded-buffer
  ./bounded-buffer 5
  ./bounded-buffer 5 4
  ./bounded-buffer 5 4 3 2 1
```
All examples above are incorrect inputs and failed as expected.

## Known bugs and problem areas
For some reason, the program seg faults when pthread_cancel() is called. When the calls to this method are commented out, the program behaves (seemingly) correctly. I also tried pthread_join(), but this causes an infinite loop/deadlock to occur.

# Assignment 4: An Audit-Logged Work-Queue based HTTP Server by Yesh Chandiramani

## Program Design
My implementation of the Audit-Logged Work-Queue based HTTP Server works as follows:
- create a mapping of file URI's to a reader writer lock (provided by the POSIX pthread library)
- based on the HTTP operation, use the reader writer lock as needed
- use a mutex to ensure atomic writes occur when logging to the audit log

### Reader Writer Lock to URI Mapping
I created a dictionary-like data structure (`MutexMap`) to hold the URI to reader writer lock mapping. I use my linked list data structure to store `Entry` structs, which are similar to a key-value pair.

When the request has been parsed, I make a `AddEntry` call to my `MutexMap` object that I defined when the httpserver was started. `AddEntry` will create the mapping and return a pointer to the mapped `pthread_rwlock_t`.

When there are multiple requests to the same URI, `MutexMap` will not create duplicate mappings. It will return the one that already exists in the list of mappings.

### Using the Reader Writer Lock
After the request is complete, the request gets processed. The the reader writer lock pointer is passed into the processing module. 

For a GET request, the read lock is acquired only right before the server sends the 200 response with the content-length and the file's data. After the server completes its response, the lock is released.

For a PUT request, the write lock is acquired when checking if the file exists and is released after creating the new file.

For an APPEND request, the read lock is acquired when copying the data from the existing file to a tempfile that will hold the final result of the APPEND request.
After copying the data, the read lock is released. Once the tempfile has copied the data that was provided by the request body, the tempfile is ready to overwrite the existing file in the directory. That is when the write lock is acquired. After the overwritting process, the write lock is released.

### Mutex for Atomic logging to the Logfile
After a request has been fully processed and responded too, the logfile will be written to. To avoid race conditions, the thread acquires a mutex that is only used for the logfile. After the mutex is acquired, the logfile is written to, flushed, and then releases the mutex.

### Side-note:
I have revised my implementation of receivng the message body for a PUT and APPEND request. As I receive the message body, I write to a tempfile. This allows for the server to handle requests with large message bodies. Also when it comes to handling concurrent requests on the same URI, the tempfile will overwrite the existing file. This is a process that allows for the server to respond as fast as possible to all the concurrent requests on the same URI.
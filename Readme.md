<h1> Memcached FUSE File System </h1>

Following file system uses memcached to operate on files and directories as if it was on real disk. Let's start overviewing memcached class



1. **Memcached Class overview**
    

    ```c
        typedef struct memcached{
            char *addr;
            int port;
            int fd;
        } memcached;
    ```

    Memcached class is responsible for sending and recieving data over network. struct memcached is one that saves file descriptor that is needed to send and recieve data. 
    each of given function returns integer type value, whis is one of the following:

    ``ENUM {STORED, DELETED, NOT_FOUND, NOT_STORED, ERROR }; ``
    
    following enumerations are used to check errors while communicating to memcached.
    main error checking is done on the lowest level. so we can avoid any of unnecessary bugs that may have occured. 

    with given memcached struct we also have struct which saves information about requested key

    ```c
    typedef struct mm_data_info{
        char *key,*value;
        int flags, ttl, size;
    } mm_data_info;
    ```
    following mm_data_info struct is responsiible for saving all needed parameters that memcached needs to get/save value. In this class malloc()-s are rarely used because it is a must for the lowest class to work as fast as possible


2. **Chunk Class overview**
    ```c
    typedef struct chunk
    {
        int ind;
        char data[DATA_SIZE];
        char key[MAX_FNAME];
    } chunk;
    ```

    chunk class is responsible for saving some part of data. each chunk can save up to DATA_SIZE bytes. also each of them have variable ind. which is indicator of current size that is filled in this chunk. 

3. **Content Class overview**
    ```c
    typedef struct content
    {
        char path[MAX_FNAME];
        int size;
    } content;
    ```
    content class is responsible to save full data of some directory or file. each content is assigned to some file or directory. which we have information from path variable. size is indicator of how much bytes are written in this current content

4. (1) **File Class overview**
    ```c
    typedef struct file{
        char file_name[MAX_FNAME];
        content ex_cn;
        content cn;
        int is_linked;
        uid_t uid;
        gid_t gid;
        mode_t mode;
    } file;
    ```
    file class is responsible for creating files and operating over it. File name is full path of current file. which is the key to the memcached. also here we have two content classes 'cn' is responsible forsaving real data, as for 'ex_cn' is is used to save external attributes.

4. (2) **Directory Class overview**
    ```c
    typedef struct dir
    {
        char dir_name[MAX_FNAME];
        content ex_cn;
        content cn;
        int is_linked;
        uid_t uid;
        gid_t gid;
        mode_t mode;
    } dir;
    ```
    Directory class is responsible for creating and saving directories. same structure as files but implementation is different.

    **Key Details**

    1). **Hierarchy :** current file system's main key is that it can be easily refactored or modified. Each of the class is on its own and only know what it really needs to know about lower classes.for example chunk class has no idea about content and Content has no idea what are directories or files. same goes to memcached. It is only interface that we use to save chunks and informations and thus have no idea that we are using it for file system. This architecture strongly follows decoupling of classes and dependancy injection.
        

         _______________
        |               |
        |   DIRECTORY   |_____
        |_______________|     |      _______________       ___________
                              |     |               |     |           |
                              |====>|    CONTENT    |====>|   CHUNK   |
         _______________      |     |_______________|     |___________|
        |               |_____|
        |     FILE      |
        |_______________| 
                          

    
    2). **Random Acces Memory :**
        Every chunk operation in this file system in in o(1). each chunk is indexed using file name and saved in the memcached as follows : "$(index_of_chunk)$(file_full_path)"

        Example:
            We create file named test.txt inside directory dir1.
            full path would be : /dir1/test.txt
            now imagine this file's size is 1 GB. and we want to access nth byte
            we just calculate index knowing that each chunk contains daa of DATA_SIZE bytes:
            index = n / DATA_SIZE
            needed chunk key = "$(index)/dir1/test.txt"

    

        
        
This project implements the following requirements:

1. Config Manager Interface
o Struct Declaration: Declare any public structures needed for storing and accessing the configuration file data. The use of opaque types is encouraged.
o Function Prototypes: Declare function prototypes for storing and fetching configuration data. You might find it helpful to implement some stub functions for testing purposes
o File I/O: Define function implementations to load configuration data from a file and save configuration data to a file. Since you don’t have the guts of the configuration manager implemented, you might find it helpful to implement stubs for the store/fetch functions.

2. Supported Data Types:   
o Basic types: int, float, C-strings.
o Arrays for the above types.

3. Functionalities:
o Store: Functions for storing values of various types
o Fetch: Functions for retrieving stored values of various types
o Other: Is there any other functionality you think might be useful for a configuration manager? Go ahead and add some prototypes for those functions.
o Load: Load configuration data from a file. You need to parse a file and store each key-value pair found in the file into the configuration manager.
o Save: Save the configuration data to a file. The resulting file should use the same format as your load function

4. File Parsing:
o Students can choose any file format (e.g., CSV, JSON, XML) and either write their own parser or use a third-party library

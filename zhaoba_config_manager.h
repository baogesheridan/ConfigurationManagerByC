#ifndef zhaoba_CONFIG_MANAGER_H
#define zhaoba_CONFIG_MANAGER_H

#include <stddef.h>  

// Forward declarations of the structs and union
typedef struct KeyValuePair KeyValuePair;
typedef struct ConfigManager ConfigManager;
typedef union Value Value;

// Enum to define the type of the value
typedef enum ValueType {
    INT,
    FLOAT,
    STRING,
    INT_ARRAY,
    FLOAT_ARRAY,
    STRING_ARRAY
} ValueType;

// Function Prototypes


// Initialize a new configuration manager
// 
// Create and populate the initial ConfigManager, set initial value of size to 0, initial capasity to 10, initialize records[]
// memory allocation failed, return NULL;
//
ConfigManager* create_config_manager();

// Free the memory used by a configuration manager

void free_config_manager(ConfigManager* cm);

// Store a value by key
//
// input existing configManage name, keyname, value, ValueType and unsigned arraySize, 
// then store supported data to configmanager. 
// supported ValueType:INT,FLOAT,STRING,INT_ARRAY,FLOAT_ARRAY, and STRING_ARRAY. standing int, float, string and their arrays.
// return 0 stands for store successfully.
// return -1 for unsupported ValueType or invalid input. 
// *****Example*****
//     int intValue = 42;
//      store_value_by_key(cm, "key_int", &intValue, INT, 0);
//     const char* stringArray[] = { "apple", "banana", "cherry" };
//      store_value_by_key(cm, "key_string_array", stringArray, STRING_ARRAY, 3);
//
int store_value_by_key(ConfigManager* cm, const char* key, void* value, ValueType type, size_t arraySize);

// Fetch a value by key
// 
// input existing configManage name, keyname,expectedType, and expected valueoutput variable name valueOut, then you can get value in ValueOut
// return 0 for fetch successfully.
// return -1 for invalid parameter, type mismatch, or key not found.
// ****Example****
//          int fetchedInt;
//          if (fetch_value_by_key(cm, "key_int", &fetchedInt, INT) == 0) {
//           printf("Fetched INT value: %d\n", fetchedInt);
//          }
//

int fetch_value_by_key(ConfigManager* cm, const char* key, void* valueOut, ValueType expectedType);

// Load configuration data from a file
//
// using the third party library cJSON.h, load from JSON file filename to an existing config manager cm
// return 0 for load successfully
// return -1 for invalid parameters, error opening file, error parsing JSON file
//
int load_config_from_file(ConfigManager* cm, const char* filename);

// Save configuration data to a file
//
// using the third party library cJSON.h, save a config manager cm to a JSON file filename
// return 0 for load successfully
// return -1 for invalid parameters, error writting file
//
int save_config_to_file(ConfigManager* cm, const char* filename);

//helper function for test, fetch and print all values from the configuration
// 
// input exist config manager cm, print all items.
// if cm is null, exit the function.
//
void print_config_values(ConfigManager* cm);

#endif // zhaoba_CONFIG_MANAGER_H

#include "zhaoba_config_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define CONFIG_FILE_PATH "config.json"



// Union to store values of different types (int, float, string, and arrays of them)
union Value {
    int intValue;
    float floatValue;
    char* stringValue;
    int* intArrayValue;
    float* floatArrayValue;
    char** stringArrayValue;
};

// Struct to store a key-value pair
struct KeyValuePair {
    char* key;
    Value value;
    ValueType type;
    size_t arraySize;
};

// Struct to represent the configuration manager
struct ConfigManager {
    KeyValuePair* records;
    size_t size;
    size_t capacity;
};

// Create a new key-value pair
KeyValuePair create_key_value_pair(const char* key, void* value, ValueType type, size_t arraySize) {
    KeyValuePair kv;
    kv.key = NULL;
    kv.arraySize = 0;
    int error = 0;

    if (!key) {
        printf("Key cannot be NULL.\n");
        return kv;
    }

    kv.key = _strdup(key);
    kv.type = type;
    kv.arraySize = arraySize;

    switch (type) {
    case INT:
        kv.value.intValue = *(int*)value;
        break;
    case FLOAT:
        kv.value.floatValue = *(float*)value;
        break;
    case STRING:
        kv.value.stringValue = _strdup((char*)value);
        if (!kv.value.stringValue) error = 1;
        break;
    case INT_ARRAY:
        kv.value.intArrayValue = (int*)malloc(arraySize * sizeof(int));
        if (!kv.value.intArrayValue) {
            printf("Memory allocation for int array failed.\n");
            error = 1;
        }
        else {
            memcpy(kv.value.intArrayValue, value, arraySize * sizeof(int));
        }
        break;
    case FLOAT_ARRAY:
        kv.value.floatArrayValue = (float*)malloc(arraySize * sizeof(float));
        if (!kv.value.floatArrayValue) {
            printf("Memory allocation for float array failed.\n");
            error = 1;
        }
        else {
            memcpy(kv.value.floatArrayValue, value, arraySize * sizeof(float));
        }
        break;
    case STRING_ARRAY:
        kv.value.stringArrayValue = (char**)malloc(arraySize * sizeof(char*));
        if (!kv.value.stringArrayValue) {
            printf("Memory allocation for string array failed.\n");
            error = 1;
        }
        else {
            for (size_t i = 0; i < arraySize; ++i) {
                kv.value.stringArrayValue[i] = _strdup(((char**)value)[i]);
                if (!kv.value.stringArrayValue[i]) {
                    printf("Memory allocation for string array element failed.\n");
                    for (size_t j = 0; j < i; ++j) {
                        free(kv.value.stringArrayValue[j]);
                    }
                    free(kv.value.stringArrayValue);
                    kv.value.stringArrayValue = NULL;
                    error = 1;
                    break;
                }
            }
        }
        break;
    default:
        printf("Error: Unsupported ValueType.\n");
        error = 1;
        break;
    }

    if (error) {
        free(kv.key);
        kv.key = NULL;  // Mark as invalid
    }

    return kv;
}


// Initialize a new configuration manager
ConfigManager* create_config_manager() {
    ConfigManager* cm = (ConfigManager*)malloc(sizeof(ConfigManager));
    if (!cm) {
        printf("Memory allocation for ConfigManager failed.\n");
        return NULL;
    }

    cm->capacity = 10;   
    cm->size = 0;

    cm->records = (KeyValuePair*)malloc(cm->capacity * sizeof(KeyValuePair));
    if (!cm->records) {
        printf("Memory allocation for records array failed.\n");
        free(cm);  
        return NULL;
    }

    for (size_t i = 0; i < cm->capacity; ++i) {
        cm->records[i].key = NULL;
        cm->records[i].value.stringValue = NULL;
        cm->records[i].type = -1;  
        cm->records[i].arraySize = 0;
    }

    return cm;
}



// Free the memory used by a configuration manager
    void free_config_manager(ConfigManager* cm) {
        if (cm != NULL) {
            for (size_t i = 0; i < cm->size; ++i) {
                if (cm->records[i].key) {
                    free(cm->records[i].key);
                }

                switch (cm->records[i].type) {
                case STRING:
                    if (cm->records[i].value.stringValue) {
                        free(cm->records[i].value.stringValue);
                    }
                    break;

                case STRING_ARRAY:
                    if (cm->records[i].value.stringArrayValue) {
                        for (size_t j = 0; j < cm->records[i].arraySize; ++j) {
                            if (cm->records[i].value.stringArrayValue[j]) {
                                free(cm->records[i].value.stringArrayValue[j]);
                            }
                        }
                        free(cm->records[i].value.stringArrayValue);
                    }
                    break;

                case INT_ARRAY:
                    if (cm->records[i].value.intArrayValue) {
                        free(cm->records[i].value.intArrayValue);
                    }
                    break;

                case FLOAT_ARRAY:
                    if (cm->records[i].value.floatArrayValue) {
                        free(cm->records[i].value.floatArrayValue);
                    }
                    break;

                case INT:
                case FLOAT:
                    break;

                default:
                    printf("Warning: Unrecognized type in free_config_manager.\n");
                    break;
                }
            }
            free(cm->records);
            free(cm);
        }
    }


// Store a value by key
    int store_value_by_key(ConfigManager* cm, const char* key, void* value, ValueType type, size_t arraySize) {
        if (!cm || !key || !value) {
            return -1;  
        }

        for (size_t i = 0; i < cm->size; ++i) {
            if (strcmp(cm->records[i].key, key) == 0) {
                if (cm->records[i].type != type) {
                    printf("Type mismatch. Cannot store value of type %d for key %s (current type: %d).\n", type, key, cm->records[i].type);
                    return -1;
                }

                switch (type) {
                case INT:
                    cm->records[i].value.intValue = *(int*)value;
                    break;

                case FLOAT:

                    cm->records[i].value.floatValue = *(float*)value;
                    break;

                case STRING:
                    free(cm->records[i].value.stringValue);  
                    cm->records[i].value.stringValue = _strdup((char*)value); 
                    break;

                case INT_ARRAY:
                    free(cm->records[i].value.intArrayValue);  
                    cm->records[i].value.intArrayValue = (int*)malloc(arraySize * sizeof(int));
                    if (!cm->records[i].value.intArrayValue) {
                        return -1;  
                    }
                    memcpy(cm->records[i].value.intArrayValue, value, arraySize * sizeof(int));  
                    cm->records[i].arraySize = arraySize;
                    break;

                case FLOAT_ARRAY:
                    free(cm->records[i].value.floatArrayValue);  
                    cm->records[i].value.floatArrayValue = (float*)malloc(arraySize * sizeof(float));
                    if (!cm->records[i].value.floatArrayValue) {
                        return -1;  
                    }
                    memcpy(cm->records[i].value.floatArrayValue, value, arraySize * sizeof(float));  
                    cm->records[i].arraySize = arraySize;
                    break;

                case STRING_ARRAY:
                    if (cm->records[i].value.stringArrayValue) {
                        for (size_t j = 0; j < cm->records[i].arraySize; ++j) {
                            free(cm->records[i].value.stringArrayValue[j]);
                        }
                        free(cm->records[i].value.stringArrayValue);
                    }

                    cm->records[i].value.stringArrayValue = (char**)malloc(arraySize * sizeof(char*));
                    if (!cm->records[i].value.stringArrayValue ) {
                        return -1; 
                    }

                    for (size_t j = 0; j < arraySize; ++j) {
                        cm->records[i].value.stringArrayValue[j] = _strdup(((char**)value)[j]);
                    }
                    cm->records[i].arraySize = arraySize;
                    break;

                default:
                    return -1; 
                }
                return 0; 
            }
        }

        if (cm->size >= cm->capacity) {
            cm->capacity *= 2;  
            cm->records = realloc(cm->records, cm->capacity * sizeof(KeyValuePair));
            if (!cm->records) {
                return -1; 
            }
        }

        cm->records[cm->size++] = create_key_value_pair(key, value, type, arraySize);
        return 0;  
    }


// Fetch a value by key with error handling
int fetch_value_by_key(ConfigManager* cm, const char* key, void* valueOut, ValueType expectedType) {
    if (!cm || !key || !valueOut ) {
        return -1;  
    }

    for (size_t i = 0; i < cm->size; ++i) {
        if (cm->records[i].key && strcmp(cm->records[i].key, key) == 0) {


            if (cm->records[i].type != expectedType) {
                printf("Type mismatch: Expected type does not match stored type for key '%s'.\n", key);
                return -1;
            }

            switch (expectedType) {
            case INT:
                *(int*)valueOut = cm->records[i].value.intValue;
                break;
            case FLOAT:
                *(float*)valueOut = cm->records[i].value.floatValue;
                break;
            case STRING:
                *(char**)valueOut = cm->records[i].value.stringValue;
                break;
            case INT_ARRAY:
                memcpy(valueOut, cm->records[i].value.intArrayValue, cm->records[i].arraySize * sizeof(int));
                break;
            case FLOAT_ARRAY:
                memcpy(valueOut, cm->records[i].value.floatArrayValue, cm->records[i].arraySize * sizeof(float));
                break;
            case STRING_ARRAY:
                memcpy(valueOut, cm->records[i].value.stringArrayValue, cm->records[i].arraySize * sizeof(char*));
                break;
            default:
                return -1;
            }

            return 0;  
        }
    }

    printf("Key '%s' not found.\n", key);
    return -1;  
}

int load_config_from_file(ConfigManager* cm, const char* filename) {
    if (!cm || !filename) {
        return -1;  
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return -1;
    }


    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* fileContent = (char*)malloc(fileSize + 1);
    if (!fileContent) {
        fclose(file);
        return -1;  
    }
    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';  
    fclose(file);


    cJSON* root = cJSON_Parse(fileContent);
    free(fileContent);  
    if (!root) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return -1;
    }

    cJSON* item = NULL;
    cJSON_ArrayForEach(item, root) {
        if (!cJSON_IsObject(item)) continue;

        const char* key = cJSON_GetObjectItem(item, "key")->valuestring;
        const char* typeStr = cJSON_GetObjectItem(item, "type")->valuestring;
        ValueType type;

        if (strcmp(typeStr, "INT") == 0) {
            type = INT;
            int intValue = cJSON_GetObjectItem(item, "value")->valueint;
            store_value_by_key(cm, key, &intValue, INT, 0);
        }
        else if (strcmp(typeStr, "FLOAT") == 0) {
            type = FLOAT;
            float floatValue = (float)cJSON_GetObjectItem(item, "value")->valuedouble;
            store_value_by_key(cm, key, &floatValue, FLOAT, 0);
        }
        else if (strcmp(typeStr, "STRING") == 0) {
            type = STRING;
            const char* stringValue = cJSON_GetObjectItem(item, "value")->valuestring;
            store_value_by_key(cm, key, (void*)stringValue, STRING, 0);
        }
        else if (strcmp(typeStr, "INT_ARRAY") == 0) {
            type = INT_ARRAY;
            cJSON* arrayItem = cJSON_GetObjectItem(item, "value");
            size_t arraySize = cJSON_GetArraySize(arrayItem);
            int* intArray = (int*)malloc(arraySize * sizeof(int));
            for (size_t i = 0; i < arraySize; ++i) {
                intArray[i] = cJSON_GetArrayItem(arrayItem, i)->valueint;
            }
            store_value_by_key(cm, key, intArray, INT_ARRAY, arraySize);
            free(intArray);
        }
        else if (strcmp(typeStr, "FLOAT_ARRAY") == 0) {
            type = FLOAT_ARRAY;
            cJSON* arrayItem = cJSON_GetObjectItem(item, "value");
            size_t arraySize = cJSON_GetArraySize(arrayItem);
            float* floatArray = (float*)malloc(arraySize * sizeof(float));
            for (size_t i = 0; i < arraySize; ++i) {
                floatArray[i] = (float)cJSON_GetArrayItem(arrayItem, i)->valuedouble;
            }
            store_value_by_key(cm, key, floatArray, FLOAT_ARRAY, arraySize);
            free(floatArray);
        }
        else if (strcmp(typeStr, "STRING_ARRAY") == 0) {
            type = STRING_ARRAY;
            cJSON* arrayItem = cJSON_GetObjectItem(item, "value");
            size_t arraySize = cJSON_GetArraySize(arrayItem);
            char** stringArray = (char**)malloc(arraySize * sizeof(char*));
            for (size_t i = 0; i < arraySize; ++i) {
                const char* strValue = cJSON_GetArrayItem(arrayItem, i)->valuestring;
                stringArray[i] = _strdup(strValue);
            }
            store_value_by_key(cm, key, stringArray, STRING_ARRAY, arraySize);

            for (size_t i = 0; i < arraySize; ++i) {
                free(stringArray[i]);
            }
            free(stringArray);
        }
    }

    cJSON_Delete(root);  
    return 0;  
}



// Save configuration data to a file
int save_config_to_file(ConfigManager* cm, const char* filename) {
    if (!cm || !filename) {
        return -1;  
    }


    cJSON* root = cJSON_CreateArray();
    if (!root) {
        return -1;  
    }

    for (size_t i = 0; i < cm->size; ++i) {
        cJSON* item = cJSON_CreateObject();
        if (!item) continue;

        cJSON_AddStringToObject(item, "key", cm->records[i].key);

        const char* typeStr = NULL;
        switch (cm->records[i].type) {
        case INT: typeStr = "INT"; break;
        case FLOAT: typeStr = "FLOAT"; break;
        case STRING: typeStr = "STRING"; break;
        case INT_ARRAY: typeStr = "INT_ARRAY"; break;
        case FLOAT_ARRAY: typeStr = "FLOAT_ARRAY"; break;
        case STRING_ARRAY: typeStr = "STRING_ARRAY"; break;
        default: break;
        }
        cJSON_AddStringToObject(item, "type", typeStr);

        cJSON* valueItem = NULL;
        if (cm->records[i].type == INT) {
            valueItem = cJSON_CreateNumber(cm->records[i].value.intValue);
        }
        else if (cm->records[i].type == FLOAT) {
            valueItem = cJSON_CreateNumber(cm->records[i].value.floatValue);
        }
        else if (cm->records[i].type == STRING) {
            valueItem = cJSON_CreateString(cm->records[i].value.stringValue);
        }
        else if (cm->records[i].type == INT_ARRAY) {
            valueItem = cJSON_CreateArray();
            for (size_t j = 0; j < cm->records[i].arraySize; ++j) {
                cJSON_AddItemToArray(valueItem, cJSON_CreateNumber(cm->records[i].value.intArrayValue[j]));
            }
        }
        else if (cm->records[i].type == FLOAT_ARRAY) {
            valueItem = cJSON_CreateArray();
            for (size_t j = 0; j < cm->records[i].arraySize; ++j) {
                cJSON_AddItemToArray(valueItem, cJSON_CreateNumber(cm->records[i].value.floatArrayValue[j]));
            }
        }
        else if (cm->records[i].type == STRING_ARRAY) {
            valueItem = cJSON_CreateArray();
            for (size_t j = 0; j < cm->records[i].arraySize; ++j) {
                cJSON_AddItemToArray(valueItem, cJSON_CreateString(cm->records[i].value.stringArrayValue[j]));
            }
        }

        cJSON_AddItemToObject(item, "value", valueItem);
        cJSON_AddNumberToObject(item, "arraySize", cm->records[i].arraySize);  
        cJSON_AddItemToArray(root, item);
    }

    FILE* file = fopen(filename, "w");
    if (!file) {
        cJSON_Delete(root);
        return -1;  
    }

    char* jsonString = cJSON_Print(root);
    fwrite(jsonString, sizeof(char), strlen(jsonString), file);
    fclose(file);


    free(jsonString);
    cJSON_Delete(root);
    return 0;  
}

// fetch and print all values from the configuration
void print_config_values(ConfigManager* cm) {

    int fetchedInt;
    if (fetch_value_by_key(cm, "key_int", &fetchedInt, INT) == 0) {
        printf("Fetched INT value: %d\n", fetchedInt);
    }

    float fetchedFloat;
    if (fetch_value_by_key(cm, "key_float", &fetchedFloat, FLOAT) == 0) {
        printf("Fetched FLOAT value: %.2f\n", fetchedFloat);
    }

    char* fetchedString;
    if (fetch_value_by_key(cm, "key_string", &fetchedString, STRING) == 0) {
        printf("Fetched STRING value: %s\n", fetchedString);
    }

    int fetchedIntArray[5];
    if (fetch_value_by_key(cm, "key_int_array", fetchedIntArray, INT_ARRAY) == 0) {
        printf("Fetched INT_ARRAY value: ");
        for (int i = 0; i < 5; ++i) {
            printf("%d ", fetchedIntArray[i]);
        }
        printf("\n");
    }

    float fetchedFloatArray[3];
    if (fetch_value_by_key(cm, "key_float_array", fetchedFloatArray, FLOAT_ARRAY) == 0) {
        printf("Fetched FLOAT_ARRAY value: ");
        for (int i = 0; i < 3; ++i) {
            printf("%.2f ", fetchedFloatArray[i]);
        }
        printf("\n");
    }

    char* fetchedStringArray[3];
    if (fetch_value_by_key(cm, "key_string_array", fetchedStringArray, STRING_ARRAY) == 0) {
        printf("Fetched STRING_ARRAY value: ");
        for (int i = 0; i < 3; ++i) {
            printf("%s ", fetchedStringArray[i]);
        }
        printf("\n");
    }
}


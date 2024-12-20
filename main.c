#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zhaoba_config_manager.h"



int main() {
    // Create and populate the initial ConfigManager
    ConfigManager* cm = create_config_manager();
    if (!cm) {
        printf("Failed to create config manager.\n");
        return 1;
    }

    // Store values in the configuration manager
    int intValue = 42;
    store_value_by_key(cm, "key_int", &intValue, INT, 0);

    float floatValue = 3.14f;
    store_value_by_key(cm, "key_float", &floatValue, FLOAT, 0);
    store_value_by_key(cm, "key_float", &floatValue, INT, 0);

    const char* stringValue = "Hello, World!";
    store_value_by_key(cm, "key_string", (void*)stringValue, STRING, 0);

    int intArray[] = { 1, 2, 3, 4, 5 };
    store_value_by_key(cm, "key_int_array", intArray, INT_ARRAY, 5);

    float floatArray[] = { 1.1f, 2.2f, 3.3f };
    store_value_by_key(cm, "key_float_array", floatArray, FLOAT_ARRAY, 3);

    const char* stringArray[] = { "apple", "banana", "cherry" };
    store_value_by_key(cm, "key_string_array", stringArray, STRING_ARRAY, 3);

    // Display original configuration
    printf("Original configuration values:\n");
    print_config_values(cm);

    // Save the configuration to a file
    printf("\n test of \"saving the configuration to a json file\".\n");
    const char* filename = "config.json";
    if (save_config_to_file(cm, filename) != 0) {
        printf("Failed to save config to file.\n");
    }
    else {
        printf("Congrats! Configuration successfully saved to %s\n", filename);
    }

    // Free the original ConfigManager
    free_config_manager(cm);


    // Create a new ConfigManager and load the data from the file
    printf("\n test of \"loading from json file\".\n");
    ConfigManager* cmLoaded = create_config_manager();
    if (!cmLoaded) {
        printf("Failed to create config manager for loading.\n");
        return 1;
    }

    if (load_config_from_file(cmLoaded, filename) != 0) {
        printf("Failed to load config from file.\n");
        free_config_manager(cmLoaded);
        return 1;
    }
    else {
        printf("Configuration successfully loaded from %s\n", filename);
    }

    // Display loaded configuration to verify correct data loading
    printf("Loaded configuration values:\n");
    print_config_values(cmLoaded);

    // Free the loaded ConfigManager
    free_config_manager(cmLoaded);

    return 0;
}

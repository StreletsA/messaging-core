# Java wrapper

## Instruction
### Run
1. Specify properties file path in [LibraryLoader](messaging-core-java/lib/src/main/java/messaging/core/utils/LibraryLoader.java) class (variable RESOURCE_FILE_PATH)
2. Specify an absolute java core path in config.properties file

### Updating native functions
1. Rewrite native functions
2. Comment imports and rows with imported classes (we don't want to add these classes to classpath in next step)
3. Run command:
```text
javac -h . YourClass.java
```
4. Move the .h file to [core/java](../../core/java/) folder
5. Create a .cpp file
6. Implement methods from your .h file
7. Run script:
```text
./lib_builder.sh
```
If you added a new class you should to castomize [lib_builder.sh](../../core/java/lib_builder.sh)

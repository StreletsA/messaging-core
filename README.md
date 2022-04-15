# Messaging Core
### Running PUB/SUB Sequence:
![alt text](Messaging-core-1.png)
### Start-up PUB/SUB Sequence:
![alt text](Messaging-core-2.png)

## How to use in Java?
1) Create <b>libs</b> directory in your Java project
2) Add <b>lib-all.jar</b> file to <b>libs</b>
3) Add commands to your gradle file:
```text
repositories {

    flatDir {
        dirs 'libs'
    }
    
    ...
    
}

dependencies {

    implementation name: 'lib-all'
    
    ...
    
}
```

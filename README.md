# SPUprocessorEmulator

Hello everyone, first of all this was an educational project, so some errors can occur, please, don't judge author too strictly.
Before using this project you have to first install required libs (make sure that you pulled needed submodules). In order to do so, run this commands from project root folder:

To install logger lib
```
make install -C ./external/LoggerLib
```

To install stack structure lib
```
make install -C ./external/StackStruct
```

How to use this project:
First of all, some important libs need to be installed before usage. Also it may require to additionaly install SFML lib. Then you need to write some assembly code in `program.asm` file. After that go to `assembler` folder and run `make run` command. You have to do this only after you made some changes to `program.asm file`. To run program you have to go `processor` folder and run `make run` command.

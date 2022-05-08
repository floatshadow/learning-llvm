## Use LLVM IR Builder to construct toy compiler

### Copyright Notice
These codes are from one of the LLVM official tutorials, [My First Language Frontend with LLVM tutorial](https://www.llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html).
I made some modification referring to `Clang` frontend source codes.

### Why IR Builder 
The reason why `IR builder` is important for me is listed below:
- It offers a way to have your front-end plugin (hand-written or other tools) into LLVM middle end target-independent opt passes and backend.
- It's not a bad way to produce LLVM IR for debug even if you are designing your own IR.
- IR builder has connections with how LLVM IR's memory representation, like `Value` class.
- Some optimization should be done on AST, e.g. unused declared variables.
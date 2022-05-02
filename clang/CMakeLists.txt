llvm_map_components_to_libnames(LLVM_LIBS support core)
# weried problem that if use `clang` rather than `libclang`
# link options would not contains libclang.so
set(CLANG_LIBS 
    "libclang;clangFrontend;clangDriver;clangSerialization;clangParse;clangSema;clangAnalysis;clangEdit;clangAST;clangLex;clangBasic;clangTooling"
)


add_executable(diagnose diagnose.cc)
target_link_libraries(diagnose
    PRIVATE ${CLANG_LIBS} ${LLVM_LIBS}
)
